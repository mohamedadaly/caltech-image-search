// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_COMMON_HPP
#define MX_COMMON_HPP

#include <typeinfo>

#include "ccCommon.hpp"

using namespace std;

//--------------------------------------------------------------------------
//Matlab types
#define TYPEOF_mxSINGLE_CLASS   float
#define TYPEOF_mxDOUBLE_CLASS   double
#define TYPEOF_mxINT8_CLASS     int8_t
#define TYPEOF_mxUINT8_CLASS    uint8_t
#define TYPEOF_mxINT32_CLASS    int32_t
#define TYPEOF_mxUINT32_CLASS   uint32_t
#define TYPEOF_mxINT64_CLASS    int64_t
#define TYPEOF_mxUINT64_CLASS   uint64_t


//creates a switch statement with fun in every statement using "var"
//as the class ID. "fun" is another macro that takes the class ID.
#define __SWITCH(var,fun)                               \
  switch(var)                                           \
  {                                                     \
    case mxSINGLE_CLASS:  fun(mxSINGLE_CLASS)  break;   \
    case mxDOUBLE_CLASS:  fun(mxDOUBLE_CLASS)  break;   \
    case mxINT8_CLASS:    fun(mxINT8_CLASS)    break;   \
    case mxUINT8_CLASS:   fun(mxUINT8_CLASS)   break;   \
    case mxINT32_CLASS:   fun(mxINT32_CLASS)   break;   \
    case mxUINT32_CLASS:  fun(mxUINT32_CLASS)  break;   \
    case mxINT64_CLASS:   fun(mxINT64_CLASS)   break;   \
    case mxUINT64_CLASS:  fun(mxUINT64_CLASS)  break;   \
  }                                                     

//creates a switch statement with fun in every statement using "var"
//as the class ID. "fun" is another macro that takes the class ID.
//in1 is the first input to fun, and the switch works on the second one
#define __SWITCH2(var,fun,in1)                          \
  switch(var)                                           \
  {                                                     \
    case mxSINGLE_CLASS:  fun(in1,mxSINGLE_CLASS)  break;   \
    case mxDOUBLE_CLASS:  fun(in1,mxDOUBLE_CLASS)  break;   \
    case mxINT8_CLASS:    fun(in1,mxINT8_CLASS)    break;   \
    case mxUINT8_CLASS:   fun(in1,mxUINT8_CLASS)   break;   \
    case mxINT32_CLASS:   fun(in1,mxINT32_CLASS)   break;   \
    case mxUINT32_CLASS:  fun(in1,mxUINT32_CLASS)  break;   \
    case mxINT64_CLASS:   fun(in1,mxINT64_CLASS)   break;   \
    case mxUINT64_CLASS:  fun(in1,mxUINT64_CLASS)  break;   \
  }       
//--------------------------------------------------------------------------
// gets the classId from the input type
template <class T>
mxClassID getClassIDfromT(T t)
{
  mxClassID cid = mxUNKNOWN_CLASS;
  
  if (typeid(T)==typeid(uint8_t))
    cid = mxUINT8_CLASS;
  else if (typeid(T)==typeid(int8_t))
    cid = mxINT8_CLASS;
  else if (typeid(T)==typeid(uint16_t))
    cid = mxUINT16_CLASS;
  else if (typeid(T)==typeid(int16_t))
    cid = mxINT16_CLASS;
  else if (typeid(T)==typeid(uint32_t))
    cid = mxUINT32_CLASS;
  else if (typeid(T)==typeid(int32_t))
    cid = mxINT32_CLASS;
  else if (typeid(T)==typeid(uint64_t))
    cid = mxUINT64_CLASS;
  else if (typeid(T)==typeid(int64_t))
    cid = mxINT64_CLASS;
  else if (typeid(T)==typeid(float))
    cid = mxSINGLE_CLASS;
  else if (typeid(T)==typeid(double))
    cid = mxDOUBLE_CLASS;
  
  return cid;
}

//--------------------------------------------------------------------------
// gets the class id of the input matrix/cell array
mxClassID getClassID(const mxArray* pointsIn)
{
  //get the class of points
  mxClassID classID = mxGetClassID(pointsIn);
  //if cell
  if (mxIsCell(pointsIn) && mxGetNumberOfElements(pointsIn)>0)
//     classID = mxGetClassID(mxGetCell(pointsIn, 0));
    for (uint i=0; i<mxGetNumberOfElements(pointsIn); ++i)
    {
      mxArray* el = mxGetCell(pointsIn, i);
      if (el != NULL)
      {
        classID = mxGetClassID(el);
        break;
      }
    }

  return classID;
}


  
#define GETCLASSIDFROMT(T) \
  template mxClassID getClassIDfromT(T);


TEMPLATE(GETCLASSIDFROMT)


#endif
