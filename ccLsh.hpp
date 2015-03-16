// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CCLSH_HPP_
#define CCLSH_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include "ccData.hpp"
#include "ccDistance.hpp"

using namespace std;

//using namespace data;

///Maximum number of dimensions allowed
#define MAX_DIM 1024 

//typedef unsigned int uint

//Access an element in a matrix
#define _MAT_ELEM_P(mat,row,col,nrows) (mat + (nrows)*(col) + (row))
#define _MAT_ELEM(mat,row,col,nrows) (*_MAT_ELEM_P(mat,row,col,nrows))

// the polynomial used for generating the random permutation for min-hash
#define MIN_HASH_P 0x989677   
//0xFFFFFFFFFFFFFFFBL 

// the polynomial used for hashing the bucket ids
//#define HASH_POLY       0xFFFFFFFFFFFFFFFBL 
//#define HASH_POLY_REM   5
#define HASH_POLY       1368547 
#define HASH_POLY_REM   573440

#define HASH_POLY_A_NUM 15
const uint32_t  HASH_POLY_A[] = {1342, 876454, 656565, 223, 337, 9847, 87676, 
  34234, 23445, 76543, 8676234, 3497, 9876, 87856, 2342858};

///-----------------------------------------------------------------------
///Types of hashing functions
enum LshHashType
{
  LSH_HASH_TYPE_HAM = 0,
  LSH_HASH_TYPE_L1,
  LSH_HASH_TYPE_L2,
  LSH_HASH_TYPE_COS,
  LSH_HASH_TYPE_SPH_SIMPLEX,
  LSH_HASH_TYPE_SPH_ORTHOPLEX,
  LSH_HASH_TYPE_SPH_HYPERCUBE,
  LSH_HASH_TYPE_MHASH,
  //binary hashing of Raginsky & Lazebnik 2009 with Gaussian Kernel
  LSH_HASH_TYPE_BIN_GAUSS
};

#define USE_LSH_PLIST_IN_BUCKET 1

///-----------------------------------------------------------------------
// forward declarations
class LshOptions;
class LshBucketId;
class LshBucket;
class LshBuckets;
class LshFunc;
class LshFuncs;
class LshTable;
class LshTables;
class Lsh;

///-----------------------------------------------------------------------
///Options for the LSH
class LshOptions
{
public:
  ///number of tables
  int ntables;
  ///number of hash functions
  int nfuncs;
  ///Hashing function type
  LshHashType htype;
  ///distance function to use
//  LshDist dist;
  DistanceType dist;
  ///normalize or not
  bool norm;
  ///number of dimensions for the data
  uint ndims;
  /// width of interval (for L1 and L2)
  float w;
  /// number of bits per individual hash function or width of string
  /// of every hash func
  int nbits;
  /// max of each indiv. hash function 
  int hwidth;
  ///option 1 & 2
  double opt1;
  double opt2;
  /// set fixed size for tables so that we don't search for buckets
  /// zero if size not fixed
  uint tablesize;
  ///bucket id type
  bool str;
  ///forest or not
  bool forest;
  //verbose
  bool verbose;  
  /// max number for min-hash values
  uint mhmax;
  /// seed for random number generator
  uint seed;
  /// number of bits per dimension for compact binary data
  int bitsperdim;
  
  ///constructor
  LshOptions()
  {
    ntables = 5;
    nfuncs = 10;
    htype = LSH_HASH_TYPE_COS;
    dist = DISTANCE_COS;
    norm = true;
    ndims = 128;
    w = 1;
    nbits = 8;
    hwidth = 0;
    opt1 = 8;
    opt2 = 5;
    tablesize = 0;
    str = false;
    forest = false;
    verbose = false;
    mhmax = 100000;
    seed = 0xffff00;
    bitsperdim = 0;
  }
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, LshOptions& o);
  friend istream& operator>>(istream& s, LshOptions& o);  
};


///-----------------------------------------------------------------------
//Lsh Hash function return value
typedef uint32_t LshFuncVal_t;
typedef vector<LshFuncVal_t> LshFuncValList;
typedef vector<LshFuncValList> LshFuncValListList;

///-----------------------------------------------------------------------

class LshFunc
{
public:
  /// type of hasing function
  LshHashType type;
  
  //number of dimensions
  int ndims;
  
  /// the function
  union
  {
    /// functions for hamming distance
    uint ham;
    /// functions for L1
    struct _l1
    {
      /// dimension
      uint dim;
      /// shift
      float s;
    } l1;
    /// functios for L2
    struct _l2
    {
      // random directions
      float* r;
      // random shifts
      float b;
    } l2;
    /// functions for cos
    float* cos;
    /// function for min-hash: a random permutation
    struct _minhash{
      // random numbers a & b to compute a random permutation
      // the permutation of a value x is: (a*x + b) mod p
      // where p is a prime number
      uint32_t a;
      uint32_t b;
    } minhash;
    /// function for spherical LSH
    //random rotation matrix or rotated vertices
    float* sphere;
    /// functions for binary-gaussian kernel hash functions
    struct _bingauss{
      //w: normal distribution
      float* w;
      //b: uniform [0 2*pi]
      float b;
      //t: uniform [-1 1]
      float t;
    } bingauss;
    
  } func;
  
  /// constructor
  LshFunc() { this->type = LSH_HASH_TYPE_L1; }
  
  /// destructor
  ~LshFunc()
  {
    switch(this->type)
    {
      case LSH_HASH_TYPE_L2:            delete [] this->func.l2.r;      break;
      case LSH_HASH_TYPE_COS:           delete [] this->func.cos;       break;
      case LSH_HASH_TYPE_SPH_SIMPLEX:
      case LSH_HASH_TYPE_SPH_ORTHOPLEX:
      case LSH_HASH_TYPE_SPH_HYPERCUBE:	delete [] this->func.sphere;    break;
      case LSH_HASH_TYPE_BIN_GAUSS:     delete [] this->func.bingauss.w; break;
    }
  }
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, LshFunc& f);
  friend istream& operator>>(istream& s, LshFunc& f);
  
  /// make a random function with the options passed in
  void create(const LshOptions& opt);
  
  /// computes the bucket id for this data point
  template <class T>
  friend uint64_t getId(LshFunc& func, Data<T>& data, uint pid,
        const LshOptions& opt, float norm1, float norm2);

};

///-----------------------------------------------------------------------
/// List of Lsh Functions
class LshFuncs
{
public:
  vector<LshFunc> funcs;
  
  /// constructor
  LshFuncs()
  {
  }
  
  /// create the list of functions
  void create(LshOptions& opt);
  
  /// number of functions
  uint size()
  {
    return this->funcs.size();
  }
  
  /// operator [] to access the member functions
  LshFunc& operator[](uint i)
  {
    return this->funcs[i];
  }
  
  /// clear memory
  void clear()
  {
    funcs.clear();
  }
  
  ///desctructor
  ~LshFuncs()
  {
    this->clear();
  }
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, LshFuncs& f);
  friend istream& operator>>(istream& s, LshFuncs& f);
  
  /// compute the bucket Id for a data point using this list of functions
  /// funcs     - the input functions
  /// data      - the data point
  /// ndims     - no of dimensions
  /// opt       - the LSH options
  template <class T>  
  friend LshBucketId getId(LshFuncs& funcs, Data<T>& data, uint pid, 
          const LshOptions& opt, float norm1, float norm2);
};

///-----------------------------------------------------------------------
typedef uint64_t BucketId_t;
typedef vector<BucketId_t> BucketIdList;

///id for the bucket
class LshBucketId
{
public:
  /// string?
  bool str;
  
  /// union for the id
  struct _id
  {
    string s;
    BucketId_t d; 
  } id;
  
  /// constructor
  LshBucketId(bool s = false)
  {
    this->str = s;
    id.d = 0;
    id.s = "";
  }
  
  /// destructor
  ~LshBucketId() {}
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, LshBucketId& b);
  friend istream& operator>>(istream& s, LshBucketId& b);
  
  /// comparison of ids
  bool operator==(const LshBucketId& bid)
  {
    return this->str ? (this->id.s == bid.id.s) : 
      (this->id.d == bid.id.d);
  }
  
  /// less than for ids
  bool operator<(const LshBucketId& bid)
  {
    return this->str ? (this->id.s < bid.id.s) : 
      (this->id.d < bid.id.d);
  }
  
  /// add another hash function value to the id
  //
  // val    - the value to add
  // pos    - the position of this function, starting with 0
  void addId(uint64_t val, int pos, const LshOptions& opt);
};

///-----------------------------------------------------------------------
///point list and iterator
typedef vector<uint> LshPointList;
typedef LshPointList::iterator LshPointIt;

///Lsh bucket
class LshBucket
{
public:  
  /// Bucket id
  LshBucketId id;
  
  /// constructor
  LshBucket(LshBucketId id) {this->id = id;}

  /// stream overloads
  friend ostream& operator<<(ostream& s, LshBucket& b);
  friend istream& operator>>(istream& s, LshBucket& b);
  
  /// equality operator
  bool operator==(const LshBucket& b)
  {
    return (this->id == b.id);
  }
  
  /// less than operator
  bool operator<(const LshBucket& b)
  {
    return (this->id < b.id);
  }
  
#if USE_LSH_PLIST_IN_BUCKET==1
  /// list of points in the bucket
  LshPointList plist;  
  
  LshBucket() {  }

  /// destructor
  ~LshBucket()
  {
    //clear point list
    this->plist.clear();   
  }
    
  /// insert an id into the point list
  void insert(uint pid) {this->plist.push_back(pid);}
  
  /// number of points
  uint size() {return this->plist.size();}
  
  /// resize of points
  void resize(uint i) {this->plist.resize(i);}  
  
  /// operator [] to access points
  uint operator[](uint i) {return this->plist[i];}
  
#else
  /// start and end of point ids in the global list
  uint strt, end;
  
  LshBucket() 
  {  
    // set both equal to zero i.e. empty bucket
    this->start = this->end = 0; 
  }

  /// destructor
  ~LshBucket()
  {
  }
    
  /// insert an id into the global plist, which 
  void insert(uint pid, LshPointList& plist)
  {
    // get current size of global list
    uint plsize = plist.size();
    // current one empty?
    if (this->size() == 0) 
    {
      // push at the back
      plist.push_back(pid);
      // set start and end
      this->start = plsize;
      this->end = plsize + 1;
    }
    else
    {
      // not empty, so need to shift and insert
      plist.insert(plist.begin() + this->end, pid);
      // set end
      this->end += 1;
    }
  }
  
  /// number of points
  uint size() {return this->end - this->start;}
  
  /// resize of points: nothing here
  void resize(uint i) { }  
  
  /// operator [] to access points
  uint operator[](uint i) {return this->plist[i];}
  
#endif
  
};

/// list and iterator for LshBucket
typedef vector<LshBucket> LshBucketList;
typedef LshBucketList::iterator LshBucketIt;
typedef vector<LshBucketList> LshBucketListList;
typedef LshBucketListList::iterator LshBucketListIt;
        

///-----------------------------------------------------------------------
class LshBuckets
{
public:
  bool fixed;
  
  /// the list of buckets
  LshBucketList varBuckets;
  LshBucketListList fixedBuckets; 
  
  /// constructor
  LshBuckets() { this->fixed = false;}
  
  /// desctructor
  ~LshBuckets() {this->clear();}
  
  /// size of list
  uint size() 
  {
    return fixed ?  this->fixedBuckets.size() : this->varBuckets.size();
  }
  
  ///resize list
  void resize(uint s) 
  {
    if (fixed)
      this->fixedBuckets.resize(s);
    else
      this->varBuckets.resize(s);
  }
  
  /// begin of list
  LshBucketIt begin(uint i=0) 
  {
    return fixed ? this->fixedBuckets[i].begin() : this->varBuckets.begin();
  }
  
  /// end of list
  LshBucketIt end(uint i=0) 
  {
    return fixed ? this->fixedBuckets[i].end() : this->varBuckets.end();
  }  
  
//  /// operator [] to access elements
//  LshBucket& operator[](uint i, uint j=0)
//  {
//    return fixed ? this->fixedBuckets[i][j] :  this->buckets[i];
//  }
  
  /// clear memory
  void clear() 
  {
    this->fixedBuckets.clear();
    this->varBuckets.clear();
  };
  
  /// find a specific bucket in the list
  /// 
  /// b     - the input bucket
  /// bit   - return iterator to the found one, or position to insert in
  /// id    - the index of the bucket list (in case of fixed bucket lis)
  /// 
  /// output returns whether found or not
  ///
  bool find(const LshBucket& b, LshBucketIt& bit, uint& id);
//  LshBucketIt find(const LshBucket& b, bool& found, LshOptions& opt);
  
  /// insert a bucket into the list
  /// 
  /// b     - the bucket
  /// bit   - the bucket iterator to insert at
  /// id    - the id of the bucket list to insert in, in case of fixed size
  ///
  /// output  - an iterator to the inserted one
  LshBucketIt insert(LshBucket& b, LshBucketIt bit, uint id=0);
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, LshBuckets& b);
  friend istream& operator>>(istream& s, LshBuckets& b);
};

///-----------------------------------------------------------------------
/// Lsh Statistics
class LshStats
{
public:
  /// number of full buckets
  int nFullBuckets;
  /// average bucket size
  int meanBucketSize;
  /// median bucket size
  int medianBucketSize;
  /// standard dev
  int stdBucketSize;
  /// mean collision size
  int meanCollision;
  
  /// constructor and destructor
	LshStats() : nFullBuckets(0), meanBucketSize(0), medianBucketSize(0),
    stdBucketSize(0), meanCollision(0) {}
  ~LshStats() {}
};

///-----------------------------------------------------------------------
class LshTable
{
public:
  /// Hashing functions
  LshFuncs funcs;
  /// buckets
  LshBuckets buckets;
  /// stats
  LshStats stats;
  
  /// constructor
  LshTable() {}
  
  /// destructor
  ~LshTable() {this->buckets.clear();}
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, LshTable& t);
  friend istream& operator>>(istream& s, LshTable& t);

  /// create functions for this table
  void createFuncs(LshOptions& opt);
  
  /// compute stats
  void computeStats(LshOptions& opt);
  

  /// insert a point into the table
  ///
  /// lshtab    - the Lsh table
  /// point     - pointer to the point to insert
  /// ndims     - number of dimensions
  /// pid       - id of that point
  /// opt       - the Lsh options
  template <class T>
  friend void insertPoint(LshTable& lshtab, Data<T>& data, uint inpid, uint pid, 
          LshOptions& opt, float norm1, float norm2);
  
  /// insert a set of points into the table
  ///
  /// lshtab    - the Lsh table
  /// data      - the input data
  /// idshift   - a number to add to point indices when adding to table
  /// opt       - the Lsh options
  template <class T>
  friend void insertPoints(LshTable& lshtab, Data<T>& data,
          uint idshift, LshOptions& opt, float* norm1, float* norm2);
  
  /// look for a point in the table
  ///
  /// lshtab    - the Lsh table
  /// point     - pointer to the point to search for
  /// ndims     - the input dimensions
  /// pl        - a list of points to hold return ids
  /// opt       - the Lsh options
  template <class T>
  friend void searchPoint(LshTable& lshtab, Data<T>& data, uint pid,
          LshPointList& pl, LshOptions& opt, float norm1, float norm2);
  
  /// look for a point in the table
  ///
  /// lshtab    - the Lsh table
  /// data      - the input data
  /// pls       - an array of point lists to hold data
  /// opt       - the Lsh options
  template <class T>
  friend void searchPoints(LshTable& lshtab, Data<T>& data, 
          LshPointList* pls, LshOptions& opt, float* norm1, float* norm2);
 
};

///-----------------------------------------------------------------------
/// list and iterators
typedef vector<LshTable> LshTableList;
typedef LshTableList::iterator LshTableIt;

class LshTables
{
public:
  /// the list of tables
  LshTableList tables;
  
  /// constructor
  LshTables() {}
  
  /// destructor
  ~LshTables() { this->clear();}
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, LshTables& t);
  friend istream& operator>>(istream& s, LshTables& t);

  /// clear memory
  void clear() {this->tables.clear();}
  
  /// size
  uint size() {return this->tables.size();}
  
  /// resize
  void resize(uint i) {this->tables.resize(i);}
  
  /// operator []
  LshTable& operator[](uint i) {return this->tables[i];}
  
  /// insert points into the list of tables
  ///
  /// lshtab    - the Lsh table
  /// data      - the input data
  /// opt       - the Lsh options
  template <class T>
  friend void insertPoints(LshTables& lshtabs, Data<T>& data, uint idshift, 
          LshOptions& opt);  
  
  /// look for points in the tables
  ///
  /// lshtabs   - the Lsh tables
  /// data      - the input data
  /// pls       - an array of point lists to hold data
  /// opt       - the Lsh options
  template <class T>
  friend void searchPoints(LshTables& lshtabs, Data<T>& data, 
          LshPointList* pls, LshOptions& opt);
  
 
};


///-----------------------------------------------------------------------
///LSH structure
class Lsh
{
public:
  /// options
  LshOptions opt;
  
  /// array of tables
  LshTables tables;
  
  /// stats
  LshStats stats;
  
  /// constructor
  Lsh() { this->init();}
  Lsh(LshOptions opt) {this->opt = opt; this->init();}
  
  /// destructor
  ~Lsh() { this->clear();}
  
  /// stream overloads
  friend ostream& operator<<(ostream& s, Lsh& l);
  friend istream& operator>>(istream& s, Lsh& l);  
  
  /// clear memory
  void clear() {this->tables.clear();}
  
  /// initialize
  void init();
  
  /// create the Lsh functions to use
  void createFuncs();
  
  /// load and save
  void load(string filename);
  void save(string filename);
  
  /// compute statistics to help decide the right settings for params
  void computeStats();
  
  /// insert points into the Lsh
  ///
  /// lsh       - the Lsh structure
  /// point     - pointer to the points to insert
  /// npoints   - number of points
  /// idshift   - a value to add to the ids of every point
  template <class T>
  friend void insertPoints(Lsh& lsh, Data<T>& data, uint idshift);
  
  /// look for points in the Lsh
  ///
  /// lsh       - the Lsh 
  /// point     - pointer to the point to search for
  /// npoints   - number of points
  /// pls       - an array of point lists to hold data
  template <class T>
  friend void searchPoints(Lsh& lsh, Data<T>& data, LshPointList* pls);
  
  /// return the bucket ids of input points
  ///
  /// lsh       - the Lsh 
  /// point     - pointer to the point to search for
  /// pls       - an array of point lists to hold data
  template <class T>
  friend void getBucketId(Lsh& lsh, Data<T>& data, BucketIdList* ids);
  
  
  /// return the Lsh function values for the given points
  ///
  /// lsh       - the Lsh 
  /// point     - pointer to the point to search for
  /// vals       - an array of point lists to hold data
  template <class T>
  friend void getFuncVal(Lsh& lsh, Data<T>& data, LshFuncValList* vals);          


  /// return the bucket point list for each input bucket
  ///
  /// lsh       - the Lsh 
  /// bids      - input bucket ids
  /// pls       - output point list for every bucket
  friend void getBucketPoints(Lsh& lsh, BucketIdList& bids, uint table, 
          LshPointList* pls);
  
  /// return the k-nearest neighbors
  template <class Tret, class T>
  friend void getKnn(Lsh& lsh, Data<T>& lshData, Data<T>& sdata, 
          uint k, DistanceType dist, uint* ids, Tret* dists);
};

#endif /*CCLSH_HPP_*/
