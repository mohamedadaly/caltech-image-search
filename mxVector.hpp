// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_VECTOR_HPP
#define MX_VECTOR_HPP

#include "ccVector.hpp"
#include "mxCommon.hpp"

//--------------------------------------------------------------------------
/// fill a Vector from a Matlab matrix
template<class T>
void fillMatrix(Vector<T>& vec, const mxArray* mx, bool cp = false)
{
  //get rows and columns
  size_t n = mxGetNumberOfElements(mx);
     
  //set the data
  vec.set((T*) mxGetData(mx), n, cp);
}

//--------------------------------------------------------------------------
/// fill matlab array from matrix
template<class T>
mxArray* fillMxArray(Vector<T>& vec)
{
  mxArray* mx;
  
  //get the class id
  T t;
  mxClassID cid = getClassIDfromT(t);
    
  //allocate the matrix
  mx = mxCreateNumericMatrix(1, vec.n, cid, mxREAL);
  //copy the data
  copy(vec.data, vec.data + vec.n, (T*) mxGetData(mx));
  
  return mx;
}



#define FILLMXARRAY_VEC(T) \
  template mxArray* fillMxArray(Vector<T>&);

#define FILLMATRIX_VEC(T) \
  template void fillMatrix(Vector<T>&, const mxArray*, bool);

TEMPLATE(FILLMATRIX_VEC)
TEMPLATE(FILLMXARRAY_VEC)

#undef FILLMATRIX_VEC
#undef FILLMXARRAY_VEC


#endif
