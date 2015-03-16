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

//output
#define statsOut    plhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=1)	mexErrMsgTxt("One input required");

  //get the pointer
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);

  //compute stats
  lsh->computeStats();
  
  //create the output
  const int nfields = 5;
  const char* fields[] = {"nFullBuckets", "meanBucketSize", 
    "medianBucketSize", "stdBucketSize", "meanCollision"};
  statsOut = mxCreateStructMatrix(1, lsh->opt.ntables, nfields, fields);
  
  //fill in data
  for (mwSize i=0; i<lsh->opt.ntables; ++i)
  {
    mxSetField(statsOut, i, "nFullBuckets", 
            mxCreateDoubleScalar(lsh->tables[i].stats.nFullBuckets));
    mxSetField(statsOut, i, "meanBucketSize", 
            mxCreateDoubleScalar(lsh->tables[i].stats.meanBucketSize));
    mxSetField(statsOut, i, "stdBucketSize", 
            mxCreateDoubleScalar(lsh->tables[i].stats.stdBucketSize));
    mxSetField(statsOut, i, "medianBucketSize", 
            mxCreateDoubleScalar(lsh->tables[i].stats.medianBucketSize));
    mxSetField(statsOut, i, "meanCollision", 
            mxCreateDoubleScalar(lsh->tables[i].stats.meanCollision));
  }
}

