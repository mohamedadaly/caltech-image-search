// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

//use kdtree list
#define USE_KDT_PLIST_IN_NODE 0

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include <limits>

#include "ccKdt.hpp"
#include "mxData.hpp"
#include "mxDistance.hpp"

//inputs
#define kdfIn       prhs[0]
#define kdtPointsIn prhs[1]
#define pointsIn    prhs[2]
#define kIn         prhs[3]
#define tPointsIn   prhs[4]
// #define distIn      prhs[4]

//outputs
#define idsOut      plhs[0]
#define distsOut    plhs[1]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs<4)	mexErrMsgTxt("At least four inputs required");
  
  //get the lsh objcect
  Kdf* kdf = *(Kdf**)mxGetData(kdfIn);
  
  //get the class of points
  mxClassID classID = getClassID(kdtPointsIn); 

  //get number of points in search data
  mwSize npoints = getNpoints(pointsIn);

  //number of nearest neighbors to return 
  uint k = (uint) *mxGetPr(kIn);
  
  //extra traversing data?
  bool extraPoints = false;
  if (nrhs>=5)
    extraPoints = true;
  
  
//  //distance
//  DistanceType dist = getDistanceType(distIn);
  
  //  mexPrintf("sizes are: %dx%d %dx%d\n", rows1, cols1, rows2, cols2);
  mxArray *dists, *ids;
  uint n1;
  #define __CASE(CLASS)                                                       \
  {                                                                           \
    /*datas*/                                                                 \
    Data<TYPEOF_##CLASS> kdtPoints, points, tPoints;                          \
    fillData(kdtPoints, kdtPointsIn);                                         \
    fillData(points, pointsIn);                                               \
    if (extraPoints) fillData(tPoints, tPointsIn);                            \
    /*allocate output*/                                                       \
    n1 = points.size();                                                       \
    dists = mxCreateNumericMatrix(k, n1, mxSINGLE_CLASS, mxREAL);             \
    ids = mxCreateNumericMatrix(k, n1, mxUINT32_CLASS, mxREAL);               \
    /*compute*/                                                               \
    if (!extraPoints)                                                         \
      getKnn(*kdf, kdtPoints, points, k,  (uint*) mxGetData(ids),             \
            (float*) mxGetData(dists), (Data<TYPEOF_##CLASS>*)NULL);          \
    else                                                                      \
    {                                                                         \
      if(points.size() != tPoints.size())                                     \
      {                                                                       \
        mexPrintf("Points and tPoints are not the same size: %d <> %d",       \
          points.size(),  tPoints.size());                                    \
        mexErrMsgTxt("Points and tPoints are not the same size");             \
      }                                                                       \
      getKnn(*kdf, kdtPoints, points, k,  (uint*) mxGetData(ids),             \
            (float*) mxGetData(dists), &tPoints);                             \
    }                                                                         \
    /*clear memory*/                                                          \
    kdtPoints.clear();                                                        \
    points.clear();                                                           \
    if (extraPoints) tPoints.clear();                                         \
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

