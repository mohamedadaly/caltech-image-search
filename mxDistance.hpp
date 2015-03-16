// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_DISTANCE_HPP
#define MX_DISTANCE_HPP


// Return the distance type from the input string
DistanceType getDistanceType(const mxArray* distIn)
{
  char tt[100];
  mxGetString(distIn, tt, 90);
  DistanceType dt;
  string str(tt);
  if (str=="l1")            dt = DISTANCE_L1;
  else if (str=="l2")       dt = DISTANCE_L2;
  else if (str=="hamming")  dt = DISTANCE_HAMMING;
  else if (str=="cos")      dt = DISTANCE_COS;
  else if (str=="arccos")   dt = DISTANCE_ARCCOS;
  else if (str=="bhat")     dt = DISTANCE_BHAT;
  else if (str=="kl")       dt = DISTANCE_KL;
  else if (str=="jac")      dt = DISTANCE_JAC;
  else if (str=="xor")      dt = DISTANCE_XOR;
  else mexErrMsgTxt("Unknown distance function.");
  
  //return
  return dt;
} 


#endif

