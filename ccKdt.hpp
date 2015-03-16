// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CCKDT_H
#define CCKDT_H


#include <vector>
#include <deque>
#include <queue>
#include <list>
#include <climits>

#include "ccData.hpp"
#include "ccDistance.hpp"

using namespace std;

//typedef unsigned int uint;

#define MAX_DIM 1000 //5000000

//use list of point ids in nodes or an array in the tree
// #ifndef USE_KDT_PLIST_IN_NODE
#define USE_KDT_PLIST_IN_NODE 0
// #endif

//----------------------------------------------------------------------------
//properties struct
class KdtOptions
{
public:
  /// number of trees
  int ntrees;
  /// range for variance
  float varrange;
  /// range for mean
  float meanrange;
  /// max depth
  int maxdepth;
  /// min variance
  float minvar;
  /// cycle dimensions or not
  bool cycle;
  /// distance to use
  DistanceType dist;
  /// max bins to search
  int maxbins;
  /// sample or not
  int sample;
  /// bits per dimension, if compact binary input
  int bitsperdim;

  //default constructor
  KdtOptions()
  {
    this->ntrees = 1;
    this->varrange = 0;
    this->meanrange = 0;
    this->maxdepth = 0;
    this->minvar = 0;
    this->cycle = 1;
    this->dist = DISTANCE_L2;
    this->maxbins = 50;
    this->sample = 0;
    this->bitsperdim = 0;
  }
};

//----------------------------------------------------------------------------
//dimension list
typedef vector<uint> KdtDimList;

//----------------------------------------------------------------------------
//Node split
struct KdtSplit
{
public:
  /// dimension to use for the split
  uint dim;
  /// val of the split
  float val;  
};

//----------------------------------------------------------------------------
//point list
typedef vector<uint> KdtPointList;
typedef KdtPointList::iterator KdtPointIt;

typedef vector<KdtPointList> KdtPointListList;
typedef KdtPointListList::iterator KdtPointListIt;

/// Kdt Node
class KdtNode
{
public:
  /// split for that node
  KdtSplit split; 
  /// visited?
  bool visited;


#if USE_KDT_PLIST_IN_NODE==1  
  /// the point list
  KdtPointList plist;

  /// constructor
  KdtNode() { this->visited = false; }
  
  /// desctructor
  ~KdtNode() { this->clear(); }

  /// clear memory
  void clear() { this->plist.clear(); }
  
  /// size
  uint size() { return this->plist.size(); }
  
  /// insert a new point
  void push_back(uint i) { this->plist.push_back(i); }
  
  /// get a point
  uint& operator[](uint i) { return this->plist[i]; }
  
  /// resize
  void resize(uint i) { this->plist.resize(i); }
  
  /// empty?
  bool empty() { return this->plist.empty(); }
#else
  ///start and end of ids: start points to starting position of point in kdt.points
  /// end points to last position + 1, so size = end-start
  uint start, end;

  /// constructor
  KdtNode() { this->visited = false; this->end = this->start = 0;}
  
  /// desctructor
  ~KdtNode() { }

  /// clear memory
  void clear() { this->start = this->end = 0; }
  
  /// size
  uint size() { return end - start; }
  
  /// empty?
  bool empty() { return this->start >= this->end; }
#endif
      
};


//----------------------------------------------------------------------------
//type for node id
typedef uint32_t NodeId;


//----------------------------------------------------------------------------
/// Kdt Branch in search process
class KdtBranch
{
public:
  /// index of the node
  NodeId nodeId;
  /// tree id
  uint treeId;
  /// distance to the split value
  float dist;
  
  ///constructor
  KdtBranch() {}
  KdtBranch(NodeId nid, uint tid, float d) : nodeId(nid), treeId(tid), dist(d) 
  {}
  
  /// comparison for branch
  bool operator<(KdtBranch& b) { return this->dist < b.dist; }
};

//priority queue definition
//typedef priority_queue<KdtBranch, deque<KdtBranch>, KdtBranchCmp>
//  KdtBranchQ;
typedef list<KdtBranch> KdtBranchQ;
typedef KdtBranchQ::iterator KdtBranchIt;

//----------------------------------------------------------------------------
//list of nodes
typedef vector<KdtNode> KdtNodeList;
typedef KdtNodeList::iterator KdtNodeIt;


/// Kdtree class
class Kdt
{
public:
  /// list of nodes
  KdtNodeList nodes;

  /// constructor
  Kdt() {}
  
  ///destructor
  ~Kdt() { this->clear(); }
  
//not using point list, then have a list of point ids
#if USE_KDT_PLIST_IN_NODE==1
  /// clear memory
  void clear() { this->nodes.clear(); }

#else
  ///list of point ids
  KdtPointList points;

  /// clear memory
  void clear() { this->nodes.clear(); this->points.clear(); }
#endif
  
  /// return a node: NodeId starts at 1, zero index is undefined and returned
  /// for out of range ids
  KdtNode& operator[](NodeId i) { return this->nodes[i-1]; }
     
  /// return a node
  KdtNode& at(NodeId i) { return this->nodes[i-1]; }

  /// resize
  void resize(NodeId i) { this->nodes.resize(i); }
  
  /// resize
  uint size() { return this->nodes.size(); }
  
  /// empty
  bool empty() { return this->nodes.empty(); }
  
  /// left child id 
  NodeId leftId(NodeId i) { return i<<1; }  //i<<1
  /// right child id
  NodeId rightId(NodeId i) { return this->leftId(i) + 1; }
  /// parent id
  NodeId parentId(NodeId i) { return i>>1; } //i>>1
  
  /// left child
  KdtNode& left(NodeId i) { return this->at(leftId(i)); }
  /// right child
  KdtNode& right(NodeId i) { return this->at(rightId(i)); }
  /// parent
  KdtNode& parent(NodeId i) { return this->at(parentId(i)); }
  
  /// valid node id
  bool validId(NodeId i) { return i>0 && i<=this->size(); };
  
//  /// leaf node?
//  bool leafId(NodeId i) 
//  { 
//    NodeId l
//    validId(leftId(i)) && left(i)
    
  /// depth of a node id: depth of root is 1
  NodeId depth(NodeId i) 
  {  
    NodeId d;
    for (d=0; i>0; i >>= 1, d++);
    return d;
  }
    
  /// get a split for the input data
  template <class T>
  friend bool getSplit(Kdt& kdt, uint nid, Data<T>& data, KdtOptions& opt, 
          KdtSplit& split);

  /// create a Kdtree with input data
  template <class T>
  friend void create(Kdt& kdt, Data<T>& data, KdtOptions& opt);
  
  /// descends the tree starting from a specific node while adding branches
  template <class T>
  friend NodeId descend(Kdt& kdt, NodeId start, Data<T>& data, uint pid, 
          KdtBranchQ& pq, KdtOptions& opt);
  
};


//----------------------------------------------------------------------------
//list of Kdtrees
typedef vector<Kdt> KdtList;

///Kdforest class
class Kdf
{
public:
  /// list of trees
  KdtList trees;
  /// options
  KdtOptions opt;
  
  /// constructor
  Kdf () {}
  Kdf (KdtOptions& o) : opt(o) {}
  
  /// desctructor
  ~Kdf() { this->clear(); }
  
  /// clear memory
  void clear() { this->trees.clear(); }
  
  /// resize
  void resize(uint i) { this->trees.resize(i); }
  
  /// access a specific tree
  Kdt& operator[](uint i) { return this->trees[i]; }
    
  /// create a Kdforest with input data
  template <class T>
  friend void create(Kdf& kdf, Data<T>& data);
  
  /// get Ids of points close to a given point
  template <class T>
  friend void getIds(Kdf& kdf, Data<T>& data, uint pid, KdtPointList& plist);
  
  /// lookup the trees and return nearest neighbors. If tData is given, it is 
  /// used for traversing the tree while sData is used for distance calculations
  template <class Tret, class T>
  friend void getKnn(Kdf& kdf, Data<T>& kdtData, Data<T>& sdata, 
          int k, uint* ids, Tret* dists, Data<T>* tData = NULL);
  
};



#endif
