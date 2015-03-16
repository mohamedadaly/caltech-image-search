// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef CC_DISTANCE
#define CC_DISTANCE

#include <iostream>

#include "ccData.hpp"

using namespace std;

//typedef unsigned int uint;

#define EPS 1e-10

#define MAX_DIST 1e35

//Access an element in a matrix
#define _MAT_ELEM_P(mat,row,col,nrows) (mat + (nrows)*(col) + (row))
#define _MAT_ELEM(mat,row,col,nrows) (*_MAT_ELEM_P(mat,row,col,nrows))

//Distance types
enum DistanceType
{
  DISTANCE_L1=0,
  DISTANCE_L2,
  DISTANCE_HAMMING,
  DISTANCE_COS,
  DISTANCE_ARCCOS,
  DISTANCE_BHAT,
  DISTANCE_KL,
  DISTANCE_JAC,
  DISTANCE_HISTINT,
  DISTANCE_XOR,
};


//Computes distance from every point in data1 to every point in data2
//Output is a matrix of cols2 * cols1 i.e. it has a column for every
//point in data1 and ret(row,col) is the distance from piont "col" in
//data1 to point "row" in data2
//template<class Tret, class T>
//void distance(Tret* ret, T* data1, uint rows1, uint cols1, 
//        T* data2, uint rows2, uint cols2, DistanceType dist);

//Computes distance from every point in data1 to every point in data2
//Output is a matrix of cols2 * cols1 i.e. it has a column for every
//point in data1 and ret(row,col) is the distance from piont "col" in
//data1 to point "row" in data2
template<class Tret, class T>
void distance(Tret* ret, Data<T>& data1, Data<T>& data2, DistanceType dist);

// Get the k nearest neighbors from each point in data1 to each point in 
// data2. The "dists" matrix should be already allocated, and should have
// k rows and cols1 columns, and contains the returned computed distances.
// The "ids" matrix should be the same size as "dists" and contains the ids
// of points in data2. Results are arranged ascendingly.
//template<class Tret, class T>
//void knn(Tret* dists, uint* ids, uint k, T* data1, uint rows1, uint cols1, 
//        T* data2, uint rows2, uint cols2, DistanceType dist);

// Get the k nearest neighbors from each point in data1 to each point in 
// data2. The "dists" matrix should be already allocated, and should have
// k rows and cols1 columns, and contains the returned computed distances.
// The "ids" matrix should be the same size as "dists" and contains the ids
// of points in data2. Results are arranged ascendingly.
template<class Tret, class T>
void knn(Tret* dists, uint* ids, uint k, Data<T>& data1, Data<T>& data2, 
        DistanceType dist);

//Computes distance from every point in data to every point in data
//Output is a matrix of cols * cols that has output ret(row,col) the 
//distance between point "row" and point "col"
//template<class Tret, class T>
//void distance_self(Tret* ret, T* data, uint rows, uint cols, 
//        DistanceType dist);
template<class Tret, class T>
void distance_self(Tret* ret, Data<T>& data, DistanceType dist);

#endif

