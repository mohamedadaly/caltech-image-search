// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CCDATA_HPP_
#define CCDATA_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <algorithm>
#include <utility>


using namespace std;

#include "ccVector.hpp"
#include "ccMatrix.hpp"

///-----------------------------------------------------------------------
///Type of data
enum DataType
{
  /// fixed length data
  DATA_FIXED = 0,
  /// variable length data
  DATA_VAR
};



///-----------------------------------------------------------------------
typedef vector<uint> DataFilter;
typedef DataFilter::iterator DataFilterIt;

typedef vector<DataFilter*> DataFilters;

/// Data class
template <typename T>
class Data
{
public:
  /// type of data
  DataType type;
  
  /// number of points
  uint npoints;
  
  /// number of dimensions
  uint ndims;
  
  /// allocated
  bool allocated;
  
  /// data content
  struct _data
  {
    /// fixed data
//     T* fixed;
    Matrix<T> fixed;
    
    /// variable data
    vector< Vector<T> > var;
  } data;
  
  /// filter on the data, so that we only consider the ids given here and not
  /// the whole data
  /// NOTE: the filter does not affect setting individual points
  DataFilters filters;
  
  /// filtered or not?
  int filtered;
  
  //is sparse?
  bool isSparse()
  {
    return this->data.fixed.isSparse();
  }
  
  /// constructor
  Data(DataType type = DATA_FIXED, uint npoints = 0, uint ndims = 0)
  {
    this->init(type, npoints, ndims);
  }
  
  /// copy constructor
  Data(const Data& d) : npoints(d.npoints), type(d.type), ndims(d.ndims),
          allocated(d.allocated), filtered(d.filtered)
  {
    //copy data structure
    this->data.fixed = d.data.fixed;
    this->data.var = d.data.var;    
    //copy filter
    this->filters = d.filters;
  }
  
  /// destructor
  ~Data() { this->clear(); }
  
  /// init
  void init(DataType type = DATA_FIXED, uint npoints = 0, uint ndims = 0)
  {
    this->type = type;
    this->npoints = npoints;
    this->ndims = ndims;
    this->allocated = false;
    this->filtered = 0;
    
    //set fixed dimensions
//     this->data.fixed.init(npoints, ndims, MATRIX_FULL, 0, false);
    this->data.fixed.init(ndims, npoints, MATRIX_FULL, 0, false);
//     cout << "mat.m=" << this->data.fixed.m << " n=" << this->data.fixed.n << endl;
    
  }
  

  /// size
  uint size() 
  {
//     cout << "filtered:" << this->filtered << " filtersize:" << this->filters.size() << " npoints:" << this->npoints << endl;
    return this->filtered > 0 ? this->filters[this->filtered-1]->size() 
      : this->npoints;
  }
  
  /// set the filter
  void setFilter(DataFilter* f)
  {
//    cout << "filter has " << f->size() << endl;
    //add the filter and increment
    this->filters.push_back(f);
//    cout << "filter has " << this->filters[this->filtered]->size() << endl;
    //increment filters
    this->filtered++;
  }
  
  //clear the filter
  void clearFilter()
  {
    if (this->filtered > 0)
    {
      //pop the filter
      this->filters.pop_back();
      //decrement
      this->filtered--;
    }
  }
  
  /// clear memory
  void clear()
  {
    // if not allocated, skip
    if (!this->allocated) return;
    // clear based on type
    switch(this->type)
    {
//       case DATA_FIXED:    delete [] this->data.fixed;   break;
      case DATA_FIXED:    this->data.fixed.clear();     break;
      case DATA_VAR:      this->data.var.clear();       break;
    }
    //clear flag
    this->allocated = false;
    //clear filter
    this->filters.clear();
    this->filtered = 0;
  }
  
  /// allocate memory
  void allocate()
  {
    // allocate memory based on type
    switch(this->type)
    {
      case DATA_FIXED:    
//         this->data.fixed = new T[this->npoints * this->ndims]; 
        data.fixed.init(this->ndims, this->npoints, MATRIX_FULL);
        this->data.fixed.allocate();
        break;
        
      case DATA_VAR:
        this->data.var.resize(this->npoints);
        break;      
    }
    //set flag
    this->allocated = true;
  }
  
  
  /// set data pointer
  ///
  /// datap   - the pointer to the data
  /// cp      - copy the data or just point to it
  void setFixed(T* datap, bool cp=false)
  {
    //clear
    this->clear();
    //set data
    this->data.fixed.set(datap, this->ndims, this->npoints, cp);
//     if (!cp)    
//       this->data.fixed = datap; 
//     else
//     {
//       //allocate
//       this->data.fixed = new T[this->npoints * this->ndims];
//       //copy
//       copy(datap, datap + this->npoints*this->ndims, this->data.fixed);
//       //allocated
//       this->allocated = true;      
//     }
  }
  
  /// set fixed sparse data
  ///
  /// datap   - the pointer to the data
  /// cp      - copy the data or just point to it
  void setFixed(T* datap, index_t* _jc, index_t* _ir, index_t _nz, bool cp = false)
  {
    //clear
    this->clear();
    //set sparse data
    this->data.fixed.set(datap, _jc, _ir, this->dims, this->npoints, _nz, cp);
  }
  
  
  /// gets data pointer to the fixed data
  ///
  /// datap   - the pointer to the data
  /// cp      - copy the data or just point to it
  T* getFixed()
  {
    if (this->data.fixed.isFull())
      return this->data.fixed.data.full;
    else
      return NULL;
  }

  /// set a point in a fixed array
  ///
  /// datap   - the input pointer of allocated data
  /// ndims   - the size of this array
  /// pid     - the point id
  /// cp      - copy the data or just point to it
  void setFixedPoint(T* datap, uint pid)
  {
//    //clear this point
//    this->data.var[pid].clear();
    if (this->data.fixed.isFull())
      //set the point
      copy(datap, datap + this->ndims, this->data.fixed.data.full + pid * this->ndims);
  }

  /// set a point
  ///
  /// datap   - the input pointer of allocated data
  /// ndims   - the size of this array
  /// pid     - the point id
  /// cp      - copy the data or just point to it
  void setVarPoint(T* datap, uint ndims, uint pid, bool cp=false)
  {
//    //clear this point
//    this->data.var[pid].clear();
    //set the point
    this->data.var[pid].set(datap, ndims, cp);      
  }
  
  /// return the true point id in case the data is filtered
  uint filterId(uint pid)
  {
    //filtered
    if (this->filtered > 0)
      for (int i=this->filters.size()-1; i>=0; i--)
      {
//         cout << "b4:" << pid;
        pid = (*this->filters[i])[pid];
//         cout << " af:" << pid << " size:" << this->size() << endl;
      }
    //return
    return pid;
  }
  
  /// compute norms for a point  
  ///
  /// pid   - the point id
  /// norm1 - the sum of values
  /// norm2 - the sum of squared values
  void norm(uint pid, float& norm1, float& norm2)
  {
    //get id
    pid = this->filterId(pid);
    
    //init
    norm1 = norm2 = 0.;
    //type?
    switch(this->type)
    {
      case DATA_FIXED:    
        //loop on this point
        for (uint i=0; i<this->ndims; ++i)
        {
//           float v = (float)this->data.fixed[pid * this->ndims + i];
          float v = (float)this->data.fixed.get(i, pid);
          norm1 += v;
          norm2 += v * v;
        }
        break;
        
      case DATA_VAR:
        //go to this vector and compute norms
        for (uint i=0; i<this->data.var[pid].size(); ++i)
        {
          float v = (float) this->data.var[pid][i];
          norm1 += v;
          norm2 += v * v;
        }
        break;
    }
  }
  
  /// compute norms for all points
  ///
  /// norm1 - pointer to array to hold sum of values (pre-allocated)
  /// norm2 - pointer to array to hold sum of squared values (pre-allocated)
  void norm(float* norm1, float* norm2)
  {
    //loop on points
    uint size = this->size();
    for (uint i=0; i<size; ++i)
      this->norm(i, *(norm1+i), *(norm2+i));
  }
  
  /// get a pointer to a point
  ///
  /// pid   - the point id
  ///
  /// out   - a pair with the pionter and the number of dimensions
  pair<T*,uint> getPoint(uint pid)
  {
    //get id
    pid = this->filterId(pid);

    pair<T*, uint> ret(NULL, 0);
    switch(this->type)
    {
      case DATA_FIXED:
        if (this->data.fixed.isFull())
        {
          ret.first = &this->data.fixed.data.full[pid * this->ndims];
          ret.second = this->ndims;
        }
        break;
        
      case DATA_VAR:
//         ret.first = &(this->data.var[pid][0]);
//         cout << "pid:" << pid << " size:" << this->data.var.size() << endl;
        ret.first = this->data.var[pid].begin();
        ret.second = this->data.var[pid].size();
        break;
    }    
    return ret;
  }  
  
  /// get a value in a specific dimension of a given point
  ///
  /// pid   - the point id
  /// dim   - which dimension
  ///
  /// val   - the required value
  inline T getPointVal(uint pid, uint dim)
  {
    //get id
    pid = this->filterId(pid);

    T val;
    switch(this->type)
    {
      case DATA_FIXED:
//         val = this->data.fixed[pid * this->ndims + dim];
        val = this->data.fixed.get(dim, pid);
        break;
        
      case DATA_VAR:
        val = this->data.var[pid][dim];
        break;
    }    
    return val;
  }  

  /// return a specific element in a sparse point
  ///  
  // col: The index of the column to work on
  // elIndex: the index of the element in that column, ranging from 0
  //      to colSize(col)
  // val: the output value
  // row: The output row number
  // return: true if found, false if not
  bool getSpPointVal(uint pid, uint elIndex, T& val, uint& row)
  {
    //filter point
    uint id = this->filterId(pid);
    //return stuff
    return this->data.fixed.getColVal(pid, elIndex, val, row);
  }
  
  /// sets a specific element in a sparse point, assuming it already exists
  ///  
  // col: The index of the column to work on
  // elIndex: the index of the element in that column, ranging from 0
  //      to colSize(col)
  // val: the output value
  // return: true if found, false if not
  bool setSpPointVal(uint pid, uint elIndex, T val)
  {
    //filter point
    uint id = this->filterId(pid);
    //return stuff
    return this->data.fixed.setColVal(pid, elIndex, val);
  }
  
  /// get the length of a specific point
  ///
  /// pid   - the point id
  /// dim   - which dimension
  ///
  /// val   - the required value
  inline uint  getPointDim(uint pid)
  {
    uint ret = this->ndims;
    uint id = this->filterId(pid);
    
    //if var, then get size of this point
    if (this->type == DATA_VAR)
      ret = this->data.var[id].size();
//     //sparse point
//     else if (this->data.fixed.isSparse())
//       ret = this->data.fixed.colSize(id);

    return ret;
  }
  
  /// get the length of a specific sparse point i.e. no. of nonzero entries
  ///
  /// pid   - the point id
  /// dim   - which dimension
  ///
  /// val   - the required value
  inline uint  getSpPointDim(uint pid)
  {
    uint id = this->filterId(pid);
    
    return this->data.fixed.colSize(id);
  }
  
  /// set a value in a specific dimension of a given point
  ///
  /// pid   - the point id
  /// dim   - which dimension
  /// val   - the required value
  void setPointVal(uint pid, uint dim, T val)
  {
    //get id
    pid = this->filterId(pid);

    switch(this->type)
    {
      case DATA_FIXED:
        this->data.fixed.set(dim, pid, val);
        break;
        
      case DATA_VAR:
        this->data.var[pid][dim] = val;
        break;
    }    
  }  
  
  /// get maximum dimension for all points
  uint getMaxDim()
  {
    uint ret = this->ndims;
    
    //if variable, then check all points
    if (this->type == DATA_VAR)
    {
      //loop
      ret = 0;
      uint size = this->size();
      for (uint i=0; i<size; ++i)
      {
        uint sz = this->data.var[this->filterId(i)].size();
        ret = sz>ret ? sz : ret;
      }
    }    
    return ret;    
  }
  
};


#endif //CCDATA_HPP_
