// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

//use kdtree list
#define USE_KDT_PLIST_IN_NODE 0

#include <string>
#include <cassert>
#include <cstring>
#include <climits>
#include <limits>

#include "ccKdt.hpp"
#include "mxData.hpp"
#include "mxDistance.hpp"

//inputs
#define kdfIn       prhs[0]
#define pointsIn    prhs[1]
#define kIn         prhs[2]

//outputs
#define idsOut      plhs[0]

void fillPlist(mxArray* ids, KdtPointList& pl, uint i, uint k, 
               mxClassID retId)
{
  //sort the ids and eliminate duplicates
  sort(pl.begin(), pl.end());
  KdtPointIt pit = unique(pl.begin(), pl.end());
  size_t size = pit - pl.begin();    
  //randomize if k!=0
  if (k != 0)
    random_shuffle(pl.begin(), pit);
  //resize
  pl.resize(size);
//    cout << "size = " << size << endl;

  //get pointer
  uint* idsp;
  if (k>0)     
  {
    //get pointer
    idsp = (uint*) mxGetPr(ids);
    idsp += k*i;
  }
  else            
  {
    //get cell array
    mxArray* pcell = mxCreateNumericMatrix(1, pl.size(), retId, mxREAL);
    mxSetCell(ids, i, pcell);              
    //allocate
    idsp = (uint*) mxGetPr(pcell);
  }
    
  //get data from this list
  for (uint j=0; (j<k || k==0) && j<pl.size(); ++j)
  {
    //add 1 to output to make it compatible with matlab
    //indices of ZERO are unused
    idsp[j] = pl[j] + 1;
//      cout << (pls[i])[j] << endl;
  }
    
  //clear this list
  pl.clear();  
}

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs<3)	mexErrMsgTxt("Three inputs required");
  
  //get the lsh objcect
  Kdf* kdf = *(Kdf**)mxGetData(kdfIn);
  
  //get the class of points
  mxClassID classID = getClassID(pointsIn); 

  //get number of points in search data
  mwSize npoints = getNpoints(pointsIn);

  //number of nearest neighbors to return 
  uint k = (uint) *mxGetPr(kIn);
  
  //return all points -> return a cell array, otherwise returna matrix kxnpoints
  mxClassID retId;
  switch (numeric_limits<uint>::digits)
  {
    case 32: retId = mxUINT32_CLASS; break;
    case 64: retId = mxUINT64_CLASS; break;
  }
  
  //  mexPrintf("sizes are: %dx%d %dx%d\n", rows1, cols1, rows2, cols2);
  uint n1;
  #define __CASE(CLASS)                                                       \
  {                                                                           \
    /*datas*/                                                                 \
    Data<TYPEOF_##CLASS> points;                                              \
    fillData(points, pointsIn);                                               \
    /*allocate output*/                                                       \
    n1 = points.size();                                                       \
    /*return cell array*/                                                     \
    if (k == 0)                                                               \
      idsOut = mxCreateCellMatrix(1, n1);                                     \
    /*return array*/                                                          \
    else if (k > 0)                                                           \
      idsOut = mxCreateNumericMatrix((mwSize)k, (mwSize)n1, retId, mxREAL);   \
    /*loop*/                                                                  \
    KdtPointList pl;                                                          \
    for (uint i=0; i<n1; ++i)                                                 \
    {                                                                         \
      /*get ids*/                                                             \
      getIds(*kdf, points, i, pl);                                            \
      /*put back*/                                                            \
      fillPlist(idsOut, pl, i, k, retId);                                     \
    }                                                                         \
    /*clear memory*/                                                          \
    points.clear();                                                           \
  }

  __SWITCH(classID, __CASE)
 
}
