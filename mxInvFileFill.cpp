// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>

#include "mxData.hpp"

#include "ccInvertedFile.hpp"

// //Matlab types
// #define TYPEOF_mxSINGLE_CLASS   float
// #define TYPEOF_mxDOUBLE_CLASS   double
// #define TYPEOF_mxINT8_CLASS     char
// #define TYPEOF_mxUINT8_CLASS    unsigned char
// #define TYPEOF_mxINT32_CLASS    int
// #define TYPEOF_mxUINT32_CLASS   unsigned int
// 
// 
// //creates a switch statement with fun in every statement using "var"
// //as the class ID. "fun" is another macro that takes the class ID.
// #define __SWITCH(var,fun)                               \
//   switch(var)                                           \
//   {                                                     \
//     case mxSINGLE_CLASS:  fun(mxSINGLE_CLASS)  break;   \
//     case mxDOUBLE_CLASS:  fun(mxDOUBLE_CLASS)  break;   \
//     case mxINT8_CLASS:    fun(mxINT8_CLASS)    break;   \
//     case mxUINT8_CLASS:   fun(mxUINT8_CLASS)   break;   \
//     case mxINT32_CLASS:   fun(mxINT32_CLASS)   break;   \
//     case mxUINT32_CLASS:  fun(mxUINT32_CLASS)  break;   \
//   }                                                     

//inputs
#define ivFileIn  prhs[0]
#define wlabelIn  prhs[1]
#define dlabelIn  prhs[2]
#define nwordsIn  prhs[3]
#define ndocsIn   prhs[4]
//#define tfidfIn   prhs[4]
//outputs          
#define ivFileOut plhs[0]
//#define ids   plhs[1]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=5)	mexErrMsgTxt("Five inputs required");

  //check class
  mxClassID classID = mxGetClassID(wlabelIn);
  
  //get dimensions  
  mwSize ntokens = mxGetN(wlabelIn);
  
  //number of words and docs
  uint nwords = (uint) *mxGetPr(nwordsIn);  
  uint ndocs = (uint) *mxGetPr(ndocsIn);  
  
//  mexPrintf("nwords=%d ndocs=%d ntokens=%d\n", nwords, ndocs, ntokens);

  //make the ivFile object  
  ivFile* ivfile = *(ivFile**)mxGetData(ivFileIn);
  //check if not passed in an object, then create a new one
  if (ivfile == NULL)
    ivfile = new ivFile;

  //call function
#define __CASE(CLASS)                                                   \
    ivFillFile(*ivfile, (TYPEOF_##CLASS *) mxGetData(wlabelIn),         \
        (TYPEOF_##CLASS *) mxGetData(dlabelIn), ntokens, nwords, ndocs);
            
    __SWITCH(classID, __CASE)
    
  //return pointer to ivFile
  mxArray* ret = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
  *(ivFile**) mxGetPr(ret) = ivfile;
  ivFileOut = ret;
}

