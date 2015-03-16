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
#define nretIn      prhs[2]

//outputs
#define idsOut      plhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=3)	mexErrMsgTxt("Three inputs required");
  
  //get the lsh objcect
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);
  
  //get the class of points
  mxClassID classID = getClassID(pointsIn); 
//  mxGetClassID(pointsIn);

  //get number of points
//  uint npoints = 
  mwSize npoints = getNpoints(pointsIn);
//  uint npoints = (uint)mxGetN(pointsIn);
//  uint ndims = (uint)mxGetM(pointsIn);

  //number of returns
  mwSize nret = (mwSize) *mxGetPr(nretIn);
  
  //create an array of LshPointList
  LshPointList* pls = new LshPointList[npoints];
  
  //now loop on the points and search
#define __CASE(CLASS)                                                   \
    {                                                                   \
    /*get the data*/                                                    \
    Data<TYPEOF_##CLASS> data;                                          \
    /*set data*/                                                        \
    fillData(data, pointsIn);                                           \
    /*insert*/                                                          \
    searchPoints(*lsh, data, pls);                                      \
    /*clear data*/                                                      \
    data.clear();                                                       \
    }

  __SWITCH(classID, __CASE) 
  
  //allocate output data and get pointer
  //## change the type of this matlab type if uint is something other than unsigned int
  mxClassID retId;
  switch (numeric_limits<uint>::digits)
  {
    case 32: retId = mxUINT32_CLASS; break;
    case 64: retId = mxUINT64_CLASS; break;
  }
  //return all points -> return a cell array
  if (nret == 0)
    idsOut = mxCreateCellMatrix(1, npoints);
  //return an array
  else if (nret > 0)
    idsOut = mxCreateNumericMatrix(nret, (mwSize)npoints, retId, mxREAL);

  //loop on points and put output
  for (uint i=0; i<npoints; ++i)
  {
    //sort the ids and eliminate duplicates
    sort(pls[i].begin(), pls[i].end());
    LshPointIt pit = unique(pls[i].begin(), pls[i].end());
    size_t size = pit - pls[i].begin();    
    //randomize if nret!=0
    if (nret != 0)
      random_shuffle(pls[i].begin(), pit);
    //resize
    pls[i].resize(size);
//    cout << "size = " << size << endl;

    //get pointer
    uint* idsp;
    if (nret>0)     
    {
      //get pointer
      idsp = (uint*) mxGetPr(idsOut);
      idsp += nret*i;
    }
    else            
    {
      //get cell array
      mxArray* pcell = mxCreateNumericMatrix(1, pls[i].size(), retId, mxREAL);
      mxSetCell(idsOut, i, pcell);              
      //allocate
      idsp = (uint*) mxGetPr(pcell);
    }
    
    //get data from this list
    for (uint j=0; (j<nret || nret==0) && j<pls[i].size(); ++j)
    {
      //add 1 to output to make it compatible with matlab
      //indices of ZERO are unused
      idsp[j] = (pls[i])[j] + 1;
//      cout << (pls[i])[j] << endl;
    }
    
    //clear this list
    pls[i].clear();
  }
  //clear list of plist
  delete [] pls;    
    
}

