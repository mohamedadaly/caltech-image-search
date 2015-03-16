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
#define wtIn        prhs[1]
#define normIn      prhs[2]

          
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=3)	mexErrMsgTxt("Three input required");

  //get the ivFile pointer
  ivFile* ivfile = *(ivFile**)mxGetData(ivfileIn);
  
  //get the weighting to use
  char s[500];
  mxGetString(wtIn, s, 500);
  string wtStr = s;
  ivFile::Weight wt;
  if (wtStr == "none")        wt = ivFile::WEIGHT_NONE;
  else if (wtStr == "bin")    wt = ivFile::WEIGHT_BIN;
  else if (wtStr == "tf")     wt = ivFile::WEIGHT_TF;
  else if (wtStr == "tfidf")  wt = ivFile::WEIGHT_TFIDF;
  else                        mexErrMsgTxt("Unknown weighting function");
  
  //get the weighting to use
  mxGetString(normIn, s, 500);
  string normStr = s;
  ivFile::Norm norm;
  if (normStr == "none")       norm = ivFile::NORM_NONE;
  else if (normStr == "l0")    norm = ivFile::NORM_L0;
  else if (normStr == "l1")    norm = ivFile::NORM_L1;
  else if (normStr == "l2")    norm = ivFile::NORM_L2;
  else                        mexErrMsgTxt("Unknown normalization function");
      
  //now compute stats using this weighting
  ivfile->computeStats(wt, norm);
//  ivfile->display();
}

