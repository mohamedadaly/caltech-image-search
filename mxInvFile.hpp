// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_INVFILE_HPP
#define MX_INVFILE_HPP


//return inverted file distance
ivFile::Dist getInvFileDist(const mxArray* distIn)
{
  ivFile::Dist dist;
  char tt[100]; 
  mxGetString(distIn, tt, 90);
  string str(tt);
  if (str == "l1")        dist = ivFile::DIST_L1;
  else if (str == "l2")   dist = ivFile::DIST_L2;
  else if (str == "ham")  dist = ivFile::DIST_HAM;
  else if (str == "kl")   dist = ivFile::DIST_KL;
  else if (str == "cos")  dist = ivFile::DIST_COS;
  else if (str == "jac")  dist = ivFile::DIST_JAC;
  else if (str == "hist-int")  dist = ivFile::DIST_HISTINT;
  else                    mexErrMsgTxt("Unknown distance function");
  
  return dist;
}

//return inverted file norm
ivFile::Norm getInvFileNorm(const mxArray* normIn)
{
  ivFile::Norm norm;
  char tt[100]; 
  mxGetString(normIn, tt, 90);
  string str(tt);
  if (str == "none")       norm = ivFile::NORM_NONE;
  else if (str == "l0")    norm = ivFile::NORM_L0;
  else if (str == "l1")    norm = ivFile::NORM_L1;
  else if (str == "l2")    norm = ivFile::NORM_L2;
  else                     mexErrMsgTxt("Unknown normalization function");

  return norm;
}


//return inverted file weight
ivFile::Weight getInvFileWeight(const mxArray* weightIn)
{
  char tt[100]; 
  mxGetString(weightIn, tt, 90);
  string str(tt);
  ivFile::Weight wt;
  if (str == "none")        wt = ivFile::WEIGHT_NONE;
  else if (str == "bin")    wt = ivFile::WEIGHT_BIN;
  else if (str == "tf")     wt = ivFile::WEIGHT_TF;
  else if (str == "tfidf")  wt = ivFile::WEIGHT_TFIDF;
  else                      mexErrMsgTxt("Unknown weighting function");

  return wt;
}

#endif
