// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010


#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>

#include "ccDistance.hpp"
#include "mxData.hpp"
#include "mxDistance.hpp"

//inputs          
#define points1In prhs[0]
#define points2In prhs[1]
#define kIn       prhs[2]
#define dist      prhs[3]
//outputs          
//#define ret   plhs[0]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=4)	mexErrMsgTxt("Four inputs and one output are required");

  //check class
  mxClassID classID1 = getClassID(points1In); //mxGetClassID(data1);
  mxClassID classID2 = getClassID(points2In);
  if (classID1!=classID2) mexErrMsgTxt("Inputs should be of the same class.");
  
  //get distance function
  DistanceType dt = getDistanceType(dist);
//  char tt[100];
//  mxGetString(dist, tt, 90);
//  DistanceType dt;
//  string str(tt);
//  if (str=="l1")            dt = DISTANCE_L1;
//  else if (str=="l2")       dt = DISTANCE_L2;
//  else if (str=="hamming")  dt = DISTANCE_HAMMING;
//  else if (str=="cos")      dt = DISTANCE_COS;
//  else if (str=="arccos")   dt = DISTANCE_ARCCOS;
//  else if (str=="bhat")     dt = DISTANCE_BHAT;
//  else if (str=="kl")       dt = DISTANCE_KL;
//  else if (str=="jac")      dt = DISTANCE_JAC;
//  else mexErrMsgTxt("Unknown distance function.");
//  
//  //get dimensions  
//  mwSize rows1 = mxGetM(data1);
//  mwSize cols1 = mxGetN(data1);
//  mwSize rows2 = mxGetM(data2);
//  mwSize cols2 = mxGetN(data2);
  
  //get k
  mwSize k = (mwSize) *mxGetPr(kIn);
//  mexPrintf("sizes are: %dx%d %dx%d\n", rows1, cols1, rows2, cols2);
mxArray *dists, *ids;
uint n1;
#define __CASE(CLASS)                                                       \
{                                                                           \
  /*datas*/                                                                 \
  Data<TYPEOF_##CLASS> data1, data2;                                        \
  fillData(data1, points1In);                                               \
  fillData(data2, points2In);                                               \
  /*allocate output*/                                                       \
  n1 = data1.size();                                                        \
  dists = mxCreateNumericMatrix(k, n1, mxSINGLE_CLASS, mxREAL);             \
  ids = mxCreateNumericMatrix(k, n1, mxUINT32_CLASS, mxREAL);               \
  /*compute*/                                                               \
  knn((float*) mxGetData(dists), (uint*) mxGetData(ids), k,                 \
        data1, data2, dt);                                                  \
  /*clear memory*/                                                          \
  data1.clear();                                                            \
  data2.clear();                                                            \
}

    __SWITCH(classID1, __CASE)
    
  //return
  plhs[0] = dists;
  if (nlhs>1) 
  {
    //add one to the ids to make it Matlab compatible
    uint* idp = (uint*) mxGetData(ids);
    for (mwSize i=0; i<k*n1; ++i) *(idp+i) += 1;
    //return
    plhs[1] = ids;  
  }
}
