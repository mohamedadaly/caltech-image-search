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
#define bucketsIn   prhs[1]
#define tableIn     prhs[2]
#define nretIn      prhs[3]

//outputs
#define idsOut      plhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=4)	mexErrMsgTxt("Four inputs required");
  
  //get the lsh objcect
  Lsh* lsh = *(Lsh**)mxGetData(lshIn);
  
  //table
  int table = -1;
  // if passed in the table, get it
  if (mxGetPr(tableIn))
    table = (int) *mxGetPr(tableIn);

  //get number of buckets
  mwSize npoints, ntables;
  if (table!=-1)
  {
    // one table
    npoints = mxGetNumberOfElements(bucketsIn);
    ntables = 1;
  }
  else
  {
    //all tables, so mxn is ntablesxnpoints
    npoints = mxGetN(bucketsIn);
    ntables = mxGetM(bucketsIn);
    if (ntables != lsh->opt.ntables)
      mexErrMsgTxt("Using all tables, but no of rows != lsh.ntables");
  }
//   cout << "table="<< table << endl;
//   cout << "ntables="<< ntables << endl;
//   cout << "npoints="<< npoints << endl;

  //number of returns
  mwSize nret = (mwSize) *mxGetPr(nretIn);
  
  //create an array of LshPointList
  LshPointList* pls = new LshPointList[npoints];
  
  //create list of buckets
  vector<BucketIdList> bids(ntables);
  BucketId_t* bidp = (BucketId_t*) mxGetPr(bucketsIn);
  for (int t=0; t<ntables; ++t)
  {
    bids[t].reserve(npoints);
    for (uint i=0; i<npoints; ++i) bids[t].push_back(bidp[i*ntables + t]);
  }
  
  //get the point lists
  if (table != -1)
  {
    // we have a table number, so use it
    getBucketPoints(*lsh, bids[0], (uint)table, pls);
  }
  else
  {
    // assume that the bucket ids correspond to the number of tables
    for (int t = 0; t < ntables; ++t)
      getBucketPoints(*lsh, bids[t], (uint)t, pls);
  }    
  
  //clear list
  bids.clear();
  
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

