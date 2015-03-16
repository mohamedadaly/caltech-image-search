// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010


//#include <stdlib.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <algorithm>

#include "ccCommon.hpp"
#include "ccHKmeans.hpp"


//random number function to use with random_shuffle
ptrdiff_t hkm_random_shuffle_rand(ptrdiff_t s) { return rand() % s; }

#define MIN(A,B) (A)<(B) ? (A) : (B)
#define URAND ((double)rand() / ((double)RAND_MAX + 1.))

//get a random number between [M1, M2)
#define RAND(M1, M2) (URAND * ((M2)-(M1)) + (M1))
//random number between [M1, M2-1]
#define IRAND(M1, M2) floor(RAND(M1,M2))


//-----------------------------------------------------------------------
// get the nearest mean to the input point
template <class T>
HkmClassId HkmNode<T>::getNearMean1(Data<T> &point, HkmOptions &opt,
        uint* cids, float* dists)
{
//   uint cid;
//   float dist;

  //use kdtree?
  if (opt.usekdt)
  {
//    cout << "creating kdt: " << this->kdf << endl;
    //create kdf if doesn't exist
    if (!this->kdf)
    {      
      //create the kdtree for the means
      this->kdf = new Kdf(opt.kdtopt);
      if (!this->kdf)
        cout << "Memory error";      
      create(*this->kdf, this->means);
    }
    
    //get nearest neighbor
    getKnn(*this->kdf, this->means, point, this->means.size(), cids, dists);
//     getKnn(*this->kdf, this->means, point, 1, &cid, &dist);
//    //## change this!!
//    this->clearKdf();
  }
  //just use exhaustive search
  else
  {
    knn(dists, cids, this->means.size(), point, this->means, opt.dist);
//     knn(&dist, &cid, 1, point, this->means, opt.dist);
  }
  
  
//  cout << " near class: " << cid << " & dist: " << dist << endl;
  
  //return
  return (HkmClassId) cids[0];
}

//-----------------------------------------------------------------------
/// gets the nearest mean for the input data
template <class T>
bool HkmNode<T>::getNearMean(Data<T> &data, HkmOptions &opt, 
        uint* ids, float* dists)
{
  //same ids like last iteration
  bool same = true;
  
  //number of points
  uint npoints = data.size();
  uint nmeans = this->means.size();
//  cout << "getNearMean:npoints " << data.size() << endl;

  //allocate ids and dists arrays
//   uint *ids = new uint[npoints];
//   float *dists = new float[npoints];
  
  //use kdtree?
  if (opt.usekdt)
  {
    //build the tree
    this->buildKdf(opt.kdtopt);
    
    //get the nearest means using the kdtree
    getKnn(*this->kdf, this->means, data, 1, ids, dists);
    
    //clear kdf
    this->clearKdf();
//    cout << "cleared kdf: " << this->kdf << endl;
  }
  else
  {
    knn(dists, ids, 1, data, this->means, opt.dist);
  }
  
  //put back ids and check
  for (uint i=0; i<npoints; ++i)
    if (this->classIds[i] != ids[i] && ids[i]<nmeans)
    {
      same = false;
      this->classIds[i] = ids[i];
    }
  
//  if (data.filterId(0) == 0)
//    cout << " point 0 -> dist:" << dists[0] << " id:" << ids[0] << endl;
//  cout << endl;
//  for (uint i=0; i<classIds.size(); ++i)
//     cout << " " << classIds[i]; cout << endl;   
//  cout << "  finished" << endl;
        
  //clear
//   delete [] ids;
//   delete [] dists;
  
  //return
  return same;
}

//-----------------------------------------------------------------------
/// gets the means
template <class T>
void HkmNode<T>::getMeans(Data<T> &data, Data<double>& tmeans, HkmOptions &opt, 
        uint* meanpoints)
{
  
//  cout << "getMeans:npoints " << data.size() << endl;
  
  //now loop and compute
  uint npoints = data.size();
  uint nmeans = this->means.size();
  
  //array to hold number of points of every mean
//   uint *meanpoints = new uint[nmeans];
  fill(meanpoints, meanpoints + nmeans, 0);
  
  //reset means to zero
  fill(tmeans.getFixed(), tmeans.getFixed() + tmeans.ndims*tmeans.npoints, 0.);
    
  //loop on the points in data
  for (uint i=0; i<npoints; ++i)
  {
    //get point
    pair<T*, uint> p = data.getPoint(i);
    
    //update count
    meanpoints[this->classIds[i]]++;
    
    //update its mean
//    pair<T*, uint> pm = this->means.getPoint(this->classIds[i]);
    pair<double*, uint> pm = tmeans.getPoint(this->classIds[i]);
    for (uint j=0; j<pm.second; ++j)	pm.first[j] += (double)p.first[j];
  } //for i
  
  //divide every mean by its number
  for (uint k=0; k<nmeans; ++k)
  {
    //get mean
    pair<T*, uint> pm = this->means.getPoint(k);
    pair<double*, uint> ptm = tmeans.getPoint(k);
    //check if its points > 0
    if (meanpoints[k])
    {
      for (uint j=0; j<pm.second; ++j)	
        pm.first[j] = (T) (ptm.first[j] / meanpoints[k]);
    }
    //this mean had no points
    else
    {
      //grab a random point
      uint rid = (uint) IRAND(0, npoints); //rand() % npoints
      pair<T*, uint> p = data.getPoint(rid);
//      cout << " empty mean: " << k << " id: " << rid << endl;
      //copy
      copy(p.first, p.first + p.second, pm.first);
    }
  }
  
  //clear
//   delete [] meanpoints;
}

//-----------------------------------------------------------------------
/// performs kmeans on the data in this node
template <class T>
void HkmNode<T>::initMeans(Data<T> &data, HkmOptions &opt)
{
  //number of points
  uint npoints = data.size();
  
//  cout << "initMeans:npoints " << data.size() << endl;
  
  //init means with random points
  this->means.clear();
  this->means.init(DATA_FIXED, MIN(opt.nbranches, npoints), data.ndims);
  this->means.allocate();
  //mean stds
  this->meanStds.resize(opt.nbranches);

//   this->means.type = DATA_FIXED;
//   this->means.npoints = MIN(opt.nbranches, npoints);
//   this->means.ndims = data.ndims;

  
//  //loop
//  for (uint i=0; i<opt.nbranches; ++i)
//  {
//    //get a random point
//    uint rid = (uint) IRAND(0, npoints); //rand() % npoints;
//    pair<T*, uint> p = data.getPoint(rid);
//    //put it into means
//    pair<T*, uint> pm = this->means.getPoint(i);
//    copy(p.first, p.first + p.second, pm.first);
//    
////    cout << " random id: " << rid << endl;
//  }
  
  //init the classIds and fill randomly
//   cout << "npoints:" << npoints << endl;
  this->classIds.resize(npoints);
//  fill(this->classIds.begin(), this->classIds.end(), 0);
  for (uint i=0; i<npoints; ++i)
    this->classIds[i] = (HkmClassId) IRAND(0, opt.nbranches);

  //set to visited
  this->visited = true;
}

//-----------------------------------------------------------------------
/// performs kmeans on the data in this node
template <class T>
void HkmNode<T>::kmeans(Data<T> &data, Data<double>& tmeans, HkmOptions &opt)
{
//  //check if we need to do something?
//  if (data.size() < opt.nbranches) return;
  
  //init means with random class memberships
  this->initMeans(data, opt);
  
  if (data.size() <= opt.nbranches)
  {
    //copy data to means
    for (uint i=0, s=data.size(); i<s; ++i)
    {
      pair<T*, uint> p = data.getPoint(i);
      pair<T*, uint> pm = this->means.getPoint(i);
      copy(p.first, p.first + p.second, pm.first);
      this->classIds[i] = i;
      //set std to 0
      this->meanStds[i] = 0;
    }
    //resize means
    this->means.npoints = data.size();
    this->meanStds.resize(data.size());
    return;
  }

  uint nmeans = means.size();
  uint npoints = data.size();
  //mean points used inside getMeans
  uint *meanpoints = new uint[nmeans];
  //ids and dists used inside getNearMean
  uint *ids = new uint[npoints];
  float *dists = new float[npoints];
  
//  cout << "kmeans:npoints " << data.size() << endl;
      
  //main loop
  for (int iter=0; iter<opt.niters; ++iter)
  {
    //compute the current means
    this->getMeans(data, tmeans, opt, meanpoints);
    
    //get new class memberships, and break if the same ids like last iteration
    if (this->getNearMean(data, opt, ids, dists))    
      break;
  }
  
  //sum & sum2 to collect stats
  float* sum = new float[nmeans];
  float* sum2 = new float[nmeans];
  fill(sum, sum+nmeans, 0);
  fill(sum2, sum2+nmeans, 0);
  
  //compute std deviation of distances
  fill(meanpoints, meanpoints+nmeans, 0);
  //loop on points
  for (uint i=0; i<npoints; ++i)
  {
    HkmClassId mid = this->classIds[i];
    //update meanpoints
    meanpoints[mid]++;
    //update sum & sum2
    float t = dists[i];
    sum[mid] += t;
    sum2[mid] += t*t;
  }
  //compute std
  for (uint i=0; i<nmeans; ++i)
  {    
    this->meanStds[i] = meanpoints[i]>0 ? 
      sqrt((sum2[i] - sum[i]*sum[i]/meanpoints[i]) / meanpoints[i]) : 0;
  }
  
  
////    cout << "iter: " << iter << endl;
//    //compute distance to nearest mean
//    bool same = this->getNearMean(data, opt);
////    cout << " same:" << same ;
//    
//    //check if same means as in last iteration
//    if (!same)
//      //re-compute means
//      this->getMeans(data, tmeans, opt);
//    else
//      //exit
//      break;
  
  //clear
  delete [] meanpoints;
  delete [] ids;
  delete [] dists;
  delete [] sum;
  delete [] sum2;
}

//-----------------------------------------------------------------------
/// create the HKM forest
template <class T>
void Hkms<T>::create(Data<T> &data)
{
  //initialize random number generator
  srand(0x1234);
  
  //loop on hkms and create
  this->resize(this->opt.ntrees);
  for (uint i=0; i<this->opt.ntrees; ++i)
  {
    this->hkms[i].opt = this->opt;
    this->hkms[i].create(data, this->opt);
  }

}
//-----------------------------------------------------------------------
/// create the HKM tree
template <class T>
void Hkm<T>::create(Data<T> &data, HkmOptions& opt)
{
  
  //allocate node list
  uint npoints = data.size();
  double nnodes = 1;
  for (int l=1; l<opt.nlevels; l++) 
    nnodes+=pow((double)opt.nbranches, l);
  this->resize((uint)nnodes);
  
//  cout << "nlevels:" << opt.nlevels << endl;
//  cout << "nbranches:" << opt.nbranches << endl;
//  cout << "nnodes:" << nnodes << endl;
//  cout << "npoints:" << npoints << endl;
  
  //initialize root with full list
  HkmNode<T>& root = this->at(1);
  root.resize(npoints);
  for (uint i=0; i<npoints; ++i) root.plist[i] = i;
  
  //make a temp means data to compute means
  Data<double> tmeans(DATA_FIXED, opt.nbranches, data.ndims);
  tmeans.allocate();
  
  //loop on the tree starting at the root
  for (HkmNodeId n=1; n<=nnodes; ++n)
  {
//    cout << "node: " << n << endl;
    
    //get the node
    HkmNode<T>& node = this->at(n);
    
    //if empty plist, just skip
    if (node.size() == 0) continue;

    //set the data filter
    data.setFilter(& node.plist);
    
//    cout << "node:" << n  << " has data " << data.size() << 
//            " and plist: " << node.plist.size() << endl;
    
    //do kmeans at this node
    node.kmeans(data, tmeans, opt);    
    
//    if (data.filterId(0) == 0)
//      cout << "node: " << n << " point 0: id: " << data.filterId(0) << " classid: "  << 
//            node.classIds[0] << endl;
    
    //clear data filter
    data.clearFilter();
    
    //get node depth
    int depth = this->getDepth(n);

//    cout << " finished kmeans with depth " << depth << endl;


//    cout << "node:" << n  << " has means " << node.means.size() << 
//            " and plist " << node.size() << endl;
    
    //not last level, then put ids onto children if have enough
    if (depth+1 < opt.nlevels)
//        && node.size() > this->opt.nbranches*this->opt.nbranches)
    { 
      //get id of first child
      HkmNodeId childId = this->getChildId(n);
      
      //fill in the point list for the children
//       cout << "childId:" << childId + node.classIds[i] << "size:" << this->size() << endl;        
      for (uint i=0; i<node.size(); ++i)
      {
//         cout << "childId:" << childId + node.classIds[i] << "size:" << this->size() << endl;        
//         cout << "plist size="<< this->at(childId + node.classIds[i]).plist.size() << endl;
        this->at(childId + node.classIds[i]).plist.push_back(node.plist[i]);
      }
      
//      for (uint i=0; i<node.size(); ++i)
//        cout << " " << node.classIds[i]; cout << endl;
      
      //clear lists: plist and classIds
      node.clearLists();
    }
    
//    //display first mean
//    cout << endl;
//    for (uint j=0; j<node.means.size(); ++j)
//    {
//      cout << "point: " << j << endl;
//      for (uint i=0; i<data.ndims; ++i)
//        cout << " " << node.means.getPointVal(j, i); 
//      cout << endl;
//    }
  } //for n
  
  
  
  //clear tmeans
  tmeans.clear();
}

//-----------------------------------------------------------------------
/// descend down the tree starting from a given node
template <class T>
HkmNodeId Hkm<T>::descend(HkmNodeId start, Data<T>& data, uint pid, 
        HkmClassId &meanid, HkmBranchQ* pq, HkmOptions& opt, uint tid)
{
  HkmNodeId end = start;
  meanid = 0;

  //return if not a valid id
  if (!this->validId(end))
    return end;

  //get the data point as a data object
  pair<T*, uint> p = data.getPoint(pid);
  Data<T> point(DATA_FIXED, 1, p.second);
  point.setFixed(p.first, false);
  
  //arrays to hold list of ids and dists to means
//   uint* cids = new uint[opt.nbranches];
//   float* dists = new float[opt.nbranches];
  
  //loop through starting at this node
  while (true)
  {
    
    //get the node
    HkmNode<T>& node = this->at(end);

    //get dists to means
    fill(this->cids, this->cids+opt.nbranches, opt.nbranches+10);
    fill(this->dists, this->dists+opt.nbranches, numeric_limits<float>::infinity());
    meanid =node.getNearMean1(point, opt, this->cids, this->dists);
    
    //make nearest mean 1-based
//     meanid++;
    
    //first child id
    HkmNodeId fcid = this->getChildId(end);
    
    //add other branches to pq    
    if (pq != 0)
      for (uint i=1; i<node.means.size(); ++i)
      {
        //get child id
        HkmNodeId cid = fcid + cids[i];
        //add branch
//         HkmBranch b(cid, tid, fabs(dists[i]), meanid);
//         pq->push_back(b);  
        //#malaa: try not using the distance, make it zero and see if that will 
        //improve, branches are just explored based on their order
//         HkmBranch b(cid, tid, 0, meanid);
//         pq->push_back(b);    
        //#malaa: try using distance divided by std dev
        HkmBranch b(cid, tid, node.meanStds[cids[i]] > 0 ?
          dists[i] / node.meanStds[cids[i]] : numeric_limits<float>::infinity(), 
                  meanid);
        pq->push_back(b);    
      }
//     meanid = node.getNearMean1(point, opt) + 1;
    
    //check if to stop now
    HkmNodeId childId = fcid + meanid;
//    cout << "pid: " << pid << " node: " << end << " has meanid " << meanid << 
//            " & child " << childId << endl;
//    if (this->validId(childId))
//      cout <<  " child visited: " << this->at(childId).visited;
//    cout << endl;
    if (!this->validId(childId) || !this->at(childId).visited)
      break;
    
    //put the right child as the next node to check
    end = childId;    
  }
  
  //clear
  point.clear();
//   delete [] cids;
//   delete [] dists;
  
  //return the current valid node
  return end;  
}

//-----------------------------------------------------------------------
/// get leaf ids for input data
template <class T>
void Hkms<T>::getLeafIds(Data<T> &data, HkmClassId *cids)
{
  // initialize for descend
  this->initDescend();
  
  //number of points
  uint npoints = data.size();
  
  //get the starting id of the first leaf (first nodedid outside the current 
  //tree), where leaf means it's not in the tree
  HkmNodeId sleaf = this->hkms[0].getLeafId() + 1;
  //get the id of the first node in the last level
  HkmNodeId pleaf = this->hkms[0].getParentId(sleaf);
  
  //loop on the points
  for (uint i=0; i<npoints; ++i)
  {
    //get the class id for this point
    HkmClassId cid;
    
    //loop on the trees
    for (uint t=0; t<this->opt.ntrees; ++t)
    {      
      cid = this->hkms[t].getLeafId(data, i, sleaf, pleaf, this->opt);
      //put back
      cids[t+this->opt.ntrees*i] = cid;
    }    
  }
  
  // initialize for descend
  this->finishDescend();  
}

//-----------------------------------------------------------------------
/// get leaf ids for input data
template <class T>
HkmClassId Hkm<T>::getLeafId(Data<T> &data, uint pid, HkmNodeId sleaf, 
        HkmNodeId pleaf, HkmOptions& opt)
{
  //get the class id for this point
  HkmClassId cid;
  HkmNodeId  nodeId = this->descend(this->getRootId(), data, pid, cid, 
          0, opt, 0);
  int depth = this->getDepth(nodeId);
//   cout << "nodeId:" << nodeId << ", depth:" << depth << ", levels:" << this->opt.nlevels << ",";
  //get the deepest child for nodeId on the last level
  if (depth+1 < this->opt.nlevels)
  {
    nodeId = this->getDeepChildId(this->getChildId(nodeId) + cid - 1);
    cid = 0;
  }
    
    //check its value
//    if (cid > 0)
//    {
  cid += (nodeId - pleaf) * opt.nbranches;
//   cout << "nodeId:" << nodeId << ", pleaf:" << pleaf << ", cid: " << cid << endl;
//    cid = this->getChildId(nodeId) + cid - 1;
//    }

  return cid;
}

//-----------------------------------------------------------------------
/// get Ids of points close to a given point
template <class T>
void Hkms<T>::getIds(Data<T>& data, uint pid, HkmPointList& plist)
{
  //create the priority queue
  HkmBranchQ pq;
  
  //initialize it with the root nodes in all the trees
  for (uint t=0; t<this->opt.ntrees; ++t)
  {
    HkmBranch b(1, t, 0.);
    pq.push_back(b);
  }
  
  //now loop on pq and process
  uint bins = this->opt.nchecks;
  while (bins && pq.size()>0)
  {
    //sort the queue
//    sort(pq.begin(), pq.end());
    pq.sort();
    //truncate if too large
    if (pq.size() > this->opt.nchecks)
      pq.resize(this->opt.nchecks);
    
    //get the top branch
    HkmBranch branch = pq.front();  pq.pop_front();
    
//    cout << "branch -> tid: " << branch.treeId << " nodeid: " << branch.nodeId <<
//            " dist: " << branch.dist << " mid: " << branch.meanId << endl;
    
    //go down the respective tree
    Hkm<T>& hkm = this->hkms[branch.treeId];
    HkmClassId meanid;
    HkmNodeId nid = hkm.descend(branch.nodeId, data, pid, meanid, 
            &pq, this->opt, branch.treeId);
//     HkmNodeId nid = hkm.descend(branch.nodeId, data, pid, branch.treeId, pq, kdf.opt);

    //if not valid id, then this was a branch at the lowest level of the tree
    //so we need to get back to its parent and get points with meanid in the 
    //branch
    if (!hkm.validId(nid))
    {
      //get parent and make sure its valid
      nid = hkm.getParentId(nid);
      //meanid is the meanId in the branch
      meanid = branch.meanId;
    }
    
    //check if valid (again if getting the parent
    if (hkm.validId(nid))
    {
              
      //add the list of ids with the same meanid
      HkmNode<T>& node = hkm.at(nid);
      for (uint i=0, s=node.size(); i<s; ++i)
        if (node.classIds[i] == meanid)
          plist.push_back(node.plist[i]);
//       plist.insert(plist.end(), node.plist.begin(), node.plist.end()); 

//      //show now
//      cout << " nodeid: " << nid << " visited:" << node.visited << " split.dim: " << node.split.dim << 
//              " split.val: " << node.split.val << " ids: ";
//      for (uint i=0; i<node.plist.size(); ++i) cout << " " << node.plist[i];
//      cout << endl;
        
    }
    
    //decrement
    bins--;
  }  
  //clear pq
  pq.clear();  
}

// //-----------------------------------------------------------------------
// /// get Ids of points close to a given point
// template <class T>
// void Hkm<T>::getIds(Data<T>& data, uint pid, HkmPointList& plist)
// {
//   //descend through the tree
//   HkmClassId meanId;
//   HkmNodeId nodeId = this->descend(this->getRootId(), data, pid, meanId);
//   
// //  cout << "point " << pid << " found node " << nodeId << " & meanid " << meanId << endl;
//   
//   //check if a valid meanId
//   if (meanId>0)
//   {
//     //get node
//     HkmNode<T> &node = this->at(nodeId);
// 
//     //subtract to make 0-based
//     meanId--;
// 
//     //loop on the points that have this class and add their ids
//     for (uint i=0, s=node.size(); i<s; ++i)
//       if (node.classIds[i] == meanId)
//         plist.push_back(node.plist[i]);
//   }
// }

//-----------------------------------------------------------------------
/// lookup the trees and return nearest neighbors
template <class T>
template <class Tret>
void Hkms<T>::hkmKnn(Data<T>& hkmData, Data<T>& sdata, int k, 
        uint* ids, Tret* dists)
{  
  // initialize for descend
  this->initDescend();
  
  //loop on the points
  DataFilter sfilter(1);
  for (uint i=0; i<sdata.size(); ++i)
  {
    //get list of points close to that point
    HkmPointList pl;
    this->getIds(sdata, i, pl);
    
//    cout << "point " << i << " has points:" << endl;
//    for (uint kk=0; kk<pl.size(); ++kk)
//      cout << " " << pl[kk];
//    cout << endl;
    
    //remove duplicates
    sort(pl.begin(), pl.end());
    HkmPointIt it = unique(pl.begin(), pl.end());
    pl.resize(it - pl.begin());
    //random shuffle
    random_shuffle(pl.begin(), pl.end(), hkm_random_shuffle_rand);

    //set the filter for kdtData
    hkmData.setFilter(&pl);
    
    //set filter for sdata
    sfilter[0] = i;
    sdata.setFilter(&sfilter);
    
    //get Knn
    knn(dists+(i*k), ids+(i*k), k, sdata, hkmData, this->opt.dist);
        
    //update ids to get the real ones
    for (uint f=0, fs=pl.size(), j=i*k; f<fs && f<k; ++f, ++j)
      ids[j] = pl[ids[j]];
    
    //reset filter
    hkmData.clearFilter();
    sdata.clearFilter();
    pl.clear();
  } //for i    
  
  // fnish for descend
  this->finishDescend();
}

//-----------------------------------------------------------------------
template <class T>
void Hkm<T>::initDescend(HkmOptions& opt)
{
  //if we are using kd-trees, then init all of them
  if (opt.usekdt)
    for (uint i=this->getRootId(); i<=this->getLeafId(); ++i)
      this->at(i).buildKdf(opt.kdtopt);
  
  //allocate buffers for descend
  cids = new uint[opt.nbranches];
  dists = new float[opt.nbranches];
}

//-----------------------------------------------------------------------
template <class T>
void Hkms<T>::initDescend()
{
  //loop on trees
  for (uint i=0; i<this->opt.ntrees; ++i)
    this->hkms[i].initDescend(this->opt);
}

//-----------------------------------------------------------------------
/// clear kdfs 
template <class T>
void Hkm<T>::finishDescend()
{
  //if we are using kd-trees, then init all of them
  if (this->opt.usekdt)
    for (uint i=this->getRootId(); i<=this->getLeafId(); ++i)
      this->at(i).clearKdf();
  
  //deallocate cids & dists
  if (cids != 0)  delete [] cids;
  if (dists != 0) delete [] dists;
}

//-----------------------------------------------------------------------
template <class T>
void Hkms<T>::finishDescend()
{
  //loop on trees
  for (uint i=0; i<this->opt.ntrees; ++i)
    this->hkms[i].finishDescend();
}

// //-----------------------------------------------------------------------
// #define TEMPLATE(F)         \
//   F(int)                    \
//   F(double)                 \
//   F(float)                  \
//   F(char)                   \
//   F(unsigned char)          \
//   F(unsigned int)           

#define GETKNN_F(T)     \
  template void Hkms<T>::hkmKnn(Data<T>&, Data<T>&, int, uint*, float*);
#define GETKNN_D(T)     \
  template void Hkms<T>::hkmKnn(Data<T>&, Data<T>&, int, uint*, double*);

#define CREATE(T)       \
  template void Hkms<T>::create(Data<T> &);
#define LEAFIDS(T)       \
  template void Hkms<T>::getLeafIds(Data<T> &, HkmClassId*);
  
  
TEMPLATE(GETKNN_F)
TEMPLATE(GETKNN_D)
TEMPLATE(CREATE)
TEMPLATE(LEAFIDS)


