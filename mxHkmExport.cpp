// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include "ccHKmeans.hpp"
#include "mxHKmeans.hpp"

#define hkmIn     prhs[0]
#define getidsIn  prhs[1]

#define hkmOut    plhs[0]


void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
  //check inputs
  if (nrhs<1)
    mexErrMsgTxt("one input required");
  
  //get the option
  bool getids = false;
  if (nrhs>1) getids = (bool) *(mxGetPr(getidsIn));
  
  //get the class id from hkm
  mxClassID cid = getClassIdMatlabPointer(hkmIn);
  
#define __CASE(CLASS)                                                     \
  {                                                                       \
  	/*allocate the kdforest*/                                             \
    TYPEOF_##CLASS dummy;                                                 \
    Hkms<TYPEOF_##CLASS>* hkm = getPointerMatlabPointer(hkmIn, dummy);     \
    /*export the hkm*/                                                    \
    hkmOut = exportMatlab(*hkm, getids);                                  \
  }

  __SWITCH(cid, __CASE)
  
}

