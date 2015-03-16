// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include "ccLsh.hpp"


//inputs
#define filenameIn  prhs[0]

//outputs
#define lshOut      plhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=1)	mexErrMsgTxt("One input required");

  //get the filename
  char s[500];
  mxGetString(filenameIn, s, 500);
  string filename = s;

  //create the Lsh
  Lsh* lsh = new Lsh;
  lsh->load(filename);
  
  //return pointer
  mxArray* ret = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
  *(Lsh**) mxGetPr(ret) = lsh;  
  lshOut = ret;
}

