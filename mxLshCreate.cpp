// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>

#include "ccLsh.hpp"
#include "mxDistance.hpp"

                                                

//inputs
#define ntablesIn   prhs[0]
#define nfuncsIn    prhs[1]
#define htypeIn     prhs[2]
#define distIn      prhs[3]
#define normIn      prhs[4]
#define ndimsIn     prhs[5]
#define wIn         prhs[6]
#define nbitsIn     prhs[7]
#define hwidthIn    prhs[8]
#define tablesizeIn prhs[9]
#define strIn       prhs[10]
#define forestIn    prhs[11]
#define seedIn      prhs[12]
#define bitsperdimIn prhs[13]
#define verboseIn   prhs[14]


//outputs
#define lshOut      plhs[0]

string getStr(const mxArray* a)
{
  char s[500];
  mxGetString(a, s, 500);
  string str = s;
  return str;  
}

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs<4)	mexErrMsgTxt("Four inputs required at least");

  //create the options
  LshOptions opt;
  //get input options
  if (nrhs>0) opt.ntables = (int) *mxGetPr(ntablesIn);
  if (nrhs>1) opt.nfuncs = (int) *mxGetPr(nfuncsIn);
  if (nrhs>2)
  {
    string str = getStr(htypeIn);
    if (str == "ham")           opt.htype = LSH_HASH_TYPE_HAM;
    else if (str == "l1")       opt.htype = LSH_HASH_TYPE_L1;
    else if (str == "l2")       opt.htype = LSH_HASH_TYPE_L2;
    else if (str == "cos")      opt.htype = LSH_HASH_TYPE_COS;
    else if (str == "min-hash") opt.htype = LSH_HASH_TYPE_MHASH;
    else if (str == "sph-sim")  opt.htype = LSH_HASH_TYPE_SPH_SIMPLEX;
    else if (str == "sph-orth") opt.htype = LSH_HASH_TYPE_SPH_ORTHOPLEX;
    else if (str == "sph-hyp")  opt.htype = LSH_HASH_TYPE_SPH_HYPERCUBE;
    else if (str == "bin-gauss")  opt.htype = LSH_HASH_TYPE_BIN_GAUSS;
    else                     mexErrMsgTxt("Unknown hash function");
  }
  if (nrhs>3)
  {
    opt.dist = getDistanceType(distIn);
//    string str = getStr(distIn);
//    if (str == "ham")        opt.dist = LSH_DIST_HAM;
//    else if (str == "l1")    opt.dist = LSH_DIST_L1;
//    else if (str == "l2")    opt.dist = LSH_DIST_L2;
//    else if (str == "cos")   opt.dist = LSH_DIST_COS;
//    else if (str == "jac")   opt.dist = LSH_DIST_JAC;
//    else                     mexErrMsgTxt("Unknown distance function");
  }
  if (nrhs>4) opt.norm = (bool) *mxGetPr(normIn);
  if (nrhs>5) opt.ndims = (uint) *mxGetPr(ndimsIn);
  if (nrhs>6) opt.w = (float) *mxGetPr(wIn);
  if (nrhs>7) opt.nbits = (int) *mxGetPr(nbitsIn);
  if (nrhs>8) opt.hwidth = (int) *mxGetPr(hwidthIn);
  if (nrhs>9) opt.tablesize = (uint) *mxGetPr(tablesizeIn);
  if (nrhs>10) opt.str = (bool) *mxGetPr(strIn);
  if (nrhs>11) opt.forest = (bool) *mxGetPr(forestIn);
  if (nrhs>12) opt.seed = (uint) *mxGetPr(seedIn);
  if (nrhs>13) opt.bitsperdim = (int) *mxGetPr(bitsperdimIn);
  if (nrhs>14) opt.verbose = (bool) *mxGetPr(verboseIn);
  
  //create the Lsh object
  Lsh* lsh = new Lsh(opt);
  
  //create the functions
  lsh->createFuncs();
  
  //return pointer
  mxArray* ret = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
  *(Lsh**) mxGetPr(ret) = lsh;
  lshOut = ret;  
}

