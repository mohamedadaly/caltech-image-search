// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010


#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>

#include "ccDistance.hpp"
#include "mxCommon.hpp"
#include "mxData.hpp"
#include "mxSumIndexed.hpp"


//inputs          
#define pointsIn  prhs[0]
#define idsIn     prhs[1] 
#define kIn       prhs[2]
//outputs          
#define sumOut    plhs[0]
#define histOut   plhs[1]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=3)	mexErrMsgTxt("Three inputs and two output are required");
  if (mxGetClassID(idsIn)!=mxUINT32_CLASS)
    mexErrMsgTxt("input ids must be uint32 or int32");

  //check class
  mxClassID classID = getClassID(pointsIn); //mxGetClassID(data1);
  
  //get k
  uint k = (uint) *mxGetPr(kIn);
  
  //call distance calculation
  void* ret;
#define __CASE(CLASS)                                                       \
{                                                                           \
  /*datas*/                                                                 \
  Data<TYPEOF_##CLASS> data;                                                \
  fillData(data, pointsIn);                                                 \
  /*allocate output*/                                                       \
	sumOut = mxCreateNumericMatrix(data.ndims, k, mxSINGLE_CLASS, mxREAL);    \
  histOut = mxCreateNumericMatrix(1, k, mxUINT32_CLASS, mxREAL);            \
  Data<float> sum; fillData(sum, sumOut);                                   \
  /*compute*/                                                               \
  sumIndexed(sum, (uint*) mxGetData(histOut), data,                         \
          (uint*) mxGetData(idsIn), k);                                     \
  /*clear memory*/                                                          \
  data.clear(); sum.clear();                                                \
}
           
    __SWITCH(classID, __CASE)
    
    
}
