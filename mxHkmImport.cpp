// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include "ccHKmeans.hpp"
#include "mxHKmeans.hpp"


#define hkmIn     prhs[0]

#define hkmOut    plhs[0]


void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
  //check inputs
  if (nrhs<1)
    mexErrMsgTxt("one input required");
  
  //get the class id from hkm
  mxClassID cid = getClassIdMatlabPointer(hkmIn);
  
#define __CASE(CLASS)                                                     \
  {                                                                       \
    TYPEOF_##CLASS dummy;                                                 \
    /*import from matlab*/                                                \
    Hkms<TYPEOF_##CLASS>* hkm = importMatlab(hkmIn, dummy);                \
    /*return the matlab pointer*/                                         \
    hkmOut = exportMatlabPointer(*hkm);                                   \
  }

  __SWITCH(cid, __CASE)
  
}

