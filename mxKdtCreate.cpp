// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010


//#include <stdlib.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "mex.h"

//use kdtree list
#define USE_KDT_PLIST_IN_NODE 0


#include "ccKdt.hpp"
#include "mxData.hpp"
#include "mxDistance.hpp"



#define dataIn      prhs[0]
#define ntreesIn    prhs[1]
#define varrangeIn  prhs[2]
#define meanrangeIn prhs[3]
#define maxdepthIn  prhs[4]
#define minvarIn    prhs[5]
#define cycleIn     prhs[6]
#define distIn      prhs[7]
#define maxbinsIn   prhs[8]
#define sampleIn    prhs[9]
#define bitsperdimIn prhs[10]

#define kdfOut      plhs[0]
          
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
  srand(0xffffff);
      
  //options struct
  KdtOptions opt;
  
  //get input class
  mxClassID classID = getClassID(dataIn);

  //get the inputs
  if (nrhs<1)
    mexErrMsgTxt("At least one input is required");  
  if (nrhs>1) opt.ntrees = (int) *(mxGetPr(ntreesIn));
  if (nrhs>2) opt.varrange= (float) *(mxGetPr(varrangeIn));
  if (nrhs>3) opt.meanrange = (float) *(mxGetPr(meanrangeIn));
  if (nrhs>4) opt.maxdepth = (int) *(mxGetPr(maxdepthIn));
  if (nrhs>5) opt.minvar = (float) *(mxGetPr(minvarIn));
  if (nrhs>6) opt.cycle = (char) *(mxGetPr(cycleIn));
  if (nrhs>7) opt.dist = getDistanceType(distIn); 
  if (nrhs>8) opt.maxbins = (int) *(mxGetPr(maxbinsIn));
  if (nrhs>9) opt.sample = (int) *(mxGetPr(sampleIn));  
  if (nrhs>10) opt.bitsperdim = (int) *(mxGetPr(bitsperdimIn));

//   mexPrintf("depth = %d\n", opt.maxdepth);
//   mexPrintf("bitsperdim = %d\n", opt.bitsperdim);
//  mexPrintf("n inputs = %d\n", nrhs);
//  mexPrintf("sample = %f\n", opt.sample);
//  plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
//  return;
  

  //allocate the kdforest
  Kdf* kdf = new Kdf(opt);
  
  //fill with the data
#define __CASE(CLASS)                                                     \
  {                                                                       \
    /*get the data*/                                                      \
    Data<TYPEOF_##CLASS> data;                                            \
    /*set data*/                                                          \
    fillData(data, dataIn);                                               \
    /*create*/                                                            \
    create(*kdf, data);                                                   \
    /*clear data*/                                                        \
    data.clear();                                                         \
  }

  __SWITCH(classID, __CASE)
  

  //return output
  mxArray* ret = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
  *(Kdf**) mxGetPr(ret) = kdf;
  kdfOut = ret;
  
}




