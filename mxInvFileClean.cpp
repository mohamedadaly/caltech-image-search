// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include "ccInvertedFile.hpp"


//inputs
#define ivfileIn    prhs[0]
          
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=1)	mexErrMsgTxt("One input required");

  //clear ivfile
  ivFile* ivfile = *(ivFile**)mxGetData(ivfileIn);
  ivfile->clear();
  delete ivfile;
}

