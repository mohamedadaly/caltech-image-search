// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include <limits>

#include "ccHKmeans.hpp"
#include "mxHKmeans.hpp"
#include "mxDistance.hpp"

                                         

//inputs
#define hkmIn       prhs[0]
#define hkmPointsIn prhs[1]
#define pointsIn    prhs[2]
#define kIn         prhs[3]
#define nchecksIn   prhs[4]

//#define distIn      prhs[4]

//outputs
#define idsOut      plhs[0]
#define distsOut    plhs[1]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs<4)	mexErrMsgTxt("At least four inputs are required");

  //number of nearest neighbors to return 
  uint k = (uint) *mxGetPr(kIn);
  
  //get the class id from hkm
  mxClassID cid = getClassIdMatlabPointer(hkmIn);

  mxArray *dists, *ids;
  uint n1;
#define __CASE(CLASS)                                                     \
  {                                                                       \
  	/*allocate the kdforest*/                                             \
    TYPEOF_##CLASS dummy;                                                 \
    Hkms<TYPEOF_##CLASS>* hkm = getPointerMatlabPointer(hkmIn, dummy);    \
    /*number of checks*/                                                  \
    if (nrhs>4) hkm->opt.nchecks = (int) *(mxGetPr(nchecksIn));            \
    /*datas*/                                                                 \
    Data<TYPEOF_##CLASS> hkmPoints, points;                                   \
    fillData(hkmPoints, hkmPointsIn);                                         \
    fillData(points, pointsIn);                                               \
    /*allocate output*/                                                       \
    n1 = points.size();                                                       \
    dists = mxCreateNumericMatrix(k, n1, mxSINGLE_CLASS, mxREAL);             \
    ids = mxCreateNumericMatrix(k, n1, mxUINT32_CLASS, mxREAL);               \
    /*compute*/                                                               \
    hkm->hkmKnn(hkmPoints, points, k, (uint*) mxGetData(ids),                 \
            (float*) mxGetData(dists));                                       \
    /*clear memory*/                                                          \
    hkmPoints.clear();                                                        \
    points.clear();                                                           \
  }

  __SWITCH(cid, __CASE)
  
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

