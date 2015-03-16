// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MXLSH_HPP_
#define MXLSH_HPP_

#include "ccData.hpp"

template <typename T>
void  fillData(Data<T>& data, const mxArray* points)
{
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
      mxArray* p = mxGetCell(points, i);
      data.setVarPoint((T*)mxGetData(p), (uint)mxGetNumberOfElements(p), i);
    }    
  }
  else
  {
    //set points and dimensions
    data.type = DATA_FIXED;
    data.npoints = (uint)mxGetN(points);
    data.ndims = (uint)mxGetM(points);
    /*set pointer*/
    data.setFixed((T*)mxGetData(points), false);
  }
  
//  cout << "points = " << data.npoints << endl;
}


#define TEMPLATE(F)         \
  F(float)                  \
  F(double)                 \
  F(char)                   \
  F(int)                    \
  F(unsigned int)           \
  F(unsigned char)          
  
#define FILLDATA(T) \
  template void fillData(Data<T>&, const mxArray*);

TEMPLATE(FILLDATA)


#endif

