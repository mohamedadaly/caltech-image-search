// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include "ccInvertedFile.hpp"

//inputs
#define filenameIn  prhs[0]
//outputs          
//#define dists plhs[0]
//#define ids   plhs[1]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=1)	mexErrMsgTxt("One input required");

  //get filename
  char s[500];
  mxGetString(filenameIn, s, 500);
  string filename = s;
  
  //load ivfile
  ivFile* ivfile = new ivFile;
  ivfile->load(filename);
  
  //return pointer to ivFile
  mxArray* ret = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
  *(ivFile**) mxGetPr(ret) = ivfile;
  plhs[0] = ret;
  
  //print it out
  ivfile->display();
//  //clear
//  delete ivfile;
}

