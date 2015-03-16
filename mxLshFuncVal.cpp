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
#define celloutIn   prhs[2]

//outputs
#define valsOut     plhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
  if (nrhs!=3)	mexErrMsgTxt("Three inputs required");
  
  //get the lsh objcect
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);
  
  //get the class of points
  mxClassID classID = getClassID(pointsIn); 

  //get number of points
  mwSize npoints = getNpoints(pointsIn);

  //number of returns
  bool cellout = (bool) *mxGetPr(celloutIn);
  
  //create an array of LshPointList
  LshFuncValList* vals = new LshFuncValList[npoints];
  
  //now loop on the points and search
#define __CASE(CLASS)                                                   \
    {                                                                   \
    /*get the data*/                                                    \
    Data<TYPEOF_##CLASS> data;                                          \
    /*set data*/                                                        \
    fillData(data, pointsIn);                                           \
    /*insert*/                                                          \
    getFuncVal(*lsh, data, vals);                                       \
    /*clear data*/                                                      \
    data.clear();                                                       \
    }

  __SWITCH(classID, __CASE) 
  
  //allocate output data and get pointer
  //## change the type of this matlab type if uint is something other than unsigned int
  mxClassID retId;
  switch (numeric_limits<LshFuncVal_t>::digits)
  {
    case 32: retId = mxUINT32_CLASS; break;
    case 64: retId = mxUINT64_CLASS; break;
  }
  //check if to return cell: if requested by user, or if have more than one table
  //otherwise, we can just return a matrix where each column is for a point
  //and each row is for a function
  cellout = cellout || (lsh->opt.ntables > 1);
  
  //return all points -> return a cell array
  if (cellout)
    valsOut = mxCreateCellMatrix(1, npoints);
  //return an array
  else
    valsOut = mxCreateNumericMatrix((mwSize)lsh->opt.nfuncs, (mwSize)npoints, 
            retId, mxREAL);

  //loop on points and put output
  for (uint i=0; i<npoints; ++i)
  {
    //get pointer
    LshFuncVal_t* valsp;
    if (cellout)
    {
      //get cell array
      mxArray* pcell = mxCreateNumericMatrix(lsh->opt.nfuncs, lsh->opt.ntables, 
              retId, mxREAL);
      mxSetCell(valsOut, i, pcell);              
      //allocate
      valsp = (LshFuncVal_t*) mxGetPr(pcell);
    }
    else            
    {
      //get pointer
      valsp = (LshFuncVal_t*) mxGetPr(valsOut);
      valsp += lsh->opt.nfuncs*i;
    }
    
    //loop on lists
    for (uint j=0; j<vals[i].size(); ++j)
      //get value
      valsp[j] = vals[i][j];
    
    //clear this list
    vals[i].clear();
  }
  //clear list of plist
  delete [] vals;    
    
}

