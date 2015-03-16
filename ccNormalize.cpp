// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010


#include "ccNormalize.hpp"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>

#include "ccMatrix.hpp"
#include "ccData.hpp"
#include "ccCommon.hpp"

//
// data     - pointer to data, with one column per point i.e. data is 
//            ndimsXnpoints aray
// ndims    - number of rows (dimensions)
// npoints  - number of cols (points)
// metric   - the normalization metric to use
//
template<class T>
void normalize(Data<T>& data, NormalizeMetric metric)
{
  uint col, row;
  uint npoints = data.size();
  
  if (metric == NORMALIZE_METRIC_NONE) return;
  bool sparse = data.isSparse();
  pair<T*, uint> p;
  
  //loop on points
  for (col=0; col<npoints; ++col)
  {
    //get the point if not sparse    
    uint ndims;
    if (!sparse)
    {
      p = data.getPoint(col);
      //get dimensions
      ndims = p.second;
    }
    else
      ndims = data.getSpPointDim(col);
    
    //compute norm
    float val = 0, t;
    switch (metric)
    {
      case NORMALIZE_METRIC_L1:
        //get norm
        if (!sparse)
          for (row=0; row<p.second; ++row)
            val += (float) fabs((float) p.first[row]);
        else
          for (uint r=0; r<ndims; ++r)
          {
            T v;
            data.getSpPointVal(col, r, v, row);
            val += (float)fabs((float)v);
          }
//         for (row=0; row<data.ndims; ++row)
// //           val += (float) fabs((float) p.first[row]);   //_MAT_ELEM(data,row,col,ndims));
//           val += (float) fabs((float) data.getPointVal(col, row));
        break;
      
      case NORMALIZE_METRIC_L2:
        //get norm
        if (!sparse)
          for (row=0; row<p.second; ++row)
          {
            t = (float) p.first[row];
            val += t*t;
          }
        else
          for (uint r=0; r<ndims; ++r)
          {
            T v;
            data.getSpPointVal(col, r, v, row);
            val += (float)v * (float)v;
          }        
//         //get norm
//         for (row=0; row<data.ndims; ++row)
//         {
// //           t = (float)p.first[row];   //_MAT_ELEM(data,row,col,ndims);
//           t = (float)data.getPointVal(col, row); 
//           val += t*t;
//         }
        val = sqrt(val);  
        break;       
       
    }
    
    //normalize
    if (!sparse)
      for (row=0; row<p.second; ++row)
        p.first[row] = (T) ((float)p.first[row] / val);
    else
      for (uint r=0; r<ndims; ++r)
      {
        T v;
        data.getSpPointVal(col, r, v, row);
        data.setSpPointVal(col, r, (T) ((float)v/val) );
      }    
//     for (row=0; row<data.ndims; ++row)
//       data.setPointVal(col, row, (T) (data.getPointVal(col,row) / val));
//       p.first[row] = (T) ((float) p.first[row] / val);      
//       _MAT_ELEM(data,row,col,ndims) = (T) 
//         ((float)_MAT_ELEM(data,row,col,ndims) / val);
    
  } //for i
}


template<class T, class Tret>
void norm(Data<T>& data, NormalizeMetric metric, Vector<Tret>& ret)
{
  uint col, row;
  uint npoints = data.size();
  bool sparse = data.isSparse();
  pair<T*, uint> p;
  
  if (metric == NORMALIZE_METRIC_NONE) return;
  
  //loop on points
  for (col=0; col<npoints; ++col)
  {
    
    //get the point if not sparse    
    uint ndims;
    if (!sparse)
    {
      p = data.getPoint(col);
      //get dimensions
      ndims = p.second;
    }
    else
      ndims = data.getSpPointDim(col);
    
    //compute norm
    Tret val = 0, t;
    switch (metric)
    {
      case NORMALIZE_METRIC_L1:
        //get norm
        if (!sparse)
          for (row=0; row<p.second; ++row)
            val += (Tret) fabs((Tret) p.first[row]);
        else
          for (uint r=0; r<ndims; ++r)
          {
            T v;
            data.getSpPointVal(col, r, v, row);
            val += (Tret)fabs((Tret)v);
          }
//         //get norm
//         for (row=0; row<data.ndims; ++row)
//           val += (Tret) fabs((Tret) data.getPointVal(col, row));   //_MAT_ELEM(data,row,col,ndims));
//           val += (Tret) fabs((Tret) p.first[row]);   //_MAT_ELEM(data,row,col,ndims));
        break;
      
      case NORMALIZE_METRIC_L2:
        if (!sparse)
          for (row=0; row<p.second; ++row)
          {
            t = (Tret) p.first[row];
            val += t*t;
          }
        else
          for (uint r=0; r<ndims; ++r)
          {
            T v;
            data.getSpPointVal(col, r, v, row);
            val += (Tret)v * (Tret)v;
          }           
        //get norm
//         for (row=0; row<data.ndims; ++row)
//         {
//           t = (Tret)data.getPointVal(col, row);   //_MAT_ELEM(data,row,col,ndims);
//           val += t*t;
//         }
        val = sqrt(val);  
        break;       
       
    }
    
    //put norm
    ret[col] = val;   
    
  } //for i
}

// 
// #define TEMPLATE(F)         \
//   F(float)                  \
//   F(double)                 \
//   F(char)                   \
//   F(int)                    \
//   F(unsigned int)           \
//   F(unsigned char)          
//   
  
#define NORMALIZE(T) \
  template void normalize<T>(Data<T>&, NormalizeMetric);


#define NORM_F(T) \
  template void norm(Data<T>&, NormalizeMetric, Vector<float>&);
#define NORM_D(T) \
  template void norm(Data<T>&, NormalizeMetric, Vector<double>&);

TEMPLATE(NORMALIZE)
TEMPLATE(NORM_F)
TEMPLATE(NORM_D)


