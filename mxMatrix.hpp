// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_MATRIX_HPP
#define MX_MATRIX_HPP

#include "ccMatrix.hpp"
#include "mxCommon.hpp"

//--------------------------------------------------------------------------
/// fill a Matrix from a Matlab matrix
template<class T>
void fillMatrix(Matrix<T>& mat, const mxArray* mx, bool cp = false)
{
  //clear
  mat.clear();
  
  //get rows and columns
  size_t m = mxGetM(mx);
  size_t n = mxGetN(mx);
     
  //sparse matrix
  if (mxIsSparse(mx))
  {
    //get nz
    index_t nz = mxGetNzmax(mx);
    //set the sparse data
    mat.set((T*) mxGetPr(mx), (index_t*) mxGetJc(mx), (index_t*) mxGetIr(mx), 
            m, n, nz, cp);
  }
  //full matrix
  else
  {
    //set the data
    mat.set((T*) mxGetData(mx), m, n, cp);
//     cout << "mat.m=" << mat.m << " mat.n=" << mat.n << endl;
  }
}

//--------------------------------------------------------------------------
/// fill matlab array from matrix
template<class T>
mxArray* fillMxArray(Matrix<T>& mat)
{
  mxArray* mx;
  
  //get the class id
  T t;
  mxClassID cid = getClassIDfromT(t);
    
  //sparse?
  if (mat.isSparse())
  {
    //allocate the sparse matrix
    mx = mxCreateSparse(mat.m, mat.n, mat.nz, mxREAL);
    //copy data
    copy(mat.data.sparse.pr.begin(), mat.data.sparse.pr.begin() + mat.nz, 
            (T*) mxGetPr(mx));
    copy(mat.data.sparse.ir.begin(), mat.data.sparse.ir.begin() + mat.nz, 
            (index_t*) mxGetIr(mx));
    copy(mat.data.sparse.jc.begin(), mat.data.sparse.jc.begin() + mat.n + 1, 
            (index_t*) mxGetJc(mx));
  }
  //full?
  else
  {
    //allocate the matrix
    mx = mxCreateNumericMatrix(mat.m, mat.n, cid, mxREAL);
    //copy the data
    copy(mat.data.full, mat.data.full + mat.m*mat.n, (T*) mxGetData(mx));
  }
  
  return mx;
}



#define FILLMXARRAY_MAT(T) \
  template mxArray* fillMxArray(Matrix<T>&);

#define FILLMATRIX_MAT(T) \
  template void fillMatrix(Matrix<T>&, const mxArray*, bool);

TEMPLATE(FILLMATRIX_MAT)
TEMPLATE(FILLMXARRAY_MAT)

#undef FILLMXARRAY_MAT
#undef FILLMATRIX_MAT

#endif
