// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CCHKMEANS_HPP
#define CCHKMEANS_HPP


#include <vector>
#include <deque>
#include <queue>
#include <list>
#include <climits>
#include <cmath>


#include "ccData.hpp"
#include "ccDistance.hpp"
#include "ccKdt.hpp"

using namespace std;

//typedef unsigned int uint;

#define MAX_DIM 1000 //5000000

//----------------------------------------------------------------------------
// Options struct
class HkmOptions
{
public:
  /// number of iterations
  int niters;
  /// number of levels
  int nlevels;
  /// number of branches per level
  int nbranches;
  /// Kdtree options
  KdtOptions kdtopt;
  /// distance to use
  DistanceType dist;
  /// use nearest neighbor or kdtree 
  bool usekdt;
  /// number of hkm trees
  int ntrees;
  /// number of backtracking steps
  int nchecks;

  //default constructor
  HkmOptions()
  {
    this->niters = 100;
    this->nlevels = 6;
    this->nbranches = 10;
    this->usekdt = true;
    this->dist = DISTANCE_L2;
    this->ntrees = 1;
    this->nchecks = this->ntrees;
    
    //kdt options
    this->kdtopt.ntrees = 5;
    this->kdtopt.varrange = 0.8;
    this->kdtopt.meanrange = 0;
    this->kdtopt.cycle = 0;
    this->kdtopt.dist = this->dist;
    this->kdtopt.maxbins = 100;
    this->kdtopt.sample = 250;            
  }
};

//----------------------------------------------------------------------------
typedef uint32_t HkmClassId;
typedef vector<HkmClassId> HkmClassIds;
typedef HkmClassIds::iterator HkmClassIdIt;

typedef uint32_t HkmPoint;
typedef vector<HkmPoint> HkmPointList;
typedef HkmPointList::iterator HkmPointIt;

typedef float MeanStd;
typedef vector<MeanStd> MeanStds;
typedef MeanStds::iterator MeanStdIt;

/// A node in the HKM tree
template <class T>
class HkmNode
{
public:
  /// the means in this node
  Data<T> means;
  
  /// the class ids for the input data
  HkmClassIds classIds;
  
  ///std deviation of distances in every mean
  MeanStds meanStds;
  
  /// kdtree for this node
  Kdf *kdf;
  
  /// list of points in this node
  HkmPointList plist;
  
  /// valid node or not i.e. has means
  bool visited;
  
  /// constructor
  HkmNode() 
  { 
    this->kdf = NULL;
    this->means.ndims = 0;
    this->means.npoints = 0;
    this->visited = false;
  }
  
  /// descructor
  ~HkmNode() { this->clear(); }
  
  /// resize list of points
  void resize(uint i) 
  { 
    this->plist.resize(i); 
    this->classIds.resize(i); 
  }
  
  /// get size of list of points
  uint size() { return this->plist.size(); }
  
  /// clear memory
  void clear()
  {
    this->clearKdf();
    this->clearLists();
    this->means.clear();
    this->meanStds.clear();
  }
  
  /// clear the kdtree
  void clearKdf()
  {
    if (this->kdf) 
    {
      this->kdf->clear();
      delete this->kdf;
      this->kdf = NULL;
    }    
  }
  
  /// clear lists
  void clearLists()
  {
    this->plist.clear();
    this->classIds.clear();
  }
    
  /// build kdtree
  void buildKdf(KdtOptions &kdtopt)
  {
    //clear the kdf if has something, otherwise allocate
    this->clearKdf();
    //have something?
    if (this->means.size() > 0)
    {
      this->kdf = new Kdf(kdtopt);
      if (!this->kdf)  cout << "Kdf memory error!" << endl;

      //build the kdtree with the means
      create(*this->kdf, this->means);    
    }
  }
  
  /// intialize means
  void initMeans(Data<T> &data, HkmOptions &opt);
  
  /// performs kmeans on the data in this node
  void kmeans(Data<T> &data, Data<double>& tmeans, HkmOptions &opt);
  
  /// compute means from ids of input data
	void getMeans(Data<T> &data, Data<double>& tmeans, HkmOptions &opt, uint* meanpoints);
  
  /// compute nearest mean for input data
  bool getNearMean(Data<T> &data, HkmOptions &opt, uint* ids, float* dists);
  
  /// get the nearest mean for the input data point
  HkmClassId getNearMean1(Data<T> &point, HkmOptions &opt,
        uint* cids, float* dists);
  //   HkmClassId getNearMean1(Data<T> &data, HkmOptions &opt);
      
};

// node id
typedef uint32_t HkmNodeId;


//----------------------------------------------------------------------------
/// Hkm Branch in search process
class HkmBranch
{
public:
  /// index of the node
  HkmNodeId nodeId;
  /// tree id
  uint treeId;
  /// distance to the nearest mean
  float dist;
  /// meanid, used to get branches within the last level nodes that have
  /// no branches to children
  HkmClassId meanId;
  
  ///constructor
  HkmBranch()  {} 
  HkmBranch(HkmNodeId nid, uint tid, float d) : nodeId(nid), treeId(tid), dist(d), meanId(0) 
  {}
  HkmBranch(HkmNodeId nid, uint tid, float d, HkmClassId cid) : nodeId(nid), treeId(tid), dist(d), meanId(cid) 
  {}
  
  /// comparison for branch
  bool operator<(HkmBranch& b) { return this->dist < b.dist; }
};

//priority queue definition
//typedef priority_queue<KdtBranch, deque<KdtBranch>, KdtBranchCmp>
//  KdtBranchQ;
typedef list<HkmBranch> HkmBranchQ;
typedef HkmBranchQ::iterator HkmBranchIt;

//----------------------------------------------------------------------------


/// HKM main class
template <class T>
class Hkm
{
public:
  // list of nodes
  typedef vector<HkmNode<T> > HkmNodes;
//  typedef HkmNodes::iterator HkmNodeIt;
  
  HkmOptions opt;
  
  /// list of nodes
  HkmNodes nodes;
 
  /// constructor
  Hkm() { cids = 0; dists = 0; }
  Hkm(HkmOptions &opt) { this->opt = opt; }
  
  /// destructor
  ~Hkm() { this->clear(); }
  
  /// clear memory
  void clear()
  {
    this->nodes.clear();
  }
  
  /// resize list of nodes
  void resize(uint i) { this->nodes.resize(i); }
  
  /// get size of nodes list
  uint size() { return this->nodes.size(); }
    
  /// valid id?: 1 -> nodes.size()
  bool validId(HkmNodeId id) { return id>0 && id<=this->nodes.size() ? true : false; }
  
  /// get the root id
  HkmNodeId getRootId() { return 1; }

  /// get the id of the last leaf
  HkmNodeId getLeafId() { return this->nodes.size(); }  
  
  /// get parent node id
  /// NodeIds are 1-based i.e. first id is no. 1
  HkmNodeId getParentId(HkmNodeId id)
  {
    //return 0 if id=1
    if (id==1) return 0;
    
    //get depth of id and start & end ids in its level
    double start, end;
    int depth = this->getDepth(id, start, end);
    //now we can get start and end ids for previous level
    HkmNodeId pstart = (HkmNodeId) (start  - (end-start+1) /  this->opt.nbranches);
    //now get the id of the parent
    return (HkmNodeId) (floor((id - start) / this->opt.nbranches) + pstart);
  }
  
  /// get node id of the first child
  HkmNodeId getChildId(HkmNodeId id)
  {
    //get depth of id and start & end ids in its level
    double start, end;
    int depth = this->getDepth(id, start, end);
    // get id of first child in next level
    return (HkmNodeId) ((id - start) * this->opt.nbranches + end + 1);
  }
  
  /// get node id of the first child (leftmost) on the last level
  HkmNodeId getDeepChildId(HkmNodeId id)
  {
    //get depth of id
    int depth = this->getDepth(id);
    HkmNodeId nid = id;
    //loop until the last level
    while(depth+1 < this->opt.nlevels)
      depth = this->getDepth(nid=this->getChildId(nid));
    // get id of first child in next level
    return nid;
  }
  
  /// return a reference to a node
  inline HkmNode<T>& at(HkmNodeId i) { return this->nodes.at(i-1); } //this->nodes[i-1]; }
          
  /// returns the depth 0-based of the id, 0 is the root depth
  int getDepth(HkmNodeId id, double &start, double &end)
  {
    //initial depth
    int depth = 0;
    //initial start and end ids
    end = 1, start = 1;
    //get the right depth
    for(; end < id; start=end+1, depth++, end+=pow((double)this->opt.nbranches, depth));
    //return the depth
    return depth;
  }
  
  /// returns the depth 0-based of the id, 0 is the root depth
  int getDepth(HkmNodeId id)
  {
    double s, e;
    return this->getDepth(id, s, e);
  }
  
  /// create the HKM tree with the input data
  void create(Data<T> &data, HkmOptions& opt);
  
  /// descend down the tree from a specific node
  //
  // start  - the starting node
  // data   - the input data
  // pid    - the data point
  // meanid - the output nearest mean, which is an output. 0-based
  //          [deprecated] This is 1 based, with 0 value if start is not a valid node
  // pq     - priority queue to add unseen branches
  // opt    - the options of the HKMs
  // tid    - the tree id of this tree (used for pq)
  //
  // output - the output leaf node id, where meanid is the nearest mean in 
  //          that node.
  HkmNodeId descend(HkmNodeId start, Data<T>& data, uint pid, 
        HkmClassId &meanid, HkmBranchQ* pq, HkmOptions& opt, uint tid);
  
//   /// get Ids of points close to a given point
//   void getIds(Data<T>& data, uint pid, KdtPointList& plist);
  
  /// get the leaf id of the input points
  /// data  - the input data
  /// cids  - a list of class ids, one for every point. Class Ids are
  ///         1-based, and count the leaves from the leftmost to rightmost
  ///         the max is nbranches^nlevels
  ///         its size is ntrees X npoints
  HkmClassId getLeafId(Data<T> &data, uint pid, HkmNodeId sleaf, 
          HkmNodeId pleaf, HkmOptions& opt);
  
//   /// lookup the trees and return nearest neighbors
//   template <class Tret>
//   void hkmKnn(Data<T>& hkmData, Data<T>& sdata, int k, uint* ids, Tret* dists);
  
  /// prepare for descend i.e. create kd-trees for the means
  /// this is called inside hkmKnn and getLeafIds before calling descend
  void initDescend(HkmOptions& opt);
  
  /// clear data after descend is not required anymore
  void finishDescend();
  
private:
  uint* cids;
  float* dists;
  
};

//----------------------------------------------------------------
/// HKMs main class: contains a list of HKM trees
template <class T>
class Hkms
{
public:
  // list of Hkm
  typedef vector<Hkm<T> > HkmList;
  HkmList hkms;
  
  /// options
  HkmOptions opt;
  
  /// constructor
  Hkms() {}
  Hkms(HkmOptions &opt) { this->opt = opt; }
  
  /// destructor
  ~Hkms() { this->clear(); }
  
  /// clear memory
  void clear()
  {
    this->hkms.clear();
  }
  
  /// resize list of hkms
  void resize(uint i) { this->hkms.resize(i); }
  
  /// get size of hkms
  uint size() { return this->hkms.size(); }
    
  /// create the HKM forest with the input data
  void create(Data<T> &data);
  
  /// get Ids of points close to a given point
  void getIds(Data<T>& data, uint pid, KdtPointList& plist);
  
  /// get the leaf id of the input points
  /// data  - the input data
  /// cids  - a list of class ids, one for every point. Class Ids are
  ///         1-based, and count the leaves from the leftmost to rightmost
  ///         the max is nbranches^nlevels
  ///         it should be preallocated, with dimes ntrees X npoints (columnwise)
  void getLeafIds(Data<T> &data, HkmClassId *cids);
  
  /// lookup the trees and return nearest neighbors
  template <class Tret>
  void hkmKnn(Data<T>& hkmData, Data<T>& sdata, int k, uint* ids, Tret* dists);
    
  /// prepare for descend i.e. create kd-trees for the means
  /// this is called inside hkmKnn and getLeafIds before calling descend
  void initDescend();
  
  /// clear data after descend is not required anymore
  void finishDescend();
};


//-------------------------------------------------------------------


#endif
