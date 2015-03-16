// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_INVFILEEXTRA_HPP
#define MX_INVFILEEXTRA_HPP

#include "mxInvFile.hpp"


//return inverted file feature matching type
FeatOpt::Type getFeatOptType(const mxArray* ftypeIn)
{
  char tt[100]; 
  mxGetString(ftypeIn, tt, 500);
  string str(tt);
  FeatOpt::Type ftype;
  if (str == "bin")        ftype = FeatOpt::VAL_BIN;
  else if (str == "exp")   ftype = FeatOpt::VAL_EXP;
  else if (str == "inv")   ftype = FeatOpt::VAL_INV;
  else                    mexErrMsgTxt("Unknown FeatOpt type");
  
  return ftype;
}

#endif

