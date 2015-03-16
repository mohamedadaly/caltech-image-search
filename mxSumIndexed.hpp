// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MXSUMINDEX_H_HPP_
#define MXSUMINDEX_H_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <deque>

#include "ccCommon.hpp"
#include "ccData.hpp"

// It sums up values from in and puts them into res. res has k elements, and 
// they are indexed by ids
// res is already allocated matrix
// ids are in matlab mode, so start at 1
//
template<class T>
void sumIndexed(Data<float>& res, uint* hist, Data<T>& in, uint* ids, uint k)
{
  //loop on inputs in in
  uint size = in.size();
  
  for (uint i=0; i<size; ++i)
  {
    //get in point
    pair<T*, uint> pin = in.getPoint(i);
    
    //get res point
    if (ids[i]<1) continue;
    pair<float*, uint> pres = res.getPoint(ids[i]-1);
    //update hist
    hist[ids[i]-1]++;
    //loop on dimensions
    for (uint d=0; d<pin.second; d++)
      pres.first[d] += (float)pin.first[d];
  }
}

#define SUMINDEXED(T) \
  template void sumIndexed(Data<float>&, uint*, Data<T>&, uint*, uint);

TEMPLATE(SUMINDEXED)

#endif

