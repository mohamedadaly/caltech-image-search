// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include <limits>

#include "ccLsh.hpp"
#include "mxData.hpp"

//inputs
#define lshIn       prhs[0]
#define pointsIn    prhs[1]

//outputs
#define idsOut      plhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=2)	mexErrMsgTxt("Two inputs required");
  
  //get the lsh objcect
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);
  
  //get the class of points
  mxClassID classID = getClassID(pointsIn); 

  //get number of points
  mwSize npoints; // = getNpoints(pointsIn, true);
//   cout << "npoints:" << npoints << endl;

  //create an array of LshPointList
  BucketIdList* ids;
//   BucketIdList* ids = new BucketIdList[npoints];
  
  //now loop on the points and search
#define __CASE(CLASS)                                                   \
    {                                                                   \
    /*get the data*/                                                    \
    Data<TYPEOF_##CLASS> data;                                          \
    /*set data*/                                                        \
    fillData(data, pointsIn);                                           \
    /*allocate*/                                                        \
    npoints = data.size();                                              \
    ids = new BucketIdList[npoints];                                \
    /*insert*/                                                          \
    getBucketId(*lsh, data, ids);                                       \
    /*clear data*/                                                      \
    data.clear();                                                       \
    }

  __SWITCH(classID, __CASE) 
  
  //allocate output data and get pointer
  //## change the type of this matlab type if uint is something other than unsigned int
  mxClassID retId;
  switch (numeric_limits<BucketId_t>::digits)
  {
    case 32: retId = mxUINT32_CLASS; break;
    case 64: retId = mxUINT64_CLASS; break;
  }
  //return an array
  idsOut = mxCreateNumericMatrix(lsh->opt.ntables, npoints, retId, mxREAL);
  //pointer
  BucketId_t* idsp =  (BucketId_t*) mxGetPr(idsOut);
  
  //loop on points and put output  
  for (uint i=0; i<npoints; ++i)
  {
    for (uint j=0; j<ids[i].size(); ++j)
      idsp[i*lsh->opt.ntables + j] = ids[i][j]; 
    //clear this list
    ids[i].clear();
  }
  //clear list of plist
  delete [] ids;        
}

