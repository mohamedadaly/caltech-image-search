// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include "ccDistance.hpp"
#include "mxData.hpp"
#include "mxDistance.hpp"

/*
//Matlab types
#define TYPEOF_mxSINGLE_CLASS   float
#define TYPEOF_mxDOUBLE_CLASS   double
#define TYPEOF_mxINT8_CLASS     char
#define TYPEOF_mxUINT8_CLASS    unsigned char
#define TYPEOF_mxINT32_CLASS    int
#define TYPEOF_mxUINT32_CLASS   unsigned int


//creates a switch statement with fun in every statement using "var"
//as the class ID. "fun" is another macro that takes the class ID.
#define __SWITCH(var,fun)                               \
  switch(var)                                           \
  {                                                     \
    case mxSINGLE_CLASS:  fun(mxSINGLE_CLASS)  break;   \
    case mxDOUBLE_CLASS:  fun(mxDOUBLE_CLASS)  break;   \
    case mxINT8_CLASS:    fun(mxINT8_CLASS)    break;   \
    case mxUINT8_CLASS:   fun(mxUINT8_CLASS)   break;   \
    case mxINT32_CLASS:   fun(mxINT32_CLASS)   break;   \
    case mxUINT32_CLASS:  fun(mxUINT32_CLASS)  break;   \
  }                                                     
*/

//inputs          
#define points1In prhs[0]
#define points2In prhs[1]
#define dist  prhs[2]
//outputs          
//#define ret   plhs[0]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=3)	mexErrMsgTxt("Three inputs and one output are required");

  //check class
  mxClassID classID1 = getClassID(points1In); //mxGetClassID(data1);
  mxClassID classID2 = getClassID(points2In);
  if (classID1!=classID2) mexErrMsgTxt("Inputs should be of the same class.");
  
  //get distance function
  DistanceType dt = getDistanceType(dist);//   char tt[100];
//   mxGetString(dist, tt, 90);
//   DistanceType dt;
//   string dists(tt);
//   if (dists=="l1")            dt = DISTANCE_L1;
//   else if (dists=="l2")       dt = DISTANCE_L2;
//   else if (dists=="hamming")  dt = DISTANCE_HAMMING;
//   else if (dists=="cos")      dt = DISTANCE_COS;
//   else if (dists=="arccos")   dt = DISTANCE_ARCCOS;
//   else if (dists=="bhat")     dt = DISTANCE_BHAT;
//   else if (dists=="kl")       dt = DISTANCE_KL;
//   else if (dists=="jac")      dt = DISTANCE_JAC;
//   else mexErrMsgTxt("Unknown distance function.");
  
  //get dimensions  
//  mwSize rows1 = mxGetM(data1);
//  mwSize cols1 = mxGetN(data1);
//  mwSize rows2 = mxGetM(data2);
//  mwSize cols2 = mxGetN(data2);
//  mexPrintf("sizes are: %dx%d %dx%d\n", rows1, cols1, rows2, cols2);
  
  //call distance calculation
  void* ret;
#define __CASE(CLASS)                                                       \
{                                                                           \
  /*datas*/                                                                 \
  Data<TYPEOF_##CLASS> data1, data2;                                        \
  fillData(data1, points1In);                                               \
  fillData(data2, points2In);                                               \
  /*compute*/                                                               \
  plhs[0] = mxCreateNumericMatrix(data2.size(), data1.size(),               \
          mxSINGLE_CLASS, mxREAL);                                          \
  distance((float *) mxGetData(plhs[0]), data1, data2, dt);                 \
  /*clear memory*/                                                          \
  data1.clear();                                                            \
  data2.clear();                                                            \
}
            
//    /*copy into a matlab matrix*/                                     \
//    memcpy(mxGetData(plhs[0]), ret, sizeof(TYPEOF_##CLASS)*cols1*cols2); \
//    delete[] (TYPEOF_##CLASS *)ret;
//    for (mwSize i=0; i<cols2; ++i) mexPrintf("%f ", ((TYPEOF_##CLASS *)ret)[i]);          \
//    memcpy(mxGetData(plhs[0]), ret, sizeof(TYPEOF_##CLASS)*cols1*cols2); \
//    delete[] (TYPEOF_##CLASS *)ret;
    __SWITCH(classID1, __CASE)
    
    
}
