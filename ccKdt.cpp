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
#include "ccKdt.hpp"

#define SWAP(a,b,t) (t)=(a); (a)=(b); (b)=(t);


//random number function to use with random_shuffle
ptrdiff_t random_shuffle_rand(ptrdiff_t s) { return rand() % s; }


// //-----------------------------------------------------------------------
// #define TEMPLATE(F)         \
//   F(int)                    \
//   F(double)                 \
//   F(float)                  \
//   F(char)                   \
//   F(unsigned char)          \
//   F(unsigned int)           

#define CREATE(T) \
  template void create(Kdf&, Data<T>&);

#define GETKNN_F(T) \
  template void getKnn(Kdf&, Data<T>&, Data<T>&, int, uint*, float*, Data<T>*);
#define GETKNN_D(T) \
  template void getKnn(Kdf&, Data<T>&, Data<T>&, int, uint*, double*, Data<T>*);

  
TEMPLATE(CREATE)
TEMPLATE(GETKNN_F)
TEMPLATE(GETKNN_D)

//random number [0,1)
#define URAND ((double)rand() / ((double)RAND_MAX + 1.))
//get a random number between [M1, M2)
#define RAND(M1, M2) (URAND * ((M2)-(M1)) + (M1))
//random number between [M1, M2-1]
#define IRAND(M1, M2) floor(RAND(M1,M2))

//use static arrays or dynamic ones
//uncomment this to use static arrays instead
//#define STATIC_TEMP_ARR
  
//-----------------------------------------------------------------------
//function to get the next split
// returns true if found a split, false otherwise
template <class T>
bool getSplit(Kdt& kdt, uint nid, Data<T>& data, KdtOptions& opt, KdtSplit& split)
{
  char found = 0;  
  
  //number of points in node
  KdtNode& node = kdt[nid];
  uint nodepoints = node.size(); 
  
  //check if empty node
  if (nodepoints <= 1) return false;

  //dims and npoints
  uint ndims = data.ndims;
  if (opt.bitsperdim) ndims *= opt.bitsperdim;
  uint npoints = data.size();
  
#ifdef STATIC_TEMP_ARR  
  bool dimavail[MAX_DIM];
#else
  bool* dimavail = new bool[ndims];
#endif
  //mark with zeros
  fill(dimavail, dimavail+ndims, false);
  ///cycle dimensions?
  if (opt.cycle)
  {    
    //disable parent dims
    for (uint i=kdt.parentId(nid); i>0; i=kdt.parentId(i))
      dimavail[kdt[i].split.dim] = true;
    
    //check if empty
    bool empty = true;
    for (uint i=0; i<ndims; ++i)
      if (!dimavail[i])
      {
        empty = false;
        break;
      }
    if (empty) fill(dimavail, dimavail+ndims, false);
  }
  
  //sample?
  char sample = 0;
  uint nsamples = nodepoints;
  if (opt.sample > 0 && opt.sample < nsamples)
  {
    sample = 1;
    nsamples = opt.sample;
  }

  //compute mean and variance
#ifdef STATIC_TEMP_ARR    
  float sum[MAX_DIM], sum2[MAX_DIM];
#else
  float *sum, *sum2;
  sum = new float[ndims];
  sum2 = new float[ndims];
#endif
  //init with zero
  fill(sum, sum+ndims, 0.);
  fill(sum2, sum2+ndims, 0.);
  
  uint ind;
  for (uint i=0; i < nsamples; ++i)
  {
    //get the id of the point
    if (sample)   ind = (uint) IRAND(0, nodepoints); //rand() % nodepoints;
    else          ind = i;
    //get sum and sum of squares
    for (uint j=0; j < ndims; ++j)
      //check if available
      if (!dimavail[j])
      {
        float v;
        //get the value
        if (opt.bitsperdim)
        {
          int m = j % opt.bitsperdim;
          int d = (int) j / opt.bitsperdim; 
#if USE_KDT_PLIST_IN_NODE==1
          v = (float)( static_cast<uint64_t>(data.getPointVal(node[ind],d)) >> m 
                  & (static_cast<uint64_t>(1)) );
#else
          v = (float)( static_cast<uint64_t>(data.getPointVal(kdt.points[node.start+ind],d)) >> m 
                  & (static_cast<uint64_t>(1)) );
#endif
//           cout << "point:" << node[ind] << " dim=" << j << "m=" << m << 
//                   " d=" << d << " v=" << v << endl;
        }
        else
        {
#if USE_KDT_PLIST_IN_NODE==1
          v = (float) data.getPointVal(node[ind], j);
#else
          v = (float) data.getPointVal(kdt.points[node.start+ind], j);
#endif
        }
        sum[j] += v;
        sum2[j] += v*v;
      } 
  }
  
  //loop and divide and compute max var
  float maxvar = -1, maxmean;
  uint maxvarind = -1;
  for (uint j=0; j<ndims; ++j)
  {
    sum[j] /= nsamples;
    sum2[j] = (sum2[j] / nsamples) - sum[j]*sum[j];
    if (sum2[j] > maxvar)
    {
      maxvar = sum2[j];
      maxvarind = j;
    }
  }
  
  bool ret = 0;
  //check if less than min var required
  if (maxvar >= opt.minvar)
  {

    //randomize dimension?
    if (opt.varrange)
    {
      //get ids of variances within range of maxvar
#ifdef STATIC_TEMP_ARR  
      uint ids[MAX_DIM];
#else
      uint* ids = new uint[ndims];
#endif
      uint i, j;
      for (i=0, j=0; i<ndims; ++i)
        if (!dimavail[i] && sum2[i] >= maxvar*opt.varrange)
          //store index and increment
          ids[j++] = i;
      //found something?
      if (j>0)
      {
        //select a random one of these
        maxvarind = ids[(uint32_t) IRAND(0,j)]; //[rand() % j]; //%ids.size()];
        maxvar = sum2[maxvarind];
      }
#ifndef STATIC_TEMP_ARR 
      //clear 
      delete [] ids;
#endif
    }

    //randomize mean?
    maxmean = sum[maxvarind];
    if (opt.meanrange)
    {
      //select a random one of these
      float r = (float) URAND; // ((float)rand()) / RAND_MAX;
      maxmean += (r - 0.5) * 2 * opt.meanrange * sqrt(maxvar);
    }

    //return
    split.dim = maxvarind;
    split.val = maxmean;
    //return true
    ret = true;
  }
  
#ifndef STATIC_TEMP_ARR   
  //clear  memory
  delete [] sum;
  delete [] sum2;
  delete [] dimavail;
#endif
  
  //return  
  return ret;
}


//---------------------------------------------------------------
template <class T>
void create(Kdt& kdt, Data<T>& data, KdtOptions& opt)
{
 
//   cout << "here" << endl;
//   NodeId d = kdt.depth(1);
  
  //allocate node list
  uint npoints = data.size();
  uint nnodes = npoints * 2 -1;
  //check max depth
  if (opt.maxdepth>0)
    nnodes = min(nnodes, (uint)pow(2., opt.maxdepth)-1);
  kdt.resize(nnodes);
  
// cout << "define=" << USE_KDT_PLIST_IN_NODE << endl;
#if USE_KDT_PLIST_IN_NODE == 1
//   cout << "using node plist" << endl;
//initialize root with full list
  KdtNode& root = kdt.at(1);
  root.resize(npoints);
  for (uint i=0; i<npoints; ++i) root[i] = i;
#else
//   cout << "using kdtree list" << endl;
  //init list of ids in tree
  kdt.points.resize(npoints);
  for (uint i=0; i<npoints; ++i) kdt.points[i] = i; 
  //init root with full list
  KdtNode& root = kdt.at(1);
  root.start = 0; root.end = npoints;
#endif
  
  //loop on the tree starting at the root
  for (uint n=1; n<=nnodes; ++n)
  {
//     cout << "node:" << n;
    
    //get the node
    KdtNode& node = kdt.at(n);
    
    //get first child and check if within limits
    if (!kdt.validId(kdt.leftId(n)))
    	break;
        
    //check max depth
    if (opt.maxdepth>0 &&  kdt.depth(n)>opt.maxdepth)
      break;
    
    //get the split for this node
    KdtSplit split;
    bool found = getSplit(kdt, n, data, opt, split);
    
//     cout << " split found: " << found << endl;
    
    //found a split?
    if (found)
    {
//       cout << "split: " << split.dim << " & " << split.val << endl;
      
      //put the split
      node.split = split;
      node.visited = true;
      
#if USE_KDT_PLIST_IN_NODE==1      
      //get ids of left and right points
      KdtPointList& lids = kdt.left(n).plist;
      KdtPointList& rids = kdt.right(n).plist;
      for (uint i=0, s=node.size(); i<s; ++i)
      { 
        //get distance from threshold
        float dist;
        if (!opt.bitsperdim)
          dist = (float)data.getPointVal(node[i], split.dim) - split.val;
        else
          dist = (float)( (static_cast<uint64_t>(data.getPointVal(node[i],
                  (int)split.dim/opt.bitsperdim)) >> split.dim%opt.bitsperdim)
                  & static_cast<uint64_t>(1) ) - split.val;
//           dist = (float)((uint64_t)data.getPointVal(node[i], (int)split.dim/opt.bitsperdim) 
//             & (((uint64_t) 1) << (split.dim%opt.bitsperdim))) - split.val;
        //if less than threshold, put in left
        if (dist < 0.)
          lids.push_back(node[i]);
        //otherwise put in right
        else
          rids.push_back(node[i]);
      }
      
//       //show points
//       cout << "Node: " << n << endl;
//       cout << "left ids: ";
//       for (uint k=0; k<lids.size(); ++k) cout << lids[k] << " "; cout << endl;
//       cout << "right ids: ";
//       for (uint k=0; k<rids.size(); ++k) cout << rids[k] << " "; cout << endl;
      
#else
      //init left and right nodes
      KdtNode& left = kdt.left(n);
      KdtNode& right = kdt.right(n);
      //left pointing at start of list
      left.start = left.end = node.start;
      //right pointing at end of list
      right.start = right.end = node.end;
      //loop on node points
      uint i = node.start;
      //loop such that left node gets start of list, right node gets end of list
      //and end when they overlap
      while(left.end < right.start && i<node.end)
      { 
        //get distance from threshold
        float dist;
        if (!opt.bitsperdim)
          dist = (float)data.getPointVal(kdt.points[i], split.dim) - split.val;
        else
          dist = (float)( (static_cast<uint64_t>(data.getPointVal(kdt.points[i],
                  (int)split.dim/opt.bitsperdim)) >> split.dim%opt.bitsperdim)
                  & static_cast<uint64_t>(1) ) - split.val;
        //put left?
        if (dist<0.)
        {
          //advance left.end and i
          left.end++;
          i = left.end;          
        }
        //right
        else
        {
          //update start to point to new location of start
          right.start--;
          //exchange this point this point with what's in right.start
          swap(kdt.points[i], kdt.points[right.start]);
          //don't update i, as we just got a new unprocessed point
        }        
      }        
      
//       //show
//       cout << "Node: " << n << endl;
//       cout << "left ids: ";
//       for (uint k=left.start; k<left.end; k++) cout << kdt.points[k] << " "; cout << endl;
//       cout << "right ids: ";
//       for (uint k=right.start; k<right.end; k++) cout << kdt.points[k] << " "; cout << endl;
      
#endif
        
//        //show now
//        if (node[i]==0)
//        {
//          cout << "create:point 0 val: " << data.getPointVal(node[i], split.dim) 
//            << " dist: " << dist << " going to node " <<
//                  (dist<0 ? kdt.leftId(n) : kdt.rightId(n)) << endl;
//          cout << " split: " << split.dim << " & " << split.val << 
//                  " node.size " << node.size() << " node ids: " << endl;
//          for (uint ii=0; ii<kdt[n].size(); ++ii) cout << " " << kdt[n][ii];
//          cout << endl;
//        }
//       }
      
      //clear this node's list
      node.clear();
    }    
  }  
  
//  for (uint n=1; n<=nnodes; ++n)
//  {
//    cout << "node:" << n << " has " << kdt[n].size() << endl;
//    for (uint ii=0; ii<kdt[n].size(); ++ii) cout << " " << kdt[n][ii];
//    cout << endl;
//    cout << " split: " << kdt[n].split.dim << " & " << kdt[n].split.val << endl;
//  }

}


//---------------------------------------------------------------
template <class T>
void create(Kdf& kdf, Data<T>& data)
{
  //set the random seed
  srand(0xFFFF);

  //allocate the trees
  kdf.resize(kdf.opt.ntrees);
  
  //loop on the trees and create
  for (uint t=0; t<kdf.opt.ntrees; ++t)
    create(kdf[t], data, kdf.opt);  
}

//-----------------------------------------------------------------------
template <class T>
NodeId descend(Kdt& kdt, NodeId start, Data<T>& data, uint pid, uint tid, 
        KdtBranchQ& pq, KdtOptions& opt)
{
  NodeId end = start;

  //skip if not valid
  if (!kdt.validId(end)) return end;
  
  //loop through starting at this node
  while (true)
  {
//    cout << "node: " << end  << " of " << kdt.size() << " tree " << tid << endl;
    //check if to stop: when left and right children are empty or outside scope
    KdtNode& node = kdt[end];
    NodeId lid = kdt.leftId(end), rid = kdt.rightId(end);
    if ( node.size()>0 || !node.visited)
//      || ((!kdt.validId(lid) || kdt[lid].size()==0) && 
//          (!kdt.validId(rid) || kdt[rid].size()==0) && node.size()>0) )
      break;
    
//    //get the split direction
//    if (node.split.dim > 200)
//    {
//      cout << " node: " << end <<  " of " << kdt.size() << " has " << kdt[end].size() << endl;
//      cout << " lid: " << lid << " with " << kdt[lid].size() << " and rid:" << rid << " with " << kdt[rid].size() << endl;
//    }
    
    float dist;
    if (!opt.bitsperdim)
      dist = (float)data.getPointVal(pid, node.split.dim) - node.split.val;
    else
      dist = (float)( (static_cast<uint64_t>(data.getPointVal(pid,
              (int)node.split.dim/opt.bitsperdim)) >> node.split.dim%opt.bitsperdim)
              & static_cast<uint64_t>(1) ) - node.split.val;
//       dist = (float)((uint64_t)data.getPointVal(pid, (int)node.split.dim/opt.bitsperdim) 
//         & (((uint64_t) 1) << (node.split.dim%opt.bitsperdim))) - node.split.val;
//     float dist = (float)data.getPointVal(pid, node.split.dim) - node.split.val;
  
    //next step
    NodeId unseen, next;      
    //go left
    if (dist<0.)
    {
      unseen = rid;
      next = lid;
    }
    //go right
    else
    {
      unseen = lid;
      next = rid;
    }
    
//    //show now
//    if (pid==0)
//    {
//      cout << "descend:point 0: val: " << data.getPointVal(pid, node.split.dim)
//        << " dist: " << dist << " going to node " << next << endl;
//    }
//    
//    cout << "descend: node: " << end << " split: " << kdt[end].split.dim << " & " << kdt[end].split.val << endl;
//    cout << " val: " << data.getPointVal(pid, node.split.dim) << endl;
//    cout << " dist: " << dist << endl;
//    cout << "going: " << (next==lid ? "left" : "right") << endl;
//    cout << " lid: " << lid << " rid: " << rid << endl;
//    cout << " end " << end << endl;
    //put the branch if valid
    if (kdt.validId(unseen))
    {
      KdtBranch b(unseen, tid, fabs(dist));
      pq.push_back(b);
    }
    
    //next?
    if (kdt.validId(next))
      end = next;
    else
      break;
  }
  
//  cout << " node " << end << " has " ;
//  for (uint ii=0; ii<kdt[end].size(); ++ii) cout << " " << kdt[end][ii];
//  cout << endl;
  
  return end;
}

//-----------------------------------------------------------------------
/// get Ids of a point
template <class T>
void getIds(Kdf& kdf, Data<T>& data, uint pid, KdtPointList& plist)
{
  //create the priority queue
  KdtBranchQ pq;
  
  //initialize it with the root nodes in all the trees
  for (uint t=0; t<kdf.opt.ntrees; ++t)
  {
    KdtBranch b(1, t, 0.);
    pq.push_back(b);
  }
  
  //now loop on pq and process
  uint bins = kdf.opt.maxbins;
  while (bins && pq.size()>0)
  {
    //sort the queue
//    sort(pq.begin(), pq.end());
    pq.sort();
    //truncate if too large
    if (pq.size() > kdf.opt.maxbins)
      pq.resize(kdf.opt.maxbins);
    
    //get the top branch
    KdtBranch branch = pq.front();  pq.pop_front();
    
//    cout << "branch -> tid: " << branch.treeId << " nodeid: " << branch.nodeId <<
//            " dist: " << branch.dist << endl;
    
    //go down the respective tree
    Kdt& kdt = kdf[branch.treeId];
    NodeId nid = descend(kdt, branch.nodeId, data, pid, branch.treeId, pq, kdf.opt);
    
    
    //check if valid
    if (kdt.validId(nid))
    {
              
      //add the list of ids
      KdtNode& node = kdt[nid];
#if USE_KDT_PLIST_IN_NODE==1      
      plist.insert(plist.end(), node.plist.begin(), node.plist.end()); 
#else
      for (uint i=0; i<node.size(); ++i)
        plist.push_back(kdt.points[node.start + i]);
#endif

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

//-----------------------------------------------------------------------
/// lookup the trees and return nearest neighbors
template <class Tret, class T>
void getKnn(Kdf& kdf, Data<T>& kdtData, Data<T>& sdata, 
        int k, uint* ids, Tret* dists, Data<T>* tData)
{
  //loop on the points
  DataFilter sfilter(1);
  for (uint i=0; i<sdata.size(); ++i)
  {
    //get list of points close to that point
    KdtPointList pl;
    // use tData if passed in to traverse the kd-tree
    if (tData)
      getIds(kdf, *tData, i, pl);
    else
      getIds(kdf, sdata, i, pl);
//    cout << "point " << i << " has " << pl.size() << " ids" << endl;
//    for (uint ii=0; ii<pl.size(); ++ii) cout << " " << pl[ii];
//    cout << endl;
    
    //get rid of duplicates
    sort(pl.begin(), pl.end());
    KdtPointIt it = unique(pl.begin(), pl.end());
    pl.resize(it - pl.begin());
    //shuffle #malaa
    random_shuffle(pl.begin(), pl.end(), random_shuffle_rand);

    //set the filter for kdtData
    kdtData.setFilter(&pl);
    
    //set filter for sdata
    sfilter[0] = i;
    sdata.setFilter(&sfilter);
    
    //get Knn
    knn(dists+(i*k), ids+(i*k), k, sdata, kdtData, kdf.opt.dist);
        
    //update ids to get the real ones
    for (uint f=0, fs=pl.size(), j=i*k; f<fs && f<k; ++f, ++j)
      ids[j] = pl[ids[j]];
    
    //reset filter
    kdtData.clearFilter();
    sdata.clearFilter();
    pl.clear();
  } //for i  
}

