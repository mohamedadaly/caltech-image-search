// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include <limits>

#include "ccLsh.hpp"
#include "mxData.hpp"
#include "mxDistance.hpp"

// //Matlab types
// #define TYPEOF_mxSINGLE_CLASS   float
// #define TYPEOF_mxDOUBLE_CLASS   double
// #define TYPEOF_mxINT8_CLASS     char
// #define TYPEOF_mxUINT8_CLASS    unsigned char
// #define TYPEOF_mxINT32_CLASS    int
// #define TYPEOF_mxUINT32_CLASS   unsigned int
// 
// 
// //creates a switch statement with fun in every statement using "var"
// //as the class ID. "fun" is another macro that takes the class ID.
// #define __SWITCH(var,fun)                               \
//   switch(var)                                           \
//   {                                                     \
//     case mxSINGLE_CLASS:  fun(mxSINGLE_CLASS)  break;   \
//     case mxDOUBLE_CLASS:  fun(mxDOUBLE_CLASS)  break;   \
//     case mxINT8_CLASS:    fun(mxINT8_CLASS)    break;   \
//     case mxUINT8_CLASS:   fun(mxUINT8_CLASS)   break;   \
//     case mxINT32_CLASS:   fun(mxINT32_CLASS)   break;   \
//     case mxUINT32_CLASS:  fun(mxUINT32_CLASS)  break;   \
//   }                                                     

//inputs
#define lshIn       prhs[0]
#define lshPointsIn prhs[1]
#define pointsIn    prhs[2]
#define kIn         prhs[3]
#define distIn      prhs[4]

//outputs
#define idsOut      plhs[0]
#define distsOut    plhs[1]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=5)	mexErrMsgTxt("Five inputs required");
  
  //get the lsh objcect
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);
  
  //get the class of points
  mxClassID classID = getClassID(lshPointsIn); 

  //get number of points in search data
  mwSize npoints = getNpoints(pointsIn);

  //number of nearest neighbors to return 
  uint k = (uint) *mxGetPr(kIn);
  
  //empty distance, then use the one inside the lsh
  DistanceType dist;
  if (mxIsEmpty(distIn))
    dist = lsh->opt.dist;
  else
  	dist = getDistanceType(distIn);
  

  //  mexPrintf("sizes are: %dx%d %dx%d\n", rows1, cols1, rows2, cols2);
  mxArray *dists, *ids;
  uint n1;
  #define __CASE(CLASS)                                                       \
  {                                                                           \
    /*datas*/                                                                 \
    Data<TYPEOF_##CLASS> lshPoints, points;                                   \
    fillData(lshPoints, lshPointsIn);                                         \
    fillData(points, pointsIn);                                               \
    /*allocate output*/                                                       \
    n1 = points.size();                                                       \
    dists = mxCreateNumericMatrix(k, n1, mxSINGLE_CLASS, mxREAL);             \
    ids = mxCreateNumericMatrix(k, n1, mxUINT32_CLASS, mxREAL);               \
    /*compute*/                                                               \
    getKnn(*lsh, lshPoints, points, k,  dist, (uint*) mxGetData(ids),         \
            (float*) mxGetData(dists));                                       \
    /*clear memory*/                                                          \
    lshPoints.clear();                                                        \
    points.clear();                                                           \
  }

  __SWITCH(classID, __CASE)

  
  
  //add one to the ids to make it Matlab compatible
  uint* idp = (uint*) mxGetData(ids);
  float* distp = (float*) mxGetData(dists);
  for (mwSize i=0; i<k*n1; ++i) 
  {
    if (distp[i] < numeric_limits<float>::infinity())
      idp[i] += 1;
  }
  idsOut = ids;
  
  if (nlhs>1) 
    distsOut = dists;  
}

