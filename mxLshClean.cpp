// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include "ccLsh.hpp"

//Matlab types
#define TYPEOF_mxSINGLE_CLASS   float
#define TYPEOF_mxDOUBLE_CLASS   double
#define TYPEOF_mxINT8_CLASS     char
#define TYPEOF_mxUINT8_CLASS    unsigned char
#define TYPEOF_mxINT32_CLASS    int
#define TYPEOF_mxUINT32_CLASS   unsigned int


//creates a switch statement with fun in every statement using "var"
//as the class ID. "fun" is another macro that takes the class ID.
#define __SWITCH(var,fun)                               \
  switch(var)                                           \
  {                                                     \
    case mxSINGLE_CLASS:  fun(mxSINGLE_CLASS)  break;   \
    case mxDOUBLE_CLASS:  fun(mxDOUBLE_CLASS)  break;   \
    case mxINT8_CLASS:    fun(mxINT8_CLASS)    break;   \
    case mxUINT8_CLASS:   fun(mxUINT8_CLASS)   break;   \
    case mxINT32_CLASS:   fun(mxINT32_CLASS)   break;   \
    case mxUINT32_CLASS:  fun(mxUINT32_CLASS)  break;   \
  }                                                     

//inputs
#define lshIn   prhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=1)	mexErrMsgTxt("One input required");

  //get the pointer
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);

  //clear the memory
  lsh->clear();
  delete lsh;  
}

