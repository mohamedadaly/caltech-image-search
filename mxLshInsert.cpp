// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>

#include "ccLsh.hpp"
#include "mxData.hpp"


//inputs
#define lshIn       prhs[0]
#define pointsIn    prhs[1]
#define idshiftIn   prhs[2]


void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs<2)	mexErrMsgTxt("Two inputs required at least");
  
  //get the lsh objcect
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);
  
  //get the class of points
  mxClassID classID = mxGetClassID(pointsIn);
  //if cell
  if (mxIsCell(pointsIn) && mxGetNumberOfElements(pointsIn)>0)
    classID = mxGetClassID(mxGetCell(pointsIn, 0));


  //id shift
  uint idshift = 0;
  if (nrhs>=3)
    idshift = (uint) *mxGetPr(idshiftIn);
  
//  mexPrintf("norm = %d\n", lsh->opt.norm);
  //get the data
  
  
  //insert the points
  //call function
#define __CASE(CLASS)                                                     \
    {                                                                     \
    /*get the data*/                                                      \
    Data<TYPEOF_##CLASS> data;                                            \
    /*set data*/                                                          \
    fillData(data, pointsIn);                                             \
    /*insert*/                                                            \
    insertPoints(*lsh, data, idshift);                                    \
    /*clear data*/                                                        \
    data.clear();                                                         \
    }
            
    __SWITCH(classID, __CASE) 
}

