// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#ifndef MX_HKMEANS_HPP
#define MX_HKMEANS_HPP

#include "mxData.hpp"


//-----------------------------------------------------------------------
// import and export HkmPointList to Matlab
mxArray* exportHkmPointList(HkmPointList &pl)
{
  mxArray* plist = mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
  //size
  uint npoints = pl.size();
  if (npoints > 0)
  {
    //create mxarary
    plist = mxCreateNumericMatrix(1, npoints, getClassIDfromT(pl[0]), mxREAL);
    //get pointer
    HkmPoint* pp = (HkmPoint*) mxGetPr(plist);
    //loop
    for (uint j=0; j<npoints; ++j)	pp[j] = pl[j];
  }
 return plist;
}

void importHkmPointList(HkmPointList &pl, const mxArray* plist)
{
  //get number of points
  uint npoints;
  if (plist)
  {
  	npoints = mxGetNumberOfElements(plist);
    //resize pl
    pl.resize(npoints);

    //pointer
    HkmPoint* pp = (HkmPoint*) mxGetPr(plist);
    //loop
    for (uint j=0; j<npoints; ++j) pl[j] = pp[j];
  }
}

//-----------------------------------------------------------------------
// import and export HkmClassIds to Matlab
mxArray* exportHkmClassIds(HkmClassIds &cids)
{
  mxArray* mxcids = mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
  //size
  uint npoints = cids.size();
  if (npoints > 0)
  {
    //create mxarary
    mxcids = mxCreateNumericMatrix(1, npoints, getClassIDfromT(cids[0]), mxREAL);
    //get pointer
    HkmClassId* pp = (HkmClassId*) mxGetPr(mxcids);
    //loop
    for (uint j=0; j<npoints; ++j)	pp[j] = cids[j];
  }
 return mxcids;
}

void importHkmClassIds(HkmClassIds &cids, const mxArray* mxcids)
{
  if (mxcids)
  {
    //get number of points
    uint npoints = mxGetNumberOfElements(mxcids);
    //resize pl
    cids.resize(npoints);

    //pointer
    HkmClassId* pp = (HkmClassId*) mxGetPr(mxcids);
    //loop
    for (uint j=0; j<npoints; ++j) cids[j] = pp[j];
  }
}

//-----------------------------------------------------------------------
// import and export MeanStds to Matlab
mxArray* exportMeanStds(MeanStds &ms)
{
  mxArray* mxms = mxCreateNumericMatrix(1, 0, mxDOUBLE_CLASS, mxREAL);
  //size
  uint npoints = ms.size();
  if (npoints > 0)
  {
    //create mxarary
    mxms = mxCreateNumericMatrix(1, npoints, getClassIDfromT(ms[0]), mxREAL);
    //get pointer
    MeanStd* pp = (MeanStd*) mxGetPr(mxms);
    //loop
    for (uint j=0; j<npoints; ++j)	pp[j] = ms[j];
  }
 return mxms;
}

void importMeanStds(MeanStds &ms, const mxArray* mxms)
{
  if (mxms)
  {
    //get number of points
    uint npoints = mxGetNumberOfElements(mxms);
    //resize pl
    ms.resize(npoints);

    //pointer
    MeanStd* pp = (MeanStd*) mxGetPr(mxms);
    //loop
    for (uint j=0; j<npoints; ++j) ms[j] = pp[j];
  }
}

//-----------------------------------------------------------------------
// import and export options struct to matlab
mxArray* exportHkmOptions(HkmOptions &opt)
{
  //get its size
  mwSize sz = sizeof(opt);
  //allocate  
  mxArray* mxopt = mxCreateNumericMatrix(1, sz, mxUINT8_CLASS, mxREAL);
  //get pointers
  uint8_t *pmxopt = (uint8_t*) mxGetData(mxopt);
  uint8_t *popt = (uint8_t*) &opt;
  //put data
  copy(popt, popt+sz, pmxopt);
  
//  for (uint i=0; i<sz; ++i) cout << " | " << (int)popt[i] << "==" << (int)pmxopt[i];
//  cout << endl;
//  cout << opt.niters << " " << opt.nbranches << " " << opt.nlevels << 
//          " " << opt.dist << " " << opt.usekdt << " " << opt.kdtopt.ntrees <<
//          " " << opt.kdtopt.varrange << " " << opt.kdtopt.cycle << 
//          " " << opt.kdtopt.dist << " " << opt.kdtopt.maxbins << 
//          " " << opt.kdtopt.sample << endl;
  
  //return
  return mxopt;
}

void importHkmOptions(HkmOptions &opt, const mxArray* mxopt)
{
  //get its size
  mwSize sz = sizeof(opt);
  //get pointers
  uint8_t *pmxopt = (uint8_t*) mxGetData(mxopt);
  uint8_t *popt = (uint8_t*) &opt;
  //put data
  copy(pmxopt, pmxopt+sz, popt);

//  cout << opt.niters << " " << opt.nbranches << " " << opt.nlevels << 
//          " " << opt.dist << " " << opt.usekdt << " " << opt.kdtopt.ntrees <<
//          " " << opt.kdtopt.varrange << " " << opt.kdtopt.cycle << 
//          " " << opt.kdtopt.dist << " " << opt.kdtopt.maxbins << 
//          " " << opt.kdtopt.sample << endl;
//  for (uint i=0; i<sz; ++i) cout << " | " << (int)popt[i] << "==" << (int)pmxopt[i];
//  cout << endl;
  
}

//-----------------------------------------------------------------------
// Exports to matlab structure
template <class T>
mxArray* exportMatlab(Hkms<T>& hkms, bool getids)
{
  //size of array
  uint nnodes = hkms.hkms[0].size();
  uint ntrees = hkms.opt.ntrees;
  
  //The output is a structure array: nodes, opt, class
  int nfields1 = 3;
  const char* fieldnames1[] = {"nodes", "opt", "class"}; 
  mxArray* out = mxCreateStructMatrix(1, 1, nfields1, fieldnames1);
  
  //make the nodes matrix: ntables X nnodes
  int nfields2 = 5;
  const char* fieldnames2[] = {"means", "plist", "classids", 
    "visited", "meanstd"};  
  mxArray* nodes = mxCreateStructMatrix(ntrees, nnodes, nfields2, fieldnames2);
  mxSetField(out, 0, "nodes", nodes);
  
  //add the options to the first one
  mxSetField(out, 0, "opt", exportHkmOptions(hkms.opt));    
  
  //add class
  T t;
  mxSetField(out, 0, "class", mxCreateDoubleScalar((double)getClassIDfromT(t)));
   
//  cout << "nnodes:" << nnodes << endl;
  mxArray* a;
  
  //now loop and build nodes
  for (uint t=0; t<ntrees; ++t)
  {
    //get that tree
    Hkm<T>& hkm = hkms.hkms[t];
    
    for (uint i=0; i<nnodes; ++i)
    {
  //    cout << "node:" << i+1 << endl;
      //rows for trees, columns for nodes within the tree
      //so index is i*ntrees (to get the right column) + t (row)
      mwSize nid = i*ntrees+t;

      //get the node
      HkmNode<T> &node = hkm.at(i+1);

      //get the data into an mxarray
      mxSetField(nodes, nid, "means", fillMxArray(node.means));

      //visited
      a = mxCreateNumericMatrix(1, 1, mxUINT8_CLASS, mxREAL);
      *((uint8_t*)mxGetData(a)) = (uint8_t) node.visited;
      mxSetField(nodes, nid, "visited", a); 

      //meanStds
      mxSetField(nodes, nid, "meanstd", exportMeanStds(node.meanStds)); 
      
      //get the lists
      if (getids && node.size()>0)
      {      
        //plist
  //      cout << "plist" << endl;
        mxSetField(nodes, nid, "plist", exportHkmPointList(node.plist));
        //classids
  //      cout << "classids" << endl;
        mxSetField(nodes, nid, "classids", exportHkmClassIds(node.classIds)); 
      }    
    }
  }
  
  return out;
}
        
//-----------------------------------------------------------------------
// Imports from matlab structure exported using exportMatlab
template <class T>
Hkms<T>* importMatlab(const mxArray* in, T dummy)
{

  //get the options
  HkmOptions opt;
  importHkmOptions(opt, mxGetField(in, 0, "opt"));
  
  //allocate
  Hkms<T>* hkms = new Hkms<T>(opt);
  
  
  //get the nodes
  mxArray* nodes = mxGetField(in, 0, "nodes");

  //size of array
//   uint nnodes = mxGetNumberOfElements(nodes);
  uint nnodes = mxGetN(nodes);
  uint ntrees = mxGetM(nodes);
    
  //resize hkms
  hkms->resize(ntrees);
  
  //loop on trees
  for (uint t=0; t<ntrees; ++t)
  {
    //get hkm
    Hkm<T>& hkm = hkms->hkms[t];
    //resize
    hkm.resize(nnodes);
    //set opt
    hkm.opt = opt;
//     cout << "opt.nlevels:" << hkm.opt.nlevels << ", orig:" << opt.nlevels << endl;

    mxArray* a;

    //now loop and build
    for (uint i=0; i<nnodes; ++i)
    {
      //node id
      mwSize nid = i*ntrees + t;

      //get the node
      HkmNode<T> &node = hkm.at(i+1);

      //get the means
      fillData(node.means, mxGetField(nodes, nid, "means"), true);
      
      //mean stds
      importMeanStds(node.meanStds, mxGetField(nodes, nid, "meanstd"));

      //get the lists
      importHkmPointList(node.plist, mxGetField(nodes, nid, "plist"));    
      importHkmClassIds(node.classIds, mxGetField(nodes, nid, "classids"));

      //visited
      node.visited = (bool) *((uint8_t*)mxGetData(mxGetField(nodes, nid, "visited")));
    }
  //  uint8_t* popt = (uint8_t*) &hkm->opt;
  //  cout << "hkmopt:" << endl;
  //  for (uint i=0; i<sizeof(opt); ++i) cout << " | " << (int)popt[i];
  //  cout << endl;
  //  popt = (uint8_t*) &opt;
  //  cout << "opt:" << endl;
  //  for (uint i=0; i<sizeof(opt); ++i) cout << " | " << (int)popt[i];
  //  cout << endl;
  }
  
  return hkms;
}

//-----------------------------------------------------------------------
// gets a matlab pointer fomr hkm
template <class T>
mxArray* exportMatlabPointer(Hkms<T>& hkm)
{
  //make a strucuture holding a pointer and the classId corresponding to the data
  int nfields = 2;
  const char* fieldnames[] = {"ptr", "class"};
  mxArray* out = mxCreateStructMatrix(1, 1, nfields, fieldnames);

  mxArray* a;
  
  //set the pointer
  a = mxCreateNumericMatrix(1,1,mxINDEX_CLASS,mxREAL);
  *(Hkms<T>**) mxGetPr(a) = &hkm;
  mxSetField(out, 0, "ptr", a);
  
  //set the class id
  T t;
//  mxClassID cid =  getClassIDfromT(t);
//  a = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
//  *mxGetPr(a) = (double) cid;
  mxSetField(out, 0, "class", mxCreateDoubleScalar((double)getClassIDfromT(t)));
  
  return out;
}

mxClassID getClassIdMatlabPointer(const mxArray* in)
{
  //get the classid
  mxClassID cid = (mxClassID) *(mxGetPr(mxGetField(in, 0, "class")));
          
  return cid;
}

template <class T>
Hkms<T>* getPointerMatlabPointer(const mxArray* in, T dummy)
{
  //get the pointer
  return *(Hkms<T>**)mxGetData(mxGetField(in, 0, "ptr"));
}

//-------------------------------------------------------------------------



//#define TEMPLATE(F)         \
//  F(float)                  \
//  F(double)                 \
//  F(char)                   \
//  F(int)                    \
//  F(unsigned int)           \
//  F(unsigned char)          
//  
//#define EXPORTMATLABPIONTER(T) \
//  template mxArray* exportMatlabPointer(Hkm<T>&);
//
//TEMPLATE(EXPORTMATLABPIONTER)


#endif
