
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <iterator>

#include "ccCommon.hpp"

#include "ccInvertedFileExtra.hpp"



//struct for sorting values and getting indices
//first fill b with ordinal numbers, then sort b passing it a
// the result will be sorted a, with indices in b
// equivalent to [c, b] = sort(a);  i.e. c[0]=a[b[0]], ...
//
// vector<int> a;
// a.push_back(5); a.push_back(; a.push_back(7);
// 
// vector<size_t> b;
// for (unsigned i = 0; i < a.size(); ++i)
// b.push_back(i);
// // b = [0, 1, 2]
// sort(b.begin(), b.end(), index_cmp<vector<int>&>(a));
// // b = [0, 2, 1]
template<class T> 
struct index_cmp 
{
  //constructor
  index_cmp(const T iarr) : arr(iarr) {}
  //sort function: ascending
  bool operator()(const uint a, const uint b) const
  { return arr[a] < arr[b]; }
  //member array that we want to sort
  const T arr;
};

//------------------------------------------------------------------------
//# TODO
// template <class T>
// void ivFillFile(ivFile& ivf, T* wlabel, T* dlabel, uint ntokens, 
//         uint nwords, uint ndocs)
// {
//   //allocate vectors
//   ivf.words.resize(nwords);
//   ivf.docs.resize(ndocs);
//   
//   //now loop on the tokens and fill in the hdata
//   uint i, j, k;
//   for (i=0; i<ntokens; ++i)
//   {
//     //get the labels
//     if (wlabel[i]==0 || wlabel[i]>nwords || dlabel[i]==0 || dlabel[i]>ndocs)
//       continue;
//     uint wl = (uint)wlabel[i]-1;
//     uint dl = (uint)dlabel[i]-1;
// 
//     //get word
//     ivWord* w = &ivf.words[wl];        
//     
//     //increment word count
//     w->wf++;    
//     
//     //make a new wordDoc
//     ivWordDoc newdoc;
//     newdoc.doc = dl;
//     
//     //find position of this document id in the list
//     ivWordDocIt wdit = lower_bound (w->docs.begin(), w->docs.end(), newdoc); 
//     
//     //check if not found, then add it in the correct place
//     if (wdit == w->docs.end() || newdoc<(*wdit))
//     {
//       wdit = w->docs.insert(wdit, newdoc);
//       w->ndocs++;
//     }
//     
//     //increment word count for this word's doc
//     wdit->count++;
//     
//     //increment document count of tokens
//     ivf.docs[dl].ntokens++;
//   }
//   
//   //put sizes
//   ivf.nwords = nwords;
//   ivf.ndocs = ndocs;
// }

//fill the inverted file with input counts
//
// data     - the input data, with one data vector per input consisting of 
//            all the word labels for its tokens
// nwords   - the total number of words
template <class T>
void ivFillFile(ivFile& ivf, Data<T>& data, uint nwords, uint idshift)
{
  //number of documents
  uint ndocs = data.size();
  
  //allocate vectors
  ivf.words.resize(nwords);
  ivf.docs.resize(ndocs);
  
  //used for sorting words
  uint* sind = new uint[MAX_DOC_TOKENS];
  if (sind==NULL) cerr << "memory error" << endl;
  
  //loop on the documents and fill in
  for (uint d=0; d<ndocs; ++d)
  {
    //get the document data
    pair<T*,uint> doc = data.getPoint(d);
    //document label
    uint dl = d + idshift;
    
//     cout << "doc:" << d << " has words:" << doc.second << endl;
    //update ntokens for this document
    ivf.docs[dl].ntokens += doc.second;
    
    //sort tokens ascendingly, indices of results are in sind
    //tokens are untouched, i am merely sorting "sind"
    for (uint i=0; i<doc.second; ++i) sind[i] = i;
    sort(sind, sind+doc.second, index_cmp<T*>(doc.first));

    //prev word and its count
    uint prevword=0, prevwordc=0;
    //loop and update
    for (uint t=0; t<doc.second; ++t)
    {
      //get token id
      uint tid = sind[t]; //t; //sind[t];
      //word label (1-based i.e. first word is word 1, so convert to 0-based)
      if (doc.first[tid]==0 || doc.first[tid]>nwords)	continue;
      uint wl = (uint)(doc.first[tid]-1);
      
      //get word count for this word, if not like prev one
      if (doc.first[tid] != prevword)
      {
        //update nwords for this doc
        ivf.docs[dl].nwords++;
        //set
        prevword = doc.first[tid];
        //loop and get how many features have this word (doc.first[tid]) which
        //is 1-based, starting from next word
        prevwordc=1;
        for (uint w=t+1; w<doc.second; ++w)
          if (doc.first[sind[w]]==prevword) prevwordc++;
          else break;
      }
      
//       cout << "  word:" << wl << " count:" << prevwordc << endl;
        
      //get word
      ivWord& w = ivf.words[wl];

      //increment word count
      w.wf++;    

      //make a new wordDoc
      ivWordDoc newdoc;
      newdoc.doc = dl;
      //feature id, after sorting
      newdoc.feat = tid + ivf.ntokens; //t +
      //put val = wordcount
      newdoc.val = (float) prevwordc;
      
      //update word docs, check only last word doc, since docs are inserted 
      //in sequence
      if (dl != w.docs.back().doc)
        w.ndocs++;

      //insert word
//       cout << "doc:" << dl << " word:" << wl << " ndocs:" << w.docs.size() << endl;
      w.docs.push_back(newdoc);
      
//       //find position of this document id in the list
//       cout << "wordid" << wl << " size" <<  w->docs.size() << endl;
//       ivWordDocIt wdit = lower_bound (w.docs.begin(), w.docs.end(), newdoc); 
//       //insert in the correct place
//       w.docs.insert(wdit, newdoc);
//       //check if new?
//       if( newdoc<(*wdit) )
//         //increment ndocs for the word
//         w->ndocs++;
    }
    
    //ntokens for ivf, update here so that we use the correct previous value when
    //setting feat inside the wordDoc
    ivf.ntokens += doc.second;
    
  }
  
  //put sizes
  ivf.nwords = nwords;
  ivf.ndocs = ndocs;  
   
  //clear
  delete [] sind;
}


//------------------------------------------------------------------------
void ivFile::computeStats(ivFile::Weight wt, ivFile::Norm norm)
{
  uint i;

  //reset documents
  for (i=0; i<this->ndocs; ++i)
  {
//     this->docs[i].norml0 = 0;
//     this->docs[i].norml1 = 0;
//     this->docs[i].norml2 = 0;
    this->docs[i].norm = 0;
//     this->docs[i].nwords = 0;
//     this->docs[i].words.clear();
  }
  //now loop and compute word frequency for every document and norms
  for (i=0; i<this->nwords; ++i)
  {
    //get the word
    ivWord& w = this->words[i];
    
    //get number of documents for this word
    w.ndocs = w.docs.size();
    
    //loop on its documents
//     ivWordDocIt wdit;
    for (ivWordDocIt wdit=w.docs.begin(), wditend=w.docs.end(); wdit!=wditend; wdit++)
    {
      //get the word document id
      ivDoc& doc = this->docs[wdit->doc];
      
      //get the value and weight it
      float a = this->weightVal(1, w, doc, wt); //wdit->val
//       wdit->val = this->weightVal(1, w, doc, wt); //wdit->val
      
      //update norms
      switch(norm)
      {
        case NORM_L0: doc.norm = doc.nwords;              break; //doc.norm += 1
        case NORM_L1: doc.norm += a;                      break; //wdit->val
        case NORM_L2: doc.norm += a * a * wdit->val;      break; //wdit->val * wdit->val
      }
      //set wdit->val = a
      wdit->val = a;
//       doc.norml0 += 1;
//       doc.norml1 += wdit->val;
//       doc.norml2 += (wdit->val * wdit->val);
    
//       //add if not already there, if it's there, it will be the last one
//       if (i != doc.words.back())
//       {
//         doc.words.push_back(i);
//         doc.nwords++;
//       }
    }    
  }
  
  //now normalize vals
  for (i=0; i<this->nwords; ++i)
  {
    //get the word
    ivWord& w = this->words[i];
    
    //loop on its documents
    for (ivWordDocIt wdit=w.docs.begin(), wditend=w.docs.end(); wdit!=wditend; wdit++)
      //normalize
      wdit->val = this->normVal(wdit->val, this->docs[wdit->doc], norm);
    
  }

//  for (i=0; i<this->ndocs; ++i)
//  {
//    ivDoc& doc = this->docs[i];
//    cout << "doc=" << i << " norm=" << doc.norm << " nwords=" << doc.nwords 
//            << " ntokens=" << doc.ntokens << endl;
//    
//  }
  
//  for (i=0; i<this->docs[5].nwords; ++i)
//  {
//    ivWordDocIt wdit;
//    for (wdit=this->words[(uint)this->docs[5].words[i]].docs.begin();
//      wdit!=this->words[(uint)this->docs[5].words[i]].docs.end(); wdit++)
//    {
//      if (wdit->doc!=5)
//        continue;
//      
//      cout << wdit->val << " ";
//    }
//  }
}
 
//------------------------------------------------------------------------
float ivFile::weightVal(float val, ivWord& word, ivDoc& doc, ivFile::Weight wt)
{
  switch(wt)
  {
    case WEIGHT_BIN:
      val = val>0 ? 1 : 0;
      break;
    case WEIGHT_TF:
      val = doc.ntokens ? val / doc.ntokens : val;
      break;
    case WEIGHT_TFIDF:
      val = doc.ntokens ? val / doc.ntokens : val;
      val *= log2(this->ndocs / (word.ndocs + EPS));
      break;
  }
  return val;
}

//------------------------------------------------------------------------
float ivFile::normVal(float val, ivDoc& doc, ivFile::Norm norm)
{
  switch(norm)
  {
    case NORM_L0:
      val /= (EPS + doc.norm);
      break;
    case NORM_L1:
      val /= (EPS + doc.norm);
      break;
    case NORM_L2:
      val /= (EPS + sqrt(doc.norm));
      break;
  }
  return val;
}

//------------------------------------------------------------------------
float ivFile::dist(float val1, float val2, ivFile::Dist dist)
{
  switch(dist)
  {
    case DIST_L1:
      val1 -= val2;
      val1 = val1>0 ? val1 : -val1;
      break;
      
    case DIST_L2:
      val1 -= val2;
      val1 *= val1;
      break;
      
    case DIST_HAM:
      val1 = (float) ((uint)val1 ^ (uint)val2);
      break;
      
    case DIST_KL:
      break;
      
    case DIST_COS:
      val1 *= val2;
      break;      
      
    //there is intersection, return 1 if neither is 0
    case DIST_JAC:
      val1 = (val1==0 || val2==0) ? 0 : 1;
      break;
      
    //histogram intersection: return the minimum
    case DIST_HISTINT:
      val1 = (val1 <= val2) ? val1 : val2;
      break;
  }
  return val1;
}

//------------------------------------------------------------------------
float ivFile::dist2Norm(ivDoc& doc, ivFile::Dist dist, ivFile::Norm norm)
{  
  float ret = 0;
  
  //if not normalizing, then return the actual norm
  if (norm == NORM_NONE)
  {
    switch(dist)
    {
      case DIST_L1:
        ret = doc.norm; //doc.norml1;
        break;
      case DIST_L2:
        ret = doc.norm;
        break;
      case DIST_HAM:
        ret = doc.norm;
        break;
      case DIST_KL:
        break;
      case DIST_COS:
        ret = 0;
        break;      
      //return 0: which is the value with no intersection
      case DIST_JAC:
        ret = 0;
        break;
      //return 0: which is the value with no intersection
      case DIST_HISTINT:
        ret = 0;
        break;
    }  
  }
  //otherwise the norm is 1
  else
  {
    switch (dist)
    {
      case DIST_COS:
        ret = 0;
        break;
      //return 0: which is the value with no intersection
      case DIST_JAC:
        ret = 0;
        break;
      //return 0: which is the value with no intersection
      case DIST_HISTINT:
        ret = 0;
        break;
      default:
        ret = 1;
        break;
    }
  }
  
  return ret;
}


//------------------------------------------------------------------------
template <class T, class Tf>
void ivSearchFile(ivFile& iv, T* wlabel, uint ntokens, 
        Data<Tf>& dfeats, Data<Tf>& sfeats, uint sfeatoffset, FeatOpt& fopt,
        ivFile::Weight weight, ivFile::Norm norm, ivFile::Dist dist,
        bool overlapOnly, uint k, ivNodeList& scorelist, uint* sind)
//        float* scores, uint* docs, uint& nret)
{
  //return if empty
  if (iv.words.empty()) return;
  
  uint i;
  //allocate array for storing scores for all documents
//  ivNodeList scorelist;
  
//   uint* sind = new uint[MAX_DOC_TOKENS];
  
  //compute counts for words first
//   ivNodeList wordcount;
  ivWordDocList wordcount;
  //doc
  ivDoc doc; 
  doc.ntokens = ntokens;
  //sort wlabel
  for (i=0; i<ntokens; ++i) sind[i] = i;
  sort(sind, sind+ntokens, index_cmp<T*>(wlabel));
          
  //prev word and its count
  uint prevword=0, prevwordc=0;
  //loop
  for (i=0; i<ntokens; ++i)
  {
    uint tid = sind[i]; //i
    //skip word if invalid
    uint wr = (uint) wlabel[tid];
    if (wr==0 || wr>iv.nwords) continue;
    //get word count for this word, if not like prev one
    if (wlabel[tid] != prevword)
    {
      //update nwords for this doc
      doc.nwords++;
      //set
      prevword = wlabel[tid];
      //loop and get how many features have this word (doc.first[tid]) which
      //is 1-based, starting from next word
      prevwordc=1;
      for (uint w=i+1; w<ntokens; ++w)
        if (wlabel[sind[w]]==prevword) prevwordc++;
        else break;
    }
    
    //get that word
    ivWordDoc w; 
    //val
    w.val = (float) prevwordc; //1
    //put word label in doc place
    w.doc = wr - 1;
    //put feature id
    w.feat = sfeatoffset + tid; //i
    
    //insert at the back since they are already sorted by word
    wordcount.push_back(w);
//     //insert ascendingly on words
//     ivWordDocIt wit = lower_bound(wordcount.begin(), wordcount.end(), w);
//     //increment word count if new word
//     if (wit==wordcount.end() ||  w < (*wit)) doc.nwords++;
//     //check
//     wordcount.insert(wit, w);
    
//     if (wt==wordcount.end() || w<(*wt))	wt = wordcount.insert(wt, w);    
//     //increment
//     wt->val += 1;
  }
  //this doc stats: norms, weights, ...
  for (ivWordDocIt ni=wordcount.begin(), niend = wordcount.end(); ni!=niend; ni++)
  {
    //weight the value
    float a = iv.weightVal(1, iv.words[ni->doc], doc, weight);
//     ni->val = iv.weightVal(ni->val, iv.words[ni->doc], doc, weight);
    //compute norms
    switch(norm)
    {
      case ivFile::NORM_L0: doc.norm = doc.nwords;          break; //doc.norm++
      case ivFile::NORM_L1: doc.norm += a;                  break; //ni->val
      case ivFile::NORM_L2: doc.norm += a * a * ni->val;    break; //ni->val * ni->val
    }
    //set val
    ni->val = a;
//     doc.norml0++;
//     doc.norml1 += ni->val;
//     doc.norml2 += (ni->val * ni->val);    
  }
//  cout << "Doc: nwords=" << doc.nwords << " ntokens=" << doc.ntokens <<
//          " norm=" << doc.norm <<  " len=" << wordcount.size() << endl;
//  cout << "Doc6: nwords=" << iv.docs[5].nwords << " ntokens=" << iv.docs[5].ntokens <<
//          " norml0=" << iv.docs[5].norml0 << " norml1=" << iv.docs[5].norml1 << 
//          " norml2=" << iv.docs[5].norml2 << "\n";
  
  //get this documents's norm
  float docNorm = iv.dist2Norm(doc, dist, norm);

  //if computing distance to all documents, update score list with this word's
  // norm. The initial one has twice the norm of every document
  if (!overlapOnly && norm==ivFile::NORM_NONE)
  {
    //loop on the documents and add to score list
    for (ivNodeIt dsit = scorelist.begin(), dsend = scorelist.end(); dsit!=dsend; dsit++)
      //update norm to include this docs norm
      dsit->val = (dsit->val / 2) +  docNorm;      
  }
  
  DataFilter sfilter(1), dfilter(1);
  //now loop on the document words and update scores  
  for (ivWordDocIt wit=wordcount.begin(), witend = wordcount.end(); wit!=witend; wit++)
  {    
    //get the word    
    ivWord& word = iv.words[wit->doc];
//    cout << "\nword: " << wit->id << "\n";
    
    //get the doc feature
//     pair<Tf*,uint> docfeat = sfeats.getPoint(wit->feat);    
    sfilter[0] = wit->feat;
    sfeats.setFilter(&sfilter);

    //normalize this doc val
    wit->val = iv.normVal(wit->val, doc, norm);
//    cout << iv.words.size() << endl;

    //loop on the documents
    for (ivWordDocIt wdit = word.docs.begin(), wditend = word.docs.end(); 
      wdit != wditend; wdit++)
    {
//      cout << " doc: "<< wdit->doc+1;

      //get this worddoc feature
//       pair<Tf*,uint> feat = dfeats.getPoint(wdit->feat);
      dfilter[0] = wdit->feat;
      dfeats.setFilter(&dfilter);
      
      //compute dist between feats
      float fdist;
      distance(&fdist, sfeats, dfeats, fopt.dist);
//       cout << "sfeat: " << sfilter[0] << " doc: " << wdit->doc << 
//               " word: " << wit->doc << " dfeat: " << dfilter[0] << " dist: " << fdist ;
      //compute value
      switch(fopt.type)
      {
        case FeatOpt::VAL_BIN: fdist = fdist <= fopt.threshold ? 1 : 0;  break;
        case FeatOpt::VAL_EXP: fdist = exp(-fdist / fopt.threshold);     break;
        case FeatOpt::VAL_INV: fdist = 1 / (fdist + EPS);                break;
      }

      //now proceed only if fdist>EPS
      if (fdist>=EPS)
      {
        //get that document in the score list
        ivNodeIt dsit; 
        //if not overlap or scorelist has equal length to docs
        if (!overlapOnly || scorelist.size()==iv.docs.size())
        {
          dsit = scorelist.begin() + wdit->doc;
        }
        //if overlap only, then search for that document in the score list and 
        //insert if not already there
        else
        {
          ivNode ds; 
          ds.id = wdit->doc;
          dsit = lower_bound(scorelist.begin(), scorelist.end(), ds);

          //check if to insert to the list of scores (new doc)
          if (dsit==scorelist.end() || ds<(*dsit))
          {
            //init with sum of norms
            ds.val = docNorm + iv.dist2Norm(iv.docs[ds.id], dist, norm);
            //insert
            dsit = scorelist.insert(dsit, ds);
          }
        }

  //      if (dsit->id == 5) //(wdit->doc == 5)
        //compute distance
        float v1=wit->val * fdist, v2=wdit->val * fdist;
//         cout << "1-> " << v1 << " 2->" << v2;
//         cout << " dist=" << dsit->val;
        dsit->val -= iv.dist(v1,0,dist) + iv.dist(v2,0,dist);
//         cout << " dist=" << dsit->val;
        dsit->val += iv.dist(v1, v2, dist);
//         cout << " dist=" << dsit->val << endl;
                        
        
//         cout << " adding " << iv.dist(v1, v2, dist);
      }
//       cout  << endl;
      
      //clear dfilter
      dfeats.clearFilter();
    }
    
    //clear sfilter
    sfeats.clearFilter();
  }  

  //further processing
  if (dist == ivFile::DIST_JAC)
  {
    //we already have the intersections, so update to compute the Jac distance
    // dist = 1 - intersection / union
    // where intersection = dsit->val
    // and union = nwords1 + nwords2 - intersection
    for (ivNodeIt dsit=scorelist.begin(), dsitEnd=scorelist.end(); dsit!=dsitEnd; dsit++)
      dsit->val = 1 - dsit->val / (iv.docs[dsit->id].nwords + doc.nwords - dsit->val);
  }
  else if (dist == ivFile::DIST_COS)
  {
    //update cosine distance by changing the range from -1 -> 1 
    // (least similar -> most similar)
    // to 0 -> 2 (most similar -> least similar)
    // where newval = -oldval + 1
    for (ivNodeIt dsit=scorelist.begin(), dsitEnd=scorelist.end(); dsit!=dsitEnd; dsit++)
      dsit->val = 1 -dsit->val;
  }
  else if (dist == ivFile::DIST_HISTINT)
  {
    //update histogram intersection kernel by inverting and adding 1 
    //(assuming normalization)
    for (ivNodeIt dsit=scorelist.begin(), dsitEnd=scorelist.end(); dsit!=dsitEnd; dsit++)
      dsit->val = 1 - dsit->val;
  }
  
  //sort ascendingly
  ivNodeCmpValAsc ncv;
  sort(scorelist.begin(), scorelist.end(), ncv);
  
  //keep only k
  if (k>0 && k<scorelist.size())
    scorelist.resize(k);
  
  //clean memory
  wordcount.clear();  
}

//------------------------------------------------------------------------
template <class T, class Tf>
void ivSearchFile(ivFile& iv, Data<T>& data,
        Data<Tf>& dfeats, Data<Tf>& sfeats, FeatOpt& fopt,
        ivFile::Weight weight, ivFile::Norm norm, ivFile::Dist dist,
        bool overlapOnly, uint k, ivNodeLists& scorelists, bool verbose=false)
{
  //number of documents
  uint ndocs = data.size();
  
  //resize the scorelists  
  scorelists.resize(ndocs);
//   cout << "we have " << ndocs << " docs" << endl;
  
  //array for sorting
  uint* sind = new uint[MAX_DOC_TOKENS];
  
  //initialize score list with full list of documents, and just copy it 
  //afterwords. Init it with twice the norm
  ivNodeList tscorelist;
  if (!overlapOnly)
  {
    tscorelist.resize(iv.docs.size());
    //loop on the documents and add to score list
    ivDocIt dit = iv.docs.begin(), dend = iv.docs.end();
    ivNodeIt dsit = tscorelist.begin();
    for (uint i=0; dit!=dend; dit++, dsit++, i++)
    {
      //put the document id
      dsit->id = i;
      //put the initial distance: sum of norms
      dsit->val = 2 * iv.dist2Norm(*dit, dist, norm);
    }  
  }
  
  //now loop
  uint sfeatoffset = 0;
  for (uint d=0; d<ndocs; d++)
  {
    if (verbose && (d==0 ||  (d % (uint)round(0.1 * ndocs) == 0)) )
    {
      cout << " doc:" << d << " / " << ndocs << endl;
      cout.flush();
    }
    //get data for this document
    pair<T*, uint> doc = data.getPoint(d);
    
    //make a copy of tscorelist
    ivNodeList ttscorelist = tscorelist;
    //search for this document
    ivSearchFile(iv, doc.first, doc.second, 
            dfeats, sfeats, sfeatoffset, fopt,
            weight, norm, dist, overlapOnly, 
            k, ttscorelist, sind);
    //put back into scorelists
    scorelists[d] = ttscorelist;
    //clear
    ttscorelist.clear();
    //update sfeatoffset: offset of starting position of this doc's features
    sfeatoffset += doc.second;
  }
  
  //clear
  delete [] sind;
}

//------------------------------------------------------------------------
void ivFile::clear()
{
  //clears the docs array
  this->docs.clear();
  
  //clear words array
  this->words.clear();
}

//------------------------------------------------------------------------
//Stream overloads for ivWord
ostream& operator<<(ostream& os, ivWord& ivw)
{
  os.write((char*)&ivw.ndocs, sizeof(ivw.ndocs));
  os.write((char*)&ivw.wf, sizeof(ivw.wf));
}
istream& operator>>(istream& is, ivWord& ivw)
{
  is.read((char*)&ivw.ndocs, sizeof(ivw.ndocs));
  is.read((char*)&ivw.wf, sizeof(ivw.wf));
}

//------------------------------------------------------------------------
//Stream overloads for ivWordDoc
ostream& operator<<(ostream& os, ivWordDoc& ivwd)
{
//   os.write((char*)&ivwd.count, sizeof(ivwd.count));
  os.write((char*)&ivwd.doc, sizeof(ivwd.doc));
}
istream& operator>>(istream& is, ivWordDoc& ivwd)
{
//   is.read((char*)&ivwd.count, sizeof(ivwd.count));
  is.read((char*)&ivwd.doc, sizeof(ivwd.doc));
}

//------------------------------------------------------------------------
//Stream overloads for ivDoc
ostream& operator<<(ostream& os, ivDoc& ivdoc)
{
  os.write((char*)&ivdoc.ntokens, sizeof(ivdoc.ntokens));
//  os.write((char*)&ivdoc.nwords, sizeof(ivdoc.nwords));
}
istream& operator>>(istream& is, ivDoc& ivdoc)
{
  is.read((char*)&ivdoc.ntokens, sizeof(ivdoc.ntokens));
//  is.read((char*)&ivdoc.nwords, sizeof(ivdoc.nwords));
}
  
//------------------------------------------------------------------------
//Stream overloads for ivFile
ostream& operator<<(ostream& os, ivFile& ivf)
{
  uint i;
  //save the document array
  //
  //put the size
  os.write((const char*)&ivf.ndocs, sizeof(ivf.ndocs));
  //put the array
  for (i=0; i<ivf.ndocs; ++i)
    os << ivf.docs[i];

  //save the words array
  //
  //put the size
  os.write((const char*)&ivf.nwords, sizeof(ivf.nwords));
  //put the array
  for (i=0; i<ivf.nwords; ++i)
    os << ivf.words[i];
  
  //save the word docs arrays
  ivWordDocIt wdit;
  for (i=0; i<ivf.nwords; ++i)
  {
    //write for this word
    for (wdit=ivf.words[i].docs.begin(); wdit != ivf.words[i].docs.end();
      wdit++)
      os << *wdit;
  }  
}

istream& operator>>(istream& is, ivFile& ivf)
{
  uint i;
  
  //read the document array
  //
  //read the size & allocate
  is.read((char*)&ivf.ndocs, sizeof(ivf.ndocs));
  ivf.docs.resize(ivf.ndocs);
  //put the array
  for (i=0; i<ivf.ndocs; ++i)
    is >> ivf.docs[i];
  
  //read the words array
  //
  //read the size & allocate
  is.read((char*)&ivf.nwords, sizeof(ivf.nwords));
  ivf.words.resize(ivf.nwords);
//  cout << "Words: " << this->words.size() << "\n";
  //put the array
  for (i=0; i<ivf.nwords; ++i)
    is >> ivf.words[i];
  
  //read the word docs arrays
  ivWordDocIt wdit;
  for (i=0; i<ivf.nwords; ++i)
  {
    //get the size of this list
    uint s = ivf.words[i].ndocs;
    ivf.words[i].docs.resize(s);
    //read for this word
    for (wdit=ivf.words[i].docs.begin(); wdit != ivf.words[i].docs.end();
      wdit++)
      is >> *wdit;
  }
}

//------------------------------------------------------------------------
void ivFile::save(string filename)
{
  //open the file for opening
  ofstream of;
  of.open(filename.c_str(), ios_base::binary);
  
  //save
  of << *this;
  
  //close file
  of.close();
}


//------------------------------------------------------------------------
void ivFile::load(string filename)
{
  //open the file for opening
  ifstream inf;
  inf.open(filename.c_str(), ofstream::binary);
  
  inf >> *this;
  
  //close file
  inf.close();
}


//------------------------------------------------------------------------
void ivFile::display()
{
  uint i, j;
  
  //display the document array
  //
  cout << "Documents:\n----------\n";
  for (i=0; i<this->docs.size(); ++i)
  {
    cout << "Doc " << i << ": ntokens=" << this->docs[i].ntokens <<  
            " nwords=" << this->docs[i].nwords <<  
//             " norml0=" << this->docs[i].norml0 << 
//             " norml1=" << this->docs[i].norml1 << 
            " norm=" << this->docs[i].norm << "\n";
  }
  
  //display the words array
  cout << "\nWords:\n--------";
  for (i=0; i<this->words.size(); ++i)
  {
    cout << "\nWord " << i << ": (ndocs=" << this->words[i].ndocs <<
            ", wf=" << this->words[i].wf << ")\n\t";
    
    ivWordDocIt wdit;
    for (wdit=this->words[i].docs.begin(); wdit != this->words[i].docs.end();
      wdit++)
    {
      cout << "<doc=" << wdit->doc << ", feat=" << wdit->feat << 
              ", val=" << wdit->val << ">";
    }
  }
}

//------------------------------------------------------------------------

// #define TEMPLATE(F)         \
//   F(float)                  \
//   F(double)                 \
//   F(char)                   \
//   F(int)                    \
//   F(unsigned int)           \
//   F(unsigned char)          
  
  
// #define IVFILLFILE(T) \
// template void ivFillFile(ivFile&, T*, T*, uint, uint, uint);

#define IVFILLFILE_INT32 \
template void ivFillFile(ivFile&, Data<int32_t>&, uint, uint);

#define IVFILLFILE_UINT32 \
template void ivFillFile(ivFile&, Data<uint32_t>&, uint, uint);

//#define IVSEARCHFILE(T) \
//  template void ivSearchFile(ivFile&, T*, uint, ivFile::Weight, \
//        ivFile::Norm, ivFile::Dist,ivNodeList&);

// #define IVSEARCHFILE(T, Tf) \
// template void ivSearchFile(ivFile&, Data<T>&, Data<Tf>&, Data<Tf>&, FeatOpt&, \
//         ivFile::Weight, ivFile::Norm, ivFile::Dist, bool, uint, ivNodeLists&, bool);

#define IVSEARCHFILE_UINT32(Tf) \
template void ivSearchFile(ivFile&, Data<uint32_t>&, Data<Tf>&, Data<Tf>&, FeatOpt&, \
        ivFile::Weight, ivFile::Norm, ivFile::Dist, bool, uint, ivNodeLists&, bool);

#define IVSEARCHFILE_INT32(Tf) \
template void ivSearchFile(ivFile&, Data<int32_t>&, Data<Tf>&, Data<Tf>&, FeatOpt&, \
        ivFile::Weight, ivFile::Norm, ivFile::Dist, bool, uint, ivNodeLists&, bool);

// TEMPLATE(IVFILLFILE)
IVFILLFILE_INT32
IVFILLFILE_UINT32
        
TEMPLATE(IVSEARCHFILE_UINT32)
TEMPLATE(IVSEARCHFILE_INT32)
// TEMPLATE(IVSEARCHFILE)

#undef IVFILLFILE
#undef IVFILLFILE2
#undef IVSEARCHFILE

