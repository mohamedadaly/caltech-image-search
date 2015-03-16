// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_DATA_HPP_
#define MX_DTTA_HPP_

#include <typeinfo>

#include "mxMatrix.hpp"
#include "ccData.hpp"
#include "mxCommon.hpp"


//--------------------------------------------------------------------------
// gets the number of points in the matrix/cell array
mwSize getNpoints(const mxArray* points, bool col=false)
{
  mwSize npoints;
  if (points==NULL)
    npoints = 0;
  else if (mxIsCell(points))
  	npoints = mxGetNumberOfElements(points);
  else
  {
    if (col)
      npoints = mxGetN(points);
    else
      npoints = mxGetNumberOfElements(points);
  }
  
  return npoints;
}

//--------------------------------------------------------------------------
template<typename T>
void getVector(const mxArray* point, pair<T*, uint>& p, bool col=false)
 {
//   pair<T*, uint> p;
  if (point==NULL)
  {
    p.first = NULL;
    p.second = 0;
  }
  else
  {
    p.first = (T*)mxGetData(point);
    p.second = (uint)getNpoints(point, col);
  }
  
//   return p;
}

//--------------------------------------------------------------------------
// Fills the input data from the input matlab data matrix/cell array
template <typename T>
void  fillData(Data<T>& data, const mxArray* points, bool copy=false)
{
  uint npoints = getNpoints(points);
  //if a cell?
  if (mxIsCell(points))
  {
    //set number of pioints
    data.type = DATA_VAR;
    data.npoints = (uint)mxGetNumberOfElements(points);
    data.allocate();
    /*set the data*/               
    for (uint i=0; i<data.npoints; ++i)
    {      
      pair<T*, uint> pv; getVector(mxGetCell(points, i), pv, false);
      data.setVarPoint(pv.first, pv.second, i, copy);
//       cout << "point:" << i << " has " << pv.second << endl;
    }    
  }
  else
  {
    //set points and dimensions
    data.type = DATA_FIXED;
    data.npoints = (uint)mxGetN(points);
    data.ndims = (uint)mxGetM(points);
    //set matrix
    fillMatrix(data.data.fixed, points, copy);
//     /*set pointer*/    
//     pair<T*, uint> pv; getVector(points, pv, true);
//     data.setFixed(pv.first, copy);
  }
  
//  cout << "points = " << data.npoints << " dims=" << data.ndims << endl;
}



//--------------------------------------------------------------------------
// Fills a matlab matrix/cell from the input data
template <typename T>
mxArray* fillMxArray(Data<T> &data)
{
  //the return data
  mxArray* points;
  
  //get the classID
  T t;
  mxClassID cid = getClassIDfromT(t);
  
  //npoints
  uint npoints = data.size();

//  cout << "fillMxArray: npoints:" << npoints << " ndims:" << data.ndims << endl;
//  //display first mean
//  cout << endl;
//  for (uint j=0; j<npoints; ++j)
//  {
//    cout << "dpoint: " << j << endl;
//    pair<T*, uint> pdata = data.getPoint(j);
//    for (uint i=0; i<pdata.second; ++i)
//      cout << " " << pdata.first[i]; //data.getPointVal(j, i); 
//    cout << endl;
//  }
  
  
  //create the mxarray
  if (data.type == DATA_FIXED)
    //make one big matrix
    points = mxCreateNumericMatrix(data.ndims, npoints, cid, mxREAL);      
  else      
    //create a cell matrix
    points = mxCreateCellMatrix(1, npoints);
  
  //loop on the points and copy data
  T* ppoints;
  for (uint i=0; i<npoints; ++i)
  {
    //get the data pointer
    pair<T*, uint> pdata = data.getPoint(i);
//    cout << " i=" << i << " has " << pdata.second << endl;
    
    //get the points pointer
    if (data.type == DATA_FIXED)
    {
      ppoints = ((T*) mxGetData(points)) + i*pdata.second;
    }
    else
    {
      //create the cell entry
      mxArray* pmx = mxCreateNumericMatrix(pdata.second, 1, cid, mxREAL);
      mxSetCell(points, i, pmx);
      //get the pointer
      ppoints = (T*) mxGetPr(pmx);
    }
    
    //copy
    copy(pdata.first, pdata.first + pdata.second, ppoints);
  }
 
//  ppoints = (T*) mxGetData(points);
//  cout << endl;
//  cout << "M:" << mxGetM(points) << " N:" << mxGetN(points) << endl;
//  for (uint i=0; i<npoints; ++i)
//  {
//    cout << "point: " << i << endl;
//    for (uint j=0; j<data.ndims; ++j)
//      cout << " " << ppoints[i*data.ndims+j]; 
//    
//  }
    
  return points;
}




//--------------------------------------------------------------------------

// #define TEMPLATE(F)         \
//   F(float)                  \
//   F(double)                 \
//   F(char)                   \
//   F(int)                    \
//   F(unsigned int)           \
//   F(unsigned char)          
  
// #define GETCLASSIDFROMT(T) \
//   template mxClassID getClassIDfromT(T);

#define FILLMXARRAY(T) \
  template mxArray* fillMxArray(Data<T>&);

#define FILLDATA(T) \
  template void fillData(Data<T>&, const mxArray*, bool);

TEMPLATE(FILLDATA)
TEMPLATE(FILLMXARRAY)

#undef FILLDATA
#undef FILLMXARRAY


#endif

