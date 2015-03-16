// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include "mex.h"

#include <string>
#include <cassert>
#include <cstring>
#include <climits>

#include "ccInvertedFileExtra.hpp"
#include "mxInvFileExtra.hpp"
#include "mxData.hpp"
#include "mxCommon.hpp"
#include "mxDistance.hpp"


//inputs
#define ivFileIn  prhs[0]
#define dataIn    prhs[1]
#define fddataIn  prhs[2]
#define fsdataIn  prhs[3]
#define wtIn      prhs[4]
#define normIn    prhs[5]
#define distIn    prhs[6]
#define fdistIn   prhs[7]
#define fthreshIn prhs[8]
#define ftypeIn   prhs[9]
#define nretIn    prhs[10]
#define overlapIn prhs[11]
#define verboseIn prhs[12]
//outputs          
#define docsOut   plhs[0]
#define scoresOut plhs[1]
          
         

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[])
{
   
  if (nrhs!=13)	mexErrMsgTxt("Thirteen inputs required");

  //get the class of points
  mxClassID classID = mxGetClassID(dataIn);
  //if cell
  if (mxIsCell(dataIn) && mxGetNumberOfElements(dataIn)>0)
    for (uint i=0; i<mxGetNumberOfElements(dataIn); ++i)
    {
      mxArray* el = mxGetCell(dataIn, i);
      if (el != NULL)
      {
        classID = mxGetClassID(el);
        break;
      }
    }
  //check class ID, only uint32 and int32 are acceptable now
  if (classID!=mxINT32_CLASS && classID!=mxUINT32_CLASS)
    mexErrMsgTxt("Accepts only int32 & uint32 for input data");  
  
  //get classId of feature data
  mxClassID fclassID1 = getClassID(fddataIn), fclassID2=getClassID(fsdataIn);
  if (fclassID1 != fclassID2) 
    mexErrMsgTxt("Features must be the same type");
  
  //number of documents
  uint ndocs = mxIsCell(dataIn) ? mxGetNumberOfElements(dataIn) : 1;
  
  //number of returns
  uint nret = (uint) *mxGetPr(nretIn);  
  
  //overlap only?
  bool overlapOnly = (bool) *mxGetPr(overlapIn);  
  
  //verbose?
  bool verbose = (bool) *mxGetPr(verboseIn);  

  //get the weighting to use
  ivFile::Weight wt = getInvFileWeight(wtIn);
  
  //get the normalization to use
  ivFile::Norm norm = getInvFileNorm(normIn);
  
  //get the distance to use
  ivFile::Dist dist = getInvFileDist(distIn);
  
  //Feature Options
  FeatOpt fopt;
  //feature distance
  fopt.dist = getDistanceType(fdistIn);   
  //feature type
  fopt.type = getFeatOptType(ftypeIn);
  //threshold
  fopt.threshold = (float) *mxGetPr(fthreshIn);

  //make the ivFile object  
  ivFile* ivfile = *(ivFile**)mxGetData(ivFileIn);
  //check if not passed in an object, then create a new one
  if (ivfile == NULL)
    mexErrMsgTxt("Empty inverted file input");
  
  //the return score list
  ivNodeLists scorelists;
  scorelists.resize(ndocs);
  
//   cout << "  searching!!";
//   mexPrintf("searching!!\n");

  //call function
#define __CASE(CLASS1, CLASS2)                                            \
    {                                                                     \
    /*get the data*/                                                      \
    Data<TYPEOF_##CLASS1> data;                                           \
    /*set data*/                                                          \
    fillData(data, dataIn);                                               \
    /*get the data*/                                                      \
    Data<TYPEOF_##CLASS2> fddata, fsdata;                                 \
    /*set data*/                                                          \
    fillData(fddata, fddataIn);                                           \
    fillData(fsdata, fsdataIn);                                           \
    /*insert*/                                                            \
    ivSearchFile(*ivfile, data, fddata, fsdata, fopt, wt, norm, dist,     \
            overlapOnly, nret, scorelists, verbose);                      \
    /*clear data*/                                                        \
    data.clear();                                                         \
    fddata.clear(); fsdata.clear();                                       \
    }
            
    switch(classID)
    {
      case mxINT32_CLASS:   __SWITCH2(fclassID1,__CASE,mxINT32_CLASS);   break;
      case mxUINT32_CLASS:  __SWITCH2(fclassID1,__CASE,mxUINT32_CLASS);  break;
    }

//     __SWITCH(classID, __CASE)
    
  mxArray *scores, *docs;
    
  //if we get all documents, then update nret with the number of docmuents
  //if it's zero
  if (!overlapOnly && nret==0)
    nret = ivfile->docs.size();
    
  //allocate output
  if (nret==0)
  {
    scores = mxCreateCellMatrix(1, ndocs);
    docs = mxCreateCellMatrix(1, ndocs);
  }
  else
  {
    scores = mxCreateNumericMatrix(nret, ndocs, mxSINGLE_CLASS,mxREAL);
    docs = mxCreateNumericMatrix(nret, ndocs, mxUINT32_CLASS,mxREAL);      
  }
  uint32_t* pdocs;
  float* pscores;
 
  //now loop on number of documents
//  cout << ndocs << " " << nret << endl;
  for (uint d=0; d<ndocs; ++d)
  {
    //get size of this list
    uint nlist = scorelists[d].size();
//    cout << " " << nlist;
    
    //get pointer to docs and score
    if (nret==0)
    {
      //create and get pointer
      mxSetCell(docs, d, mxCreateNumericMatrix(1, nlist, mxUINT32_CLASS, mxREAL));
      pdocs = (uint32_t*) mxGetPr(mxGetCell(docs, d));
      mxSetCell(scores, d, mxCreateNumericMatrix(1, nlist, mxSINGLE_CLASS, mxREAL));
      pscores = (float*) mxGetPr(mxGetCell(scores, d));
    }
    else
    {
      pdocs = (uint32_t*)mxGetPr(docs) + nret*d;
      pscores = (float*)mxGetPr(scores) + nret*d;      
    }
    
    //now we have pointers, so put data
//    nret = nret==0 ? nlist : nret;    
    for (uint i=0; i<nlist && (nret==0 || i<nret); ++i)
    {
      pscores[i] = (scorelists[d])[i].val;
      pdocs[i]   = (scorelists[d])[i].id + 1;      
    }
  }
   
  docsOut = docs;
  if (nlhs>1)
    scoresOut = scores;
  
  //clear 
  scorelists.clear();
}

