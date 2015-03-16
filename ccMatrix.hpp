// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CC_MATRIX_HPP
#define CC_MATRIX_HPP

// #include <cstdint>
#include <vector>

#include "ccVector.hpp"

using namespace std;

/// Enumeration of the matrix type
typedef enum MatrixType
{
  MATRIX_FULL = 0,
  MATRIX_SPARSE
};

// typedef uint_32t dim_t;
// typedef uint_64t index_t;

//---------------------------------------------------------------------------
/// Matrix class
template<class T>
class Matrix
{
public:
  
  /// matrix type
  MatrixType type;
  
  /// size of matrix: m: number of rows, n: number of columns
  size_t m, n;
  
  /// number of nonzero entries
  index_t nz;
  
  /// allocated or not
  bool allocated;
  
  /// the matrix data
  struct _data
  {
    // pointer to full matrix
    T* full;
    
    // structure to hold sparse matrix
    struct _sparse
    {
      // array to hold row indices
      Vector<index_t> ir;
      // array to hold column indices
      Vector<index_t> jc;
      // array to hold data
      Vector<T> pr;
    } sparse;
  } data;
  
  /// constructor
  Matrix(size_t _m=0, size_t _n=0, MatrixType _type=MATRIX_FULL, 
          index_t _nz=0, bool _allocated=false) 
    : type(_type), m(_m), n(_n), allocated(_allocated), nz(_nz) {}
    
  /// destructor
  ~Matrix() { this->clear(); }
  
  /// initialize
  void init(size_t _m=0, size_t _n=0, MatrixType _type=MATRIX_FULL, 
          index_t _nz=0, bool _allocated=false)
  {
    this->m = _m;
    this->n = _n;
    this->type = _type;
    this->nz = _nz;
    this->allocated = _allocated;
  }
  
  /// clear memory
  void clear()
  {
    if (this->allocated)
      switch(this->type)
      {
        case MATRIX_FULL: 
          delete [] this->data.full; 
          break;
          
        case MATRIX_SPARSE:
          this->data.sparse.ir.clear();
          this->data.sparse.jc.clear();
          this->data.sparse.pr.clear();
          break;
      }
    this->allocated = false;
  }
  
  /// allocate memory
  void allocate()
  {
    this->allocated = true;
    //allocate data
    switch(this->type)
    {
      case MATRIX_FULL: 
        this->data.full = new T[this->m * this->n]; 
        break;

      case MATRIX_SPARSE:
        //rows and data have size of nz
        this->data.sparse.ir.resize(this->nz);
        this->data.sparse.ir.init(0);
        this->data.sparse.pr.resize(this->nz);
        //jc has length n+1
        this->data.sparse.jc.resize(this->n + 1);
        this->data.sparse.jc.init(0);
        break;
    }    
  }
  
  /// set the matrix to a full matrix
  void set(T* d, size_t _m, size_t _n, bool cp = false)
  {
    //clear
    this->clear();
    //set type and dims
    this->type = MATRIX_FULL;
    this->m = _m;
    this->n = _n;
    
    //copy
    if (cp)
    {
      //allocate
      this->allocate();
      copy(d, d+ this->m * this->n, this->data.full);
      this->allocated = true;
    }
    else
      this->data.full = d;
  }
 
  /// set the matrix to a sparse matrix
  void set(T* _pr, index_t* _jc, index_t* _ir, size_t _m, size_t _n, 
          index_t _nz, bool cp = false)
  {
    //clear
    this->clear();
    //set type and dims
    this->type = MATRIX_SPARSE;
    this->m = _m;
    this->n = _n;
    this->nz = _nz;
    //put data
    this->data.sparse.pr.set(_pr, _nz, cp);
    this->data.sparse.ir.set(_ir, _nz, cp);
    this->data.sparse.jc.set(_jc, _n+1, cp);
    //set copy
    this->allocated = cp;
  }
  
  /// set matrix to another matrix
  void set(Matrix<T>& mat, bool cp = false)
  {
    //clear this one
    this->clear();
    // set
    this->init(mat.m, mat.n, mat.type, mat.nz, mat.allocated);
    
    //copy data or just point to it
    if (cp)
    {
      switch(this->type)
      {
        case MATRIX_FULL:
          this->set(mat.data.full, mat.m, mat.n, true);
          break;
          
        case MATRIX_SPARSE:
          this->set(mat.data.sparse.pr.begin(), mat.data.sparse.jc.begin(), 
                  mat.data.sparse.ir.begin(), mat.m, mat.n, mat.nz, true);
          break;
      }
    }
  }
  
  /// return the index of the sparse element in index, which indexes into 
  /// ir and pr arrays
  /// returns true if it exists, otherwise false
  bool spIndex(size_t row, size_t col, index_t& index)
  {
    bool exist = false;
    //get the staring index for this column
    index_t rstart = this->data.sparse.jc[col];
    index_t rend = this->data.sparse.jc[col+1];
    //column has something
    if (rend > rstart)
    {
      //search for that row
      size_t* bg = this->data.sparse.ir.begin(); 
      index_t* r = lower_bound(bg + rstart, bg + rend, row);
      index = r - bg;
//       cout << endl << "row:" << row << " col:" << col << " rstart:" << rstart << " rend:" << rend << " index:" << index << endl;
      //compare 
      exist = index<rend &&  *r == row;
    }
    //empty column
    else
      index = rstart;
    
    return exist;
    
  }
  
  /// returns the number of elements in column col
  size_t colSize(size_t col)
  {
    if (this->isSparse())
    {
      //get the staring index for this column
      index_t rstart = this->data.sparse.jc[col];
      index_t rend = this->data.sparse.jc[col+1];
      //count number of elements
      return (rend - rstart);
    }
    else
      return this->m;
  }
  
  //return specific element of a specific column
  // col: The index of the column to work on
  // elIndex: the index of the element in that column, ranging from 0
  //      to colSize(col)
  // val: the output value
  // row: The output row number
  // return: true if found, false if not
  bool getColVal(uint col, uint elIndex, T& val, uint& row)
  {
    //default
    bool found = false;
    //sparse
    if (this->isSparse())
    {
      //get the staring index for this column
      index_t rstart = this->data.sparse.jc[col];
      index_t rend = this->data.sparse.jc[col+1];
      index_t index = rstart + elIndex;
      //column has something
      if (index <= rend)
      {
        found = true;
        //get point data & row
        val = this->data.sparse.pr[index];
        row = this->data.sparse.ir[index];
      }
    }
    else
    {
      found = true;
      row = elIndex;
      val = this->get(row, col);
    }
    
    //return
    return found;
  }
   
  //sets a specific element of a specific column, assuming it already exists 
  // i.e. getColVal returns true for it
  //
  // col: The index of the column to work on
  // elIndex: the index of the element in that column, ranging from 0
  //      to colSize(col)
  // val: the output value
  // row: The output row number
  // return: true if found, false if not
  bool setColVal(uint col, uint elIndex, T val)
  {
    //default
    bool found = false;
    //sparse
    if (this->isSparse())
    {
      //get the staring index for this column
      index_t rstart = this->data.sparse.jc[col];
      index_t rend = this->data.sparse.jc[col+1];
      index_t index = rstart + elIndex;
      //column has something
      if (index <= rend)
      {
        found = true;
        //set point data
        this->data.sparse.pr[index] = val;
      }
    }
    else
    {
      found = true;
      this->set(elIndex, col, val);
    }
    
    //return
    return found;
  }
  
  /// insert a sparse element at index, row, & col
  /// index is the value returned from spIndex, which would be the position
  /// to insert the value into ir and pr
  void spInsert(size_t row, size_t col, index_t index)
  {
    //insert an element into ir and pr
//     cout << "before ir: "; for (uint i=0; i<data.sparse.ir.n; ++i) cout << " " << data.sparse.ir[i]; cout << endl;
    this->data.sparse.ir.insert(row, index);
//     cout << "after ir: "; for (uint i=0; i<data.sparse.ir.n; ++i) cout << " " << data.sparse.ir[i]; cout << endl;
    this->data.sparse.pr.insert((T) 0, index);
    //increment next columns anyway
//     cout << "before jc: "; for (uint i=0; i<data.sparse.jc.n; ++i) cout << " " << data.sparse.jc[i]; cout << endl;    
    for (uint i=col+1; i<this->n+1; ++i)
      this->data.sparse.jc[i] = this->data.sparse.jc[i] + 1;    
    //check if there's a smaller value in that column, then update
    if (this->data.sparse.jc[col] > index)	this->data.sparse.jc[col] = index;
//     cout << "after jc: "; for (uint i=0; i<data.sparse.jc.n; ++i) cout << " " << data.sparse.jc[i]; cout << endl;
    //increment nz
    this->nz++;
  }
   
  /// check if sparse element exists or not?
  bool spExist(size_t row, size_t col)
  {
    index_t t;
    return spIndex(row, col, t);
  }
  
  /// set an element at row/col
  //T& operator() (size_t row, size_t col)
  void set(size_t row, size_t col, T val)
  {    
    switch(this->type)
    {
      case MATRIX_FULL: 
        this->data.full[ this->m * col + row ] = val;
        break;

      case MATRIX_SPARSE:
        //get index
        index_t index;
        bool exist = this->spIndex(row, col, index);
//         cout << "row:" << row << " col:" << col << " index:" << index << " val:" << val << endl;
        //insert if not there
        if (!exist && val!=(T) 0)	
        {
          this->spInsert(row, col, index);
          exist = true;
        }
        if (exist)
          this->data.sparse.pr[index] = val;
        break;
    }    
  }
  
  /// get an element at row/col
  //T operator() (size_t row, size_t col) const
  inline T get(size_t row, size_t col)
  {
    switch(this->type)
    {
      case MATRIX_FULL: 
        return this->data.full[ this->m * col + row ];
        break;

      case MATRIX_SPARSE:
        //get index
        index_t index;
        bool exist = this->spIndex(row, col, index);
        //check
        if (exist)
          return this->data.sparse.pr[index];
        else
          return (T) 0;
        break;
    }    
  }
  
  void print(ostream& os)
  {
    for (uint i=0; i<this->m; ++i)
    {
      for (uint j=0; j<this->n; ++j)
        os << this->get(i, j) << " ";
      os << "\n";
    }
  }
  
  /// is sparse>
  inline bool isSparse() { return this->type == MATRIX_SPARSE; }
  
  /// is full?
  bool isFull() { return this->type == MATRIX_FULL; }
  
};


#endif
