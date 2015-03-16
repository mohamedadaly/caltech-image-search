// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include "ccLsh.hpp"


//inputs
#define lshIn       prhs[0]
#define filenameIn  prhs[1]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=2)	mexErrMsgTxt("Two inputs required");

  //get the pointer
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);
  
  //get the filename
  char s[500];
  mxGetString(filenameIn, s, 500);
  string filename = s;
  
  //save the lsh
  lsh->save(filename);
}

