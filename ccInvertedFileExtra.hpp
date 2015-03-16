#ifndef CC_INVERTEDFILEEXTRA
#define CC_INVERTEDFILEEXTRA

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <stdint.h>

#include "ccData.hpp"
#include "ccDistance.hpp"

using namespace std;

//typedef unsigned int uint;
//typedef size_t uint;
//typedef unsigned int id_t;
typedef uint32_t uint;
typedef unsigned long ulong;

#define EPS 1e-10

//maximum tokens per document, this is used when inserting the inverted file
//to sort word labels
#define MAX_DOC_TOKENS 100000

//Access an element in a matrix
#define _MAT_ELEM_P(mat,row,col,nrows) (mat + (nrows)*(col) + (row))
#define _MAT_ELEM(mat,row,col,nrows) (*_MAT_ELEM_P(mat,row,col,nrows))

//------------------------------------------------------------------------
//Inverted file Word Document entry i.e. entry for a given word containing 
//counts in different documents
// [Wordi] = [wordDoc0] -> [wordDoc1] -> ... 
class ivWordDoc
{
public:
  //weighted value
  float val;
  //feature id
  uint feat;
  //document id
  uint doc;
  
  //constructor
  ivWordDoc()
  {
    feat = 0;
    val = 0;
    doc = 0;
  }
  
  //less than operator for ordering
  bool operator< (const ivWordDoc& j) const
  {
    return (this->doc < j.doc);
  }

  //stream overloads
  friend ostream& operator<<(ostream& os, ivWordDoc& ivworddoc);
  friend istream& operator>>(istream& is, ivWordDoc& ivworddoc);  
};

//list of ivWordDoc
typedef vector<ivWordDoc> ivWordDocList;
typedef vector<ivWordDoc>::iterator ivWordDocIt;

//------------------------------------------------------------------------
// inverted file Word entry
// [Word0] -> [Word1] -> ...
class ivWord
{
public:
  //number of documents that have this word
  uint ndocs;
  //number of times this word appears in the database
  uint wf;
  //documents entries
  ivWordDocList docs;
  
  //constructor
  ivWord()
  {
    ndocs = 0;
    wf = 0;
    docs.resize(0);
  }
  
  //destructor
  ~ivWord()
  {
    docs.clear();
  }

  //stream overloads
  friend ostream& operator<<(ostream& os, ivWord& ivword);
  friend istream& operator>>(istream& is, ivWord& ivword);  
};


//------------------------------------------------------------------------
typedef vector<uint> ivDocWordList;
typedef ivDocWordList::iterator ivDocWordIt;

//Inverted file Document entry having information about documents
class ivDoc
{
public:
//   //number of unique words in the document
//   float norml0;
//   //number of words in this document
//   float norml1;
//   //square sum of words in the document
//   float norml2;
  //norm used
  float norm;
  //number of tokens
  uint ntokens;
  //number of unique words
  uint nwords;
  
//   //list of word indices
//   ivDocWordList words;
  
  //constructor
  ivDoc()
  {
//     norml0 = 0;
//     norml1 = 0;
//     norml2 = 0;
    norm = 0;
    ntokens = 0;
    nwords = 0;
//     words.resize(0);
  }
  
  //destructor
  ~ivDoc()
  {
//     words.clear();
  }
  
  //stream overloads
  friend ostream& operator<<(ostream& os, ivDoc& ivdoc);
  friend istream& operator>>(istream& is, ivDoc& ivdoc);
};

//------------------------------------------------------------------------
//class to hold an id and a value
class ivNode
{
public:
  float val;
  uint id;
  
  //constructor
  ivNode()
  {
    val = id = 0;
  }
  
  //operator < for comparison by id
  bool operator< (const ivNode& d) const
  { return (this->id < d.id);  }  
  
};
//list of ivDocScore
typedef vector<ivNode>        ivNodeList;
typedef ivNodeList::iterator  ivNodeIt;

//list of lists
typedef vector<ivNodeList>     ivNodeLists;
typedef ivNodeLists::iterator  ivNodeListIt;

//class to compare ivNode by val
class ivNodeCmpValAsc
{ 
public:
  bool operator() (const ivNode& i,const ivNode& j) { return (i.val<j.val);} 
}; 

class ivNodeCmpValDesc
{ 
public:
  bool operator() (const ivNode& i,const ivNode& j) { return (i.val>j.val);} 
}; 


//------------------------------------------------------------------------
//Options for matching features
class FeatOpt
{
public:
  /// threshold value for matching
  float threshold;
  /// distance function to use
  DistanceType dist;
  /// what to put in the value as a function of dist & threshold
  enum Type
  {
    VAL_BIN,  // put 1 if dist<threshold, 0 otherwise
    VAL_EXP, //put exp(-dist / threshold)
    VAL_INV  //inverse 1/dist if dist>0
    
  } type;
};

//------------------------------------------------------------------------
//Inverted file structure

typedef vector<ivDoc> ivDocList;
typedef ivDocList::iterator ivDocIt;

typedef vector<ivWord> ivWordList;
typedef ivWordList::iterator ivWordIt;

class ivFile
{
public:
  //file name for disk index
  string diskFilename;
  //filename for memory part
  string memFilename;
  
  //array of word entries
  uint nwords;
  ivWordList words;
  
  //array of document entries
  uint ndocs;
  ivDocList docs;
  
  //number of tokens
  uint ntokens;
  
  //enumerators for normalization
  enum Norm
  {
    NORM_NONE,
    NORM_L0,
    NORM_L1,
    NORM_L2,           
  };
  
  //enumerators for distance measure
  enum Dist
  {
    DIST_L1,
    DIST_L2,
    DIST_HAM,
    DIST_KL,
    DIST_COS,
    DIST_JAC,
    DIST_HISTINT
  };
  
  //enumerators for weight to use
  enum Weight
  {
    WEIGHT_NONE,
    WEIGHT_BIN,
    WEIGHT_TF,
    WEIGHT_TFIDF
  };
  
  //constructor
  ivFile()
  { 
    nwords = 0;
    ndocs = 0;
    ntokens = 0;
  }
  
  //desctructor
  ~ivFile()
  {
    //clear memory
    this->clear();
  }
  
  //write to file
  void save(string filename);
  
  //load from file
  void load(string filename);
  
  void display();
  
  //clears the memory
  void clear();
  
  //compute stats: document norms, weights, ... to prepare for search
  void computeStats(ivFile::Weight wt, ivFile::Norm norm);
  
  //stream overloads
  friend istream& operator>>(istream& is, ivFile& ivf);
  friend ostream& operator<<(ostream& os, ivFile& ivf);
  
  //weight a document value
  inline float weightVal(float val, ivWord& word, ivDoc& doc, ivFile::Weight wt);
  
  //normalize a document value
  inline float normVal(float val, ivDoc& doc, ivFile::Norm norm);
  
  //compute distance
  inline float dist(float val1, float val2, ivFile::Dist dist);
  
  //convert form a distance to a norm
  inline float dist2Norm(ivDoc& doc, ivFile::Dist dist, ivFile::Norm norm);

  //fill the inverted file with input counts
  //
  // wlabel   - word labels for every token, 1->nwords
  // dlabel   - doc labels for every token. 1->ndocs
  // ntokens  - number of tokens (length of wlabel and dlabel)
  // nwords   - number of words
  // ndocs    - number of docs
  //
  template <class T>
  friend void ivFillFile(ivFile& iv, T* wlabel, T* dlabel, uint ntokens, 
          uint nwords, uint ndocs);
  
  //fill the inverted file with input counts
  //
  // data     - the input data, with one data vector per input consisting of 
  //            all the word labels for its tokens
  // nwords   - the total number of words
  // idshift  - a value to add to document all ids (useful for adding more data)
  template <class T>
  friend void ivFillFile(ivFile& iv, Data<T>& data, uint nwords, uint idshift=0);
  
  
  // search the inverted file for the closest document
  //
  // wlabel   - word labels for every token, 1->nwords
  // ntokens  - number of tokens (length of wlabel)
  // dfeats   - features corresponding to data already in inverted file, not cell array but fixed size
  // sfeats   - features corresponding to input data above
  // sfeatoffset - offset of this documents' features in sfeats
  // fopt     - options for feature matching
  // weight   - weight to use for individual counts (binary, term-freq, ..)
  // norm     - normalization of weights (L1, L2, ...)
  // dist     - distance measure to use (L1, L2, ...)
  // overlapOnly - return only those documents with overlapping words
  /// k       - no. of outputs required, if 0 then return everything
  // scorelist - a list of ivNode to hold the results
  // sind     - temp array to hold sorted entries for each document, size MAX_DOC_TOKENS
  //
  template <class T, class Tf>
  friend void ivSearchFile(ivFile& iv, T* wlabel, uint ntokens, 
        Data<Tf>& dfeats, Data<Tf>& sfeats, uint sfeatoffset, FeatOpt& fopt,
        ivFile::Weight weight, ivFile::Norm norm, ivFile::Dist dist,
        bool overlapOnly, uint k, ivNodeList& scorelist, uint* sind);

  
  // search the inverted file for the closest document
  //
  // data     - the input data, with one vector per iput
  // dfeats   - features corresponding to data already in inverted file, not cell array but fixed size
  // sfeats   - features corresponding to input data above
  // fopt     - options for feature matching
  // weight   - weight to use for individual counts (binary, term-freq, ..)
  // norm     - normalization of weights (L1, L2, ...)
  // dist     - distance measure to use (L1, L2, ...)
  // overlapOnly - return only those documents with overlapping words
  // k        - no. of output required per document, if 0 then return everything
  // scorelists   - a list of ivNOdeList to hold the results
  //
  template <class T, class Tf>
  friend void ivSearchFile(ivFile& iv, Data<T>& data,
        Data<Tf>& dfeats, Data<Tf>& sfeats, FeatOpt& fopt,
        ivFile::Weight weight, ivFile::Norm norm, ivFile::Dist dist,
        bool overlapOnly, uint k, ivNodeLists& scorelists, bool verbose=false);
};


#endif
