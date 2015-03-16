// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include <cassert>
#include <cmath>
#include <algorithm>

#include "ccCommon.hpp"
#include "ccDistance.hpp"

//# TODO
// Fix to work better with Sparse Matrices, and just loop over filled entries 
// in the sparse vector


//Distance macros
#define COMPUTE_L1(d, data1, data2)                     \
  d = 0;                                                        \
  if (sparse)                                                               \
    for (row=0; row<rows1; ++row)                         \
    {                                                             \
      d += (Tret)fabs(data1.getPointVal(col1,row) - data2.getPointVal(col2,row));   \
    }                                                                           \
  else      \
  {\
    pair<T*, uint> point1 = data1.getPoint(col1), point2=data2.getPoint(col2);\
    for (row=0; row<rows1; ++row)                         \
      d += (Tret)fabs((point1.first[row]) - (point2.first[row]));   \
  }
          
  //d += (Tret)fabs((point1.first[row]) - (point2.first[row]));   \
  
#define COMPUTE_L2(d, data1, data2) /*, point1, point2)*/          \
  d = 0;                                                        \
  Tret t = 0;                                                   \
  if (sparse)                                                               \
    for (row=0; row<rows1; ++row)                         \
    {                                                             \
      t = (Tret)data1.getPointVal(col1, row) - (Tret)data2.getPointVal(col2, row); \
      /*t = (Tret)point1.first[row] - (Tret)point2.first[row];      */\
      d += t * t;                                                 \
    }                                                             \
  else      \
  {\
    pair<T*, uint> point1 = data1.getPoint(col1), point2=data2.getPoint(col2);\
    for (row=0; row<rows1; ++row)                         \
    {\
      t = (Tret)point1.first[row] - (Tret)point2.first[row];      \
      d += t * t;                                                 \
    }\
  } \
  d = sqrt(d);                                                  
  
  
#define COMPUTE_HAMMING(d, data1, data2)                      \
  d = 0;                                                        \
  if (sparse)                                                               \
    for (row=0; row<rows1; ++row)                         \
      d = data1.getPointVal(col1, row) == data2.getPointVal(col2, row) ? d : d+1;     \
  else      \
  {\
    pair<T*, uint> point1 = data1.getPoint(col1), point2=data2.getPoint(col2);\
    for (row=0; row<rows1; ++row)                         \
      d = point1.first[row] == point2.first[row] ? d : d+1;     \
  }
            
//computes the hamming distance between packed binary values
#define COMPUTE_XOR(d, data1, data2)                                        \
{                                                                           \
	d = 0;                                                                    \
	uint64_t val;                                                             \
	if (sparse)                                                               \
    for (row=0; row<rows1; ++row)                                           \
    {                                                                       \
      val = (uint64_t)data1.getPointVal(col1, row) ^                        \
            (uint64_t)data2.getPointVal(col2, row);                         \
      while (val>0)                                                         \
      {                                                                     \
        d++; val &= val - 1;                                                \
      }                                                                     \
    }                                                                       \
  else                                                                      \
  {                                                                         \
    pair<T*, uint> point1 = data1.getPoint(col1),                           \
      point2=data2.getPoint(col2);                                          \
    for (row=0; row<rows1; ++row)                                           \
    {                                                                       \
      val = (uint64_t)point1.first[row] ^ (uint64_t)point2.first[row];      \
      while (val>0)                                                         \
      {                                                                     \
        d++; val &= val - 1;                                                \
      }                                                                     \
    }                                                                       \
  }                                                                         \
} \

#define COMPUTE_COS(d, data1, data2)                          \
  /*Tret norm1 = 0, norm2 = 0;*/                                \
  d = 0;                                                        \
  if (sparse)                             \
    for (row=0; row<rows1; ++row)                         \
      /*compute inner product*/                                   \
      d += ((Tret)(data1.getPointVal(col1, row))) *                          \
            (Tret)(data2.getPointVal(col2, row));                            \
  else      \
  {\
    pair<T*, uint> point1 = data1.getPoint(col1), point2=data2.getPoint(col2);\
    for (row=0; row<rows1; ++row)                         \
      d += (Tret) point1.first[row] * (Tret) point2.first[row];     \
  }

#define COMPUTE_COS_D(d, data1, data2)                        \
  COMPUTE_COS(d, data1, data2)                                \
  d = -d;
  
//  d /= (sqrt(norm1) * sqrt(norm2) + EPS);
//    /*get values*/                                              \
//    Tret t1, t2;                                                \
//    t1 = (Tret) _MAT_ELEM(data1,row,col1,rows);                 \
//    t2 = (Tret) _MAT_ELEM(data2,row,col2,rows);                 \
//    /*store norms*/                                             \
//    norm1 +=  t1*t1;                                            \
//    norm2 +=  t2*t2;                                            \

  
#define COMPUTE_ARCCOS(d, data1, data2)                       \
  COMPUTE_COS(d, data1, data2)                                \
  d = acos(d);
  

#define COMPUTE_BHAT(d, data1, data2)                               \
  d = 0;                                                              \
  if (data1.isSparse() || data2.isSparse())                             \
    for (row=0; row<rows1; ++row)                               \
      d += (Tret)sqrt((Tret)data1.getPointVal(col1, row) *  data2.getPointVal(col2, row));    \
  else      \
  {\
    pair<T*, uint> point1 = data1.getPoint(col1), point2=data2.getPoint(col2);\
    for (row=0; row<rows1; ++row)                         \
      d += (Tret) sqrt((Tret)point1.first[row] * (Tret) point2.first[row]);     \
  } \
  d = -log(d) / log(2);                                               \
  
          
#define COMPUTE_KL(d, data1, data2)                           \
  d = 0;                                                        \
  for (row=0; row<rows1; ++row)                         \
  {                                                             \
    /*get values*/                                              \
    Tret t1, t2;                                                \
    t1 = (Tret)data1.getPointVal(col1, row);                               \
    t2 = (Tret)data2.getPointVal(col2, row);                               \
    /*get quotients*/                                           \
    Tret q1, q2;                                                \
    q1 = t1/t2;                                                 \
    q2 = t2/t1;                                                 \
    /*compute*/                                                 \
    d += (t1 * log(q1) / log(2)) + (t2 * log(q2) / log(2));     \
  }                                                             \
  d /= 2;
  
  
//TODO: allocate this vector only once
#define COMPUTE_JAC(d, data1, data2)                          \
{ \
  d = 0;                                                        \
  /*get intersection*/                                          \
  pair<T*, uint> point1 = data1.getPoint(col1);                 \
  pair<T*, uint> point2 = data2.getPoint(col2);                 \
  Tret* it = set_intersection(point1.first, point1.first+point1.second,   \
          point2.first, point2.first+point2.second, inter);               \
  /*compute*/                                                   \
  d = (Tret)(it - inter);                                       \
  d = 1 - d / (point1.second + point2.second - d);              \
} 
//  Tret* inter = new Tret[max(point1.second,point2.second)];     \
//  /*clear*/                                                     \
//  delete [] inter;                                              
 
//Macro for computing total distance between two matrices using 
//passed distance macro. 
#define TOTAL_DISTANCE(ret, DIST, data1, data2)                 \
  {                                                             \
    /*get max dimension*/                                       \
    uint maxdim = max(data1.getMaxDim(), data2.getMaxDim());    \
    Tret* inter = new Tret[maxdim];                             \
    uint col1, col2, n1, n2, row, rows1, rows2;                 \
    n1 = data1.size();                                          \
    n2 = data2.size();                                          \
    Tret d = 0;                                                 \
    bool sparse = data1.isSparse() || data2.isSparse();          \
    for (col1=0; col1<n1; ++col1)                               \
    {                                                           \
      rows1 = data1.getPointDim(col1);                          \
      /*pair<T*, uint> point1 = data1.getPoint(col1);             */\
      for (col2=0; col2<n2; ++col2)                             \
      {                                                         \
        /*pair<T*, uint> point2 = data2.getPoint(col2);           */\
        rows2 = data2.getPointDim(col2);                        \
        DIST(d, data1, data2)                                     \
        _MAT_ELEM(ret,col2,col1,n2) =  d;                       \
      }                                                         \
    }                                                           \
    /*clear data*/                                              \
    delete [] inter;                                            \
  }

//Macro for computing total distance between one matrix and itself
#define TOTAL_DISTANCE_SELF(ret, DIST, data)                    \
  {                                                             \
    /*get max dimension*/                                       \
    uint maxdim = data.getMaxDim();                             \
    Tret* inter = new Tret[maxdim];                             \
    uint n, col1, col2, row, rows1, rows2;                          \
    n = data.size();                                            \
    Tret d = 0;                                                 \
    bool sparse = data.isSparse();                              \
    for (col1=0; col1<n; ++col1)                                      \
    {                                                           \
      rows1 = data.getPointDim(col1);                             \
      /*pair<T*, uint> point1 = data.getPoint(c1);                */\
      for (col2=col1+1; col2<n; ++col2)                                 \
      {                                                         \
        /*pair<T*, uint> point2 = data.getPoint(c2);              */\
        rows2 = data.getPointDim(col2);                           \
        DIST(d, data, data)                                         \
        _MAT_ELEM(ret,col1,col2,n) =  d;                            \
        _MAT_ELEM(ret,col2,col1,n) =  d;                            \
      }                                                         \
    }                                                           \
    /*clear data*/                                              \
    delete [] inter;                                            \
  }


#define SWAP(A, B, C)   (C) = (A); (A) = (B); (B) = (C); 


//Macro for k nearest neighbors for a passed distance function
#define KNN_DISTANCE(dists, ids, DIST, data1, data2)            \
  {                                                             \
    /*get max dimension*/                                       \
    uint maxdim = max(data1.getMaxDim(), data2.getMaxDim());    \
    Tret* inter = new Tret[maxdim];                             \
    uint n1, n2, col1, col2, row, rows1, rows2;                 \
    uint i, j;                                                  \
    n1 = data1.size();                                          \
    n2 = data2.size();                                          \
    /*cout << "n1:" << n1 << " n2:" << n2 << endl;            */\
    /*max value and its id*/                                    \
    Tret maxVal = numeric_limits<Tret>::infinity();             \
    uint maxId = 0;                                             \
    Tret tempDist; uint tempId;                                 \
    /*set dists into a large value*/                            \
    fill(dists, dists+k*n1, maxVal);                            \
    Tret d = 0;                                                 \
    bool sparse = data1.isSparse() || data2.isSparse();         \
    /*loop on points*/                                          \
    for (col1=0; col1<n1; ++col1)                               \
    {                                                           \
      /*pair<T*, uint> point1 = data1.getPoint(col1);             */\
      rows1 = data1.getPointDim(col1);                          \
      for (col2=0; col2<n2; ++col2)                             \
      {                                                         \
        /*pair<T*, uint> point2 = data2.getPoint(col2);           */\
        rows2 = data2.getPointDim(col2);                        \
        /*get distance in d*/                                   \
        DIST(d, data1, data2)                                     \
        /*get position in already computed distances*/          \
        for(i=0; i<k && d>=_MAT_ELEM(dists,i,col1,k); ++i);   /* > */   \
        /*check*/                                               \
        if (i<k)                                                \
        {                                                       \
          /*move back values to the back of the array*/         \
          for(j=k-1; j>i; --j)                                  \
          {                                                     \
            SWAP(_MAT_ELEM(dists,j,col1,k),                     \
                 _MAT_ELEM(dists,j-1,col1,k), tempDist)         \
            SWAP(_MAT_ELEM(ids,j,col1,k),                       \
                 _MAT_ELEM(ids,j-1,col1,k), tempId)             \
          }                                                     \
          /*put in place*/                                      \
          _MAT_ELEM(dists,i,col1,k) = d;                        \
          _MAT_ELEM(ids,i,col1,k) = col2;                       \
        }                                                       \
      }                                                         \
    }                                                           \
    /*clear data*/                                              \
    delete [] inter;                                            \
  }

//Computes distance from every point in data1 to every point in data2
//Output is a matrix of cols2 * cols1 i.e. it has a column for every
//point in data1 and ret(row,col) is the distance from piont "col" in
//data1 to point "row" in data2
template<class Tret, class T>
void distance(Tret* ret, Data<T>& data1, Data<T>& data2, DistanceType dist)
{
//  assert(rows1==rows2);
  
  //loop
  switch(dist)
  {
    //L1 distance
    case DISTANCE_L1:
      TOTAL_DISTANCE(ret, COMPUTE_L1, data1, data2)
      break;      
      
    //L2 distance
    case DISTANCE_L2:
      TOTAL_DISTANCE(ret, COMPUTE_L2, data1, data2)
      break;
      
    //Hamming distance
    case DISTANCE_HAMMING:
      TOTAL_DISTANCE(ret, COMPUTE_HAMMING, data1, data2)
      break;
      
    //Hamming distance
    case DISTANCE_XOR:
      TOTAL_DISTANCE(ret, COMPUTE_XOR, data1, data2)
      break;

    //Cosine distance
    case DISTANCE_COS:
      TOTAL_DISTANCE(ret, COMPUTE_COS, data1, data2)
      break;
      
    case DISTANCE_ARCCOS:
      TOTAL_DISTANCE(ret, COMPUTE_ARCCOS, data1, data2)
      break;
      
    //Bhattacharya Coefficient
    case DISTANCE_BHAT:
      TOTAL_DISTANCE(ret, COMPUTE_BHAT, data1, data2)
      break;
  
    //KL divergence
    case DISTANCE_KL:
      TOTAL_DISTANCE(ret, COMPUTE_KL, data1, data2)
      break;
      
    //Jacard distance
    case DISTANCE_JAC:
      TOTAL_DISTANCE(ret, COMPUTE_JAC, data1, data2)
      break;
      
  } 
}

//Computes distance from every point in data to every point in data
//Output is a matrix of cols * cols that has output ret(row,col) the 
//distance between point "row" and point "col"
template<class Tret, class T>
void distance_self(Tret* ret, Data<T>& data, DistanceType dist)
{
//  assert(rows==cols);
  
  //loop
  switch(dist)
  {
    //L1 distance
    case DISTANCE_L1:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_L1, data)
      break;      
      
    //L2 distance
    case DISTANCE_L2:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_L2, data)
      break;
      
    //Hamming distance
    case DISTANCE_HAMMING:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_HAMMING, data)
      break;
      
    //Hamming distance
    case DISTANCE_XOR:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_XOR, data)
      break;

    //Cosine distance
    case DISTANCE_COS:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_COS, data)
      break;
      
    case DISTANCE_ARCCOS:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_ARCCOS, data)
      break;
      
    //Bhattacharya Coefficient
    case DISTANCE_BHAT:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_BHAT, data)
      break;
  
    //KL divergence
    case DISTANCE_KL:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_KL, data)
      break;    
      
    //Jacard distance
    case DISTANCE_JAC:
      TOTAL_DISTANCE_SELF(ret, COMPUTE_JAC, data)
      break;
      
  } 
}


// Get the k nearest neighbors from each point in data1 to each point in 
// data2. The "dists" matrix should be already allocated, and should have
// k rows and cols1 columns, and contains the returned computed distances.
// The "ids" matrix should be the same size as "dists" and contains the ids
// of points in data2. Results are arranged ascendingly.
template<class Tret, class T>
void knn(Tret* dists, uint* ids, uint k, Data<T>& data1, Data<T>& data2, DistanceType dist)
{
  //assert rows1==rows2
//  assert(rows1 == rows2);
    //loop
  switch(dist)
  {
    //L1 distance
    case DISTANCE_L1:
      KNN_DISTANCE(dists, ids, COMPUTE_L1, data1, data2)
      break;      
      
    //L2 distance
    case DISTANCE_L2:
      KNN_DISTANCE(dists, ids, COMPUTE_L2, data1, data2)
      break;
      
    //Hamming distance
    case DISTANCE_HAMMING:
      KNN_DISTANCE(dists, ids, COMPUTE_HAMMING, data1, data2)
      break;

    //Hamming distance
    case DISTANCE_XOR:
      KNN_DISTANCE(dists, ids, COMPUTE_XOR, data1, data2)
      break;
      
    //Cosine distance
    case DISTANCE_COS:
      KNN_DISTANCE(dists, ids, COMPUTE_COS_D, data1, data2)
      break;
      
    case DISTANCE_ARCCOS:
      KNN_DISTANCE(dists, ids, COMPUTE_ARCCOS, data1, data2)
      break;
      
    //Bhattacharya Coefficient
    case DISTANCE_BHAT:
      KNN_DISTANCE(dists, ids, COMPUTE_BHAT, data1, data2)
      break;
  
    //KL divergence
    case DISTANCE_KL:
      KNN_DISTANCE(dists, ids, COMPUTE_L1, data1, data2)
      break;    
      
    //KL divergence
    case DISTANCE_JAC:
      KNN_DISTANCE(dists, ids, COMPUTE_JAC, data1, data2)
      break;    
      
  }   
}


// #define TEMPLATE(F)         \
//   F(float, int)             \
//   F(double, int)            \
//   F(float, float)           \
//   F(double, float)          \
//   F(float, double)          \
//   F(double, double)         \
//   F(float, char)            \
//   F(double, char)           \
//   F(float, unsigned int)    \
//   F(double, unsigned int)   \
//   F(float, unsigned char)   \
//   F(double, unsigned char)  
//   
  
#define DISTANCE_D(T) \
  template void distance(double*, Data<T>&, Data<T>&, DistanceType);
#define DISTANCE_F(T) \
  template void distance(float*, Data<T>&, Data<T>&, DistanceType);

TEMPLATE(DISTANCE_F)
TEMPLATE(DISTANCE_D)

#define KNN_F(T)      \
  template void knn(float*, uint*, uint, Data<T>&, Data<T>&, DistanceType);
#define KNN_D(T)      \
  template void knn(double*, uint*, uint, Data<T>&, Data<T>&, DistanceType);

TEMPLATE(KNN_F)
TEMPLATE(KNN_D)

#define DISTANCE_SELF_F(T) \
  template void distance_self(float*, Data<T>&, DistanceType);
#define DISTANCE_SELF_D(T) \
  template void distance_self(double*, Data<T>&, DistanceType);

TEMPLATE(DISTANCE_SELF_F)
TEMPLATE(DISTANCE_SELF_D)
