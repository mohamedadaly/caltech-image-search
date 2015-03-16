// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>

#include "ccNormalize.hpp"
#include "mxData.hpp"
#include "mxCommon.hpp"

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
#define dataIn    prhs[0]
#define metricIn  prhs[1]
//#define distIn prhs[2]
//#define metricIn   prhs[3]
//outputs          
#define dataOut plhs[0]
//#define ids   plhs[1]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=2)	mexErrMsgTxt("Two inputs required");

  //check class
  mxClassID classID = mxGetClassID(dataIn);
  
  //get dimensions  
  mwSize ndims = mxGetM(dataIn);
  mwSize npoints = mxGetN(dataIn);
  
  //get metric
  char tt[100];
  NormalizeMetric metric;
  mxGetString(metricIn, tt, 90);
  string metricStr(tt);
  if (metricStr=="l1")            metric = NORMALIZE_METRIC_L1;
  else if (metricStr=="l2")       metric = NORMALIZE_METRIC_L2;
  else                            metric = NORMALIZE_METRIC_NONE; 
//    mexErrMsgTxt("Unknown distance function.");

  //allocate output
  dataOut = mxDuplicateArray(dataIn);  //(mxArray*)dataIn; //
  
  //call agg cluster  
#define __CASE(CLASS)                                                   \
  {                                                                     \
    /*get data*/                                                        \
    Data<TYPEOF_##CLASS> data;                                          \
    fillData(data, dataOut);                                            \
    /*normalize*/                                                       \
    normalize(data, metric);                                            \
  }
            
    __SWITCH(classID, __CASE)    
    
}

