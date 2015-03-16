// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

//#include <stdlib.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "mex.h"

#include "ccHKmeans.hpp"
#include "mxHKmeans.hpp"
//#include "mxData.hpp"
#include "mxDistance.hpp"

#define dataIn      prhs[0]
#define nitersIn    prhs[1]
#define nlevelsIn   prhs[2]
#define nbranchesIn prhs[3]
#define distIn      prhs[4]
#define ntreesIn    prhs[5]
#define nchecksIn   prhs[6]
#define usekdtIn    prhs[7]

#define nkdtreesIn  prhs[8]
#define varrangeIn  prhs[9]
#define meanrangeIn prhs[10]
#define cycleIn     prhs[11]
#define maxbinsIn   prhs[12]
#define sampleIn    prhs[13]

#define hkmOut      plhs[0]

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
//  srand(0xffffff);
      
  //options struct
  HkmOptions opt; 
  
  //get input class
  mxClassID classID = getClassID(dataIn);

  //get the inputs
  if (nrhs<1)
    mexErrMsgTxt("At least one input is required");  
  if (nrhs>1) opt.niters = (int) *(mxGetPr(nitersIn));
  if (nrhs>2) opt.nlevels = (int) *(mxGetPr(nlevelsIn));
  if (nrhs>3) opt.nbranches = (int) *(mxGetPr(nbranchesIn));
  if (nrhs>4) opt.dist = getDistanceType(distIn);
  if (nrhs>5) opt.ntrees = (int) *(mxGetPr(ntreesIn));
  if (nrhs>6) opt.nchecks = (int) *(mxGetPr(nchecksIn));
  if (nrhs>7) opt.usekdt = (bool) *(mxGetPr(usekdtIn));
  
  if (nrhs>8) opt.kdtopt.ntrees = (int) *(mxGetPr(nkdtreesIn));
  if (nrhs>9) opt.kdtopt.varrange= (float) *(mxGetPr(varrangeIn));
  if (nrhs>10) opt.kdtopt.meanrange = (float) *(mxGetPr(meanrangeIn));
  if (nrhs>11) opt.kdtopt.cycle = (char) *(mxGetPr(cycleIn));
  if (nrhs>12) opt.kdtopt.maxbins = (int) *(mxGetPr(maxbinsIn));
  if (nrhs>13) opt.kdtopt.sample = (int) *(mxGetPr(sampleIn));  

  //set distance
  opt.kdtopt.dist = opt.dist;

//  //return output
//  mxArray* ret;
  
  //fill with the data
#define __CASE(CLASS)                                                     \
  {                                                                       \
    /*get the data*/                                                      \
    Data<TYPEOF_##CLASS> data;                                            \
  	/*allocate the kdforest*/                                             \
    Hkms<TYPEOF_##CLASS>* hkm = new Hkms<TYPEOF_##CLASS>(opt);            \
    /*set data*/                                                          \
    fillData(data, dataIn, false);                                        \
    /*create*/                                                            \
    hkm->create(data);                                                    \
    /*clear data*/                                                        \
    data.clear();                                                         \
    /*put output*/                                                        \
    hkmOut = exportMatlabPointer(*hkm);                                   \
  }

  __SWITCH(classID, __CASE)
    
  
//  hkmOut = ret;
//  cout << "here" << endl;
}
