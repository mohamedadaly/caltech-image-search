// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CC_VECTOR_HPP
#define CC_VECTOR_HPP

#include <iostream>
#include <algorithm>
#include <stdint.h>

using namespace std;

//typedef size_t uint;
typedef uint32_t uint;

// typedef 
typedef size_t index_t;

///-----------------------------------------------------------------------
// a class to encapsulate a vector that can point to an already allocated
// data or allocate its own data
template <typename T>
class Vector
{
public:
  // the data poitner
  T* data;
  // the current size
  index_t n;
  // allocated or not
  bool allocated;
  // capacity of the vector
  index_t cap;
  
  // iterator
  typedef T* iterator;
  
  /// constructor
  Vector() : n(0), cap(0), data(NULL), allocated(false) {}  
  Vector(T* _data, index_t _n) : n(_n), cap(_n), data(data), allocated(false) {}
  Vector(index_t _n) : n(_n), cap(_n), allocated(true) { this->data = new T[_n]; }
  
  /// destructor
  ~Vector() { this->clear(); }
  
  /// clear memory
  void clear()
  {
    if (this->allocated) delete [] this->data;    
    this->cap = 0;
    this->n = 0;
    this->allocated = false;
  }
  
  
  /// insert data at a specific location
  void insert(const T& val, index_t pos)
  {
    //resize the vector to add a new space
    this->reserve(this->size() + 10);
    
    //shift places
    if (pos < this->n)
      copy_backward(this->data+pos, this->data+this->n, this->data + this->n + 1);
    
    //insert
    this->data[pos] = val;
    
    //increment n
    this->n++;
  }
  
  /// pushes back at the end of the vector
  void push_back(const T& val)
  {
    this->insert(val, this->n);
  }
  
  /// get the lower bound index for the input data
  index_t lower_bound(T& val)
  {
    return (std::lower_bound(this->data, this->data + this->n, val) - this->data);
  }
  
  /// return the index of the val. If it's not there, index = size()
  index_t index(T& val)
  {
    uint i;
    for (i = 0; i<this->n && this->data[i]!=val; ++i);
    return i;
  }
  
  /// function for ascending sorting
  static inline bool lt(T a, T b) { return a<b; }
  /// function for descending sorting
  static inline bool gt(T a, T b) { return a>b; }
  
  /// sort the vector
  void sort(bool ascend = true)
  {
    std::stable_sort(this->data, this->data + this->n, 
            ascend ? Vector::lt : Vector::gt);
  }
  
  /// remove data at a specific position
  void remove(index_t pos)
  {
    //check if we need to shift i.e. not the last element
    if (pos < this->n-1)
    {
      //copy the values to override that at pos
      copy(this->data+pos+1, this->data+this->n, this->data+pos);
    }
    
    //decrement
    this->n--;
  }
  
  /// swap two positions
  void swap(index_t p1, index_t p2)
  {
    T t = this->data[p1];
    this->data[p1] = this->data[p2];
    this->data[p2] = t;
  }
  
  /// set pointer
  void set(T* _data, index_t _n, bool cp = false)
  {
    //clear if we have something
    this->clear();
    //copy?
    if (cp)
    {
      //allocate
      this->resize(_n);
      //copy this data
      copy(_data, _data+_n, this->data);
    }
    //don't copy
    else
    {
      this->data = _data;
    }
    //set n & allocated
    this->allocated = cp;
    this->n = _n;    
  }
  
  /// resize
  void resize(index_t _n)
  {
    //check size
    if (_n > this->n)
    {
      //allocate more memory
      this->reserve(_n);
      //set n
      this->n = _n;
    }
  }
  
  /// reserve memeory wihtout increasing
  void reserve(index_t _n)
  {
    //check size
    if (_n > this->capacity())
    {
      //set new capacity
      this->cap = _n;
      //allocate more memory
      T* temp = new T[_n];
      //copy data if not null
      if (this->data)
        copy(this->data, this->data + this->size(), temp);
      //clear old data
      if (this->allocated)
        delete [] this->data;
      this->data = temp;
      //set allocated
      this->allocated = true;      
    }
  }
  
  /// initialize to some value
  void init(const T& val)
  {
    fill(this->data, this->data+this->n, val);
  }
  
  /// return the size
  index_t size() { return this->n; }
  
  /// capacity
  index_t capacity() { return this->cap; }

  /// access operator
  T& operator[](index_t i) { return this->data[i]; }
  
  
  /// print
  void print(ostream& os)
  {
    for (uint i=0; i<this->n; ++i) os << " " << this->data[i];
    os << "\n";      
  }
  
  /// iterators
  T* begin() { return this->data; }
  T* end() { return this->data+this->n; }
};

#endif
