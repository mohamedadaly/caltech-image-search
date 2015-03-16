// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010


#include "mex.h"

//use kdtree list
#define USE_KDT_PLIST_IN_NODE 0

#include "ccKdt.hpp"


void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
  //check inputs
  if (nrhs<1)
    mexErrMsgTxt("one input required");
  
  //get the kdtree
  Kdf *kdf = *(Kdf**)mxGetData(prhs[0]);
  
  //Clear the memory for a tree
  kdf->clear();
  delete kdf;
}
