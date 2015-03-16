// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include "ccInvertedFileExtra.hpp"

#include "mxData.hpp"


//inputs
#define ivFileIn  prhs[0]
#define dataIn    prhs[1]
#define nwordsIn  prhs[2]
#define idshiftIn prhs[3]
//#define tfidfIn   prhs[4]
//outputs          
#define ivFileOut plhs[0]
//#define ids   plhs[1]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs<3)	mexErrMsgTxt("AT least three inputs are required");

  //get the class of points
  mxClassID classID = mxGetClassID(dataIn);
  //if cell
  if (mxIsCell(dataIn) && mxGetNumberOfElements(dataIn)>0)
  {
    for (uint i=0; i<mxGetNumberOfElements(dataIn); ++i)
    {
      mxArray* el = mxGetCell(dataIn, i);
      if (el != NULL)
      {
        classID = mxGetClassID(el);
        break;
      }
    }
  }
  //check class ID, only uint32 and int32 are acceptable now
  if (classID!=mxINT32_CLASS && classID!=mxUINT32_CLASS)
    mexErrMsgTxt("Accepts only int32 & uint32 for input data");
 
  //number of words and docs
  uint nwords = (uint) *mxGetPr(nwordsIn);  
  
  //idshift
  uint idshift = 0;
  if (nrhs>=4)	idshift = (uint) *mxGetPr(idshiftIn);  
  
//  mexPrintf("nwords=%d ndocs=%d ntokens=%d\n", nwords, ndocs, ntokens);

  //make the ivFile object  
  ivFile* ivfile = *(ivFile**)mxGetData(ivFileIn);
  //check if not passed in an object, then create a new one
  if (ivfile == NULL)
    ivfile = new ivFile;

  //call function
#define __CASE(CLASS)                                                     \
    {                                                                     \
    /*get the data*/                                                      \
    Data<TYPEOF_##CLASS> data;                                            \
    /*set data*/                                                          \
    fillData(data, dataIn);                                               \
    /*cout << "npoints:" << data.npoints << endl; */                      \
    /*insert*/                                                            \
    ivFillFile(*ivfile, data, nwords, idshift);                           \
    /*clear data*/                                                        \
    data.clear();                                                         \
    }
        
//     __SWITCH(classID, __CASE)
  switch(classID)
  {
    case mxINT32_CLASS:  __CASE(mxINT32_CLASS);   break;
    case mxUINT32_CLASS: __CASE(mxUINT32_CLASS);  break;
  }
    
    
  //return pointer to ivFile
  mxArray* ret = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
  *(ivFile**) mxGetPr(ret) = ivfile;
  ivFileOut = ret;
}

