// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CC_NORMALIZE
#define CC_NORMALIZE

#include <iostream>

#include "ccData.hpp"

using namespace std;

typedef unsigned int uint;

#define EPS 1e-10

// //Access an element in a matrix
// #define _MAT_ELEM_P(mat,row,col,nrows) (mat + (nrows)*(col) + (row))
// #define _MAT_ELEM(mat,row,col,nrows) (*_MAT_ELEM_P(mat,row,col,nrows))


//Types of metrics
typedef enum NormalizeMetric
{
  NORMALIZE_METRIC_L1 = 0,
  NORMALIZE_METRIC_L2,
  NORMALIZE_METRIC_NONE
};

//
// data     - pointer to data, with one column per point i.e. data is 
//            ndimsXnpoints aray
// ndims    - number of rows (dimensions)
// npoints  - number of cols (points)
// metric   - the normalization metric to use
//
template<class T>
void normalize(Data<T>& data, NormalizeMetric metric);


template<class T, class Tret>
void norm(Data<T>& data, NormalizeMetric metric, Vector<Tret>& ret);

#endif

