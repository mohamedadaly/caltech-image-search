// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>

#include "ccInvertedFileExtra.hpp"
#include "mxInvFileExtra.hpp"


//inputs
#define ivfileIn    prhs[0]
#define wtIn        prhs[1]
#define normIn      prhs[2]

          
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=3)	mexErrMsgTxt("Three input required");

  //get the ivFile pointer
  ivFile* ivfile = *(ivFile**)mxGetData(ivfileIn);
  
  //get the weighting to use
  ivFile::Weight wt = getInvFileWeight(wtIn);
  
  //get the normalization to use
  ivFile::Norm norm = getInvFileNorm(normIn);
  
  //now compute stats using this weighting
  ivfile->computeStats(wt, norm);
//  ivfile->display();
}

