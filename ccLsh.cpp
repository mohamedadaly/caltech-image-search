// Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
// Date: October 6, 2010

#include <cstdlib>
#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>
#include <boost/random.hpp>

#include "ccCommon.hpp"
#include "ccLsh.hpp"

//random number generator
//boost::lagged_fibonacci23209         randEngine(0x123456);
boost::mt19937                      randEngine(0x123456);
//boost::uniform_01<boost::mt19937>   randUniReal(randEngine);
//boost::uniform_int<uint32_t>        randUniInt;
//boost::normal_distribution<double>  randNorm;



//a random number between 0 -> 1
//#define URAND (((double)randEngine() - randEngine().min()) / (double)randEngine().max_value)
//#define URAND (randEngine())
//#define URAND ((double)randUniInt(randEngine) / randUniInt.max())
//#define URAND (randUniReal())
#define URAND ((double)rand() / (double)RAND_MAX)

//get a random number between M1->M2-1
#define RAND(M1, M2) (URAND * ((M2)-(M1)) + (M1))
//#define RAND(M1, M2) ((double)randUniInt(randEngine) / randUniInt.max() * ((M2)-(M1)) + (M1))
//#define RAND(M1, M2) (float)rand() / (float)RAND_MAX * ((M2)-(M1)) + (M1)
//#define RAND(M2) RAND(0,M2)

#define IRAND(M1, M2) floor(RAND(M1,M2))
//#define IRAND(M2) IRAND(0,M2)


//normal random
//#define NRAND   randNorm(randEngine)
#define NRAND randn()

//PI
#define PI 3.141592653589793f

//------------------------------------------------------------------------
// // Template function definitions
// #define TEMPLATE(F)         \
//   F(float)                  \
//   F(double)                 \
//   F(char)                   \
//   F(int)                    \
//   F(unsigned int)           \
//   F(unsigned char)          
  
#define SEARCHPOINTS(T) \
  template void searchPoints(Lsh&, Data<T>&, LshPointList*);

#define INSERTPOINTS(T) \
  template void insertPoints(Lsh&, Data<T>&, uint);

#define BUCKETID(T) \
  template void getBucketId(Lsh&, Data<T>&, BucketIdList*);

#define GETFUNCVAL(T) \
template void getFuncVal(Lsh&, Data<T>&, LshFuncValList*);

#define GETKNN_FL(T) \
template void getKnn(Lsh&, Data<T>&, Data<T>&, uint k, DistanceType, uint*, float* );

#define GETKNN_DB(T) \
template void getKnn(Lsh&, Data<T>&, Data<T>&, uint k, DistanceType, uint*, double* );


TEMPLATE(SEARCHPOINTS)
TEMPLATE(INSERTPOINTS)
TEMPLATE(BUCKETID)
TEMPLATE(GETFUNCVAL)
TEMPLATE(GETKNN_FL)
TEMPLATE(GETKNN_DB)

//-----------------------------------------------------------------------
/// stream overloads for LshOptions
ostream& operator<<(ostream& s, LshOptions& o)
{
  //write
  s.write((char*) &o, sizeof(LshOptions));
//  s.write((char*) &o.ntables, sizeof(o.ntables));
//  s.write((char*) &o.nfuncs, sizeof(o.nfuncs));
//  s.write((char*) &o.htype, sizeof(o.htype));
//  s.write((char*) &o.dist, sizeof(o.dist));
//  s.write((char*) &o.norm, sizeof(o.norm));
//  s.write((char*) &o.ndims, sizeof(o.ndims));
//  s.write((char*) &o.nbits, sizeof(o.nbits));
//  s.write((char*) &o.hwidth, sizeof(o.hwidth));
//  s.write((char*) &o.opt1, sizeof(o.opt1));
//  s.write((char*) &o.opt2, sizeof(o.opt2));
//  s.write((char*) &o.tablesize, sizeof(o.tablesize));
//  s.write((char*) &o.str, sizeof(o.str));
//  s.write((char*) &o.forest, sizeof(o.forest));
//  s.write((char*) &o.verbose, sizeof(o.verbose));
}

istream& operator>>(istream& s, LshOptions& o)
{
  //read
  s.read((char*) &o, sizeof(LshOptions));
//  s.read((char*) &o.ntables, sizeof(o.ntables));
//  s.read((char*) &o.nfuncs, sizeof(o.nfuncs));
//  s.read((char*) &o.htype, sizeof(o.htype));
//  s.read((char*) &o.dist, sizeof(o.dist));
//  s.read((char*) &o.norm, sizeof(o.norm));
//  s.read((char*) &o.ndims, sizeof(o.ndims));
//  s.read((char*) &o.nbits, sizeof(o.nbits));
//  s.read((char*) &o.hwidth, sizeof(o.hwidth));
//  s.read((char*) &o.opt1, sizeof(o.opt1));
//  s.read((char*) &o.opt2, sizeof(o.opt2));
//  s.read((char*) &o.tablesize, sizeof(o.tablesize));
//  s.read((char*) &o.str, sizeof(o.str));
//  s.read((char*) &o.forest, sizeof(o.forest));
//  s.read((char*) &o.verbose, sizeof(o.verbose));
}

//-----------------------------------------------------------------------
// a simple function to compute standard gaussian random numbers
// using Box-Muller polar transformations
inline float randn()
{
  float x1, x2, w;
  do 
  { 
    x1 = 2.0 * URAND - 1.0; 
    x2 = 2.0 * URAND - 1.0; 
    w = x1 * x1 + x2 * x2; 
  } while ( w >= 1.0  || w == 0); 
  return x1 * sqrt(-2.0 * log( w ) / w);
}

//-----------------------------------------------------------------------
// a simple hash function that returns the remainder modulo a prime number
// using Rabin's algorithm
inline uint64_t hash(uint64_t v) { return (v % HASH_POLY); }
inline uint64_t hash(uint32_t v) { return ((uint64_t)v % HASH_POLY); }
inline uint64_t hash(int64_t v) { return ((uint64_t)v % HASH_POLY); }
inline uint64_t hash(int32_t v) { return ((uint64_t)v % HASH_POLY); }
inline uint64_t hash(float v) { return ((uint64_t)v % HASH_POLY); }
inline uint64_t hash(double v) { return ((uint64_t)v % HASH_POLY); }


//-----------------------------------------------------------------------
void LshFunc::create(const LshOptions& opt)
{
  this->type = opt.htype;
  this->ndims = (int)opt.ndims;
  float norm = 0.;
  switch(this->type)
  {
    case LSH_HASH_TYPE_HAM:
      //choose a random dimension
      if (!opt.bitsperdim)
        this->func.ham = (uint)IRAND(0, opt.ndims);
      else
        this->func.ham = (uint)IRAND(0, opt.ndims * opt.bitsperdim);
      break;
      
    case LSH_HASH_TYPE_L1:
      // choose a random dimension
      this->func.l1.dim = (uint)IRAND(0, opt.ndims);
      //random shift
      this->func.l1.s = (float)RAND(0, opt.w);
      break;

    case LSH_HASH_TYPE_L2:
      //random shift
      this->func.l2.b = (float)RAND(0, opt.w);
      //allocate memory
      this->func.l2.r = new float[(uint)opt.ndims];
      //random direction
      for (uint i=0; i<opt.ndims; ++i)
      {
//        this->func.l2.r[i] = randn();
        this->func.l2.r[i] = (float)NRAND;
//        cout << this->func.l2.r[i] << " ";
        norm += this->func.l2.r[i] * this->func.l2.r[i];
      }
//      cout << endl;
      //normalize
      norm = sqrt(norm);
      for (uint i=0; i<opt.ndims; ++i)
        this->func.l2.r[i] /= norm;
      break;

    case LSH_HASH_TYPE_COS:
      //allocate memory
      this->func.cos = new float[(uint)opt.ndims];
      //get random direction
      for (uint i=0; i<opt.ndims; ++i)
      {
//        this->func.cos[i] = randn();
        this->func.cos[i] = (float)NRAND;
        norm += this->func.cos[i] * this->func.cos[i];
      }
      //normalize
      norm = sqrt(norm);
      for (uint i=0; i<opt.ndims; ++i)
        this->func.cos[i] /= norm;
      break;
      
    case LSH_HASH_TYPE_MHASH:
    
      //choose two random numbers 0->2^32-1
      this->func.minhash.a = (uint32_t)IRAND(0, 0xFFFFFFFF); //(uint32_t)IRAND(0, 1e6); // 0xFFFFFFFF);
      this->func.minhash.b = (uint32_t)IRAND(0, 0xFFFFFFFF); //(uint32_t)IRAND(0, 1e6); //0xFFFFFFFF);
//      cout << "randmax = " << RAND_MAX << endl << flush;
      break;
      
    case LSH_HASH_TYPE_SPH_SIMPLEX:
    case LSH_HASH_TYPE_SPH_ORTHOPLEX:
    case LSH_HASH_TYPE_SPH_HYPERCUBE:    
      //allocate memory
      this->func.sphere = new float[this->ndims*(this->ndims+1)];
      //get the random rotation matrix: loop over columns
      for (uint j=0; j<this->ndims; ++j)
      {
        //generate this column
        for (uint i=0; i<this->ndims; ++i)
          this->func.sphere[j*this->ndims + i] = (float)NRAND;
        //do gram-schmidt
        for (uint k=0; k<j; ++k)
        {
          //get dot-product
          float dp = 0.;
          for (uint i=0; i<this->ndims; ++i)
            dp += this->func.sphere[j*this->ndims + i] * this->func.sphere[k*this->ndims + i];
          //subtract
          for (uint i=0; i<this->ndims; ++i)
            this->func.sphere[j*this->ndims+i] -= dp * this->func.sphere[k*this->ndims+i];
        }

        //calculate norm for this column
        norm = 0.;
        for (uint i=0; i<this->ndims; ++i)
          norm += this->func.sphere[j*this->ndims+i] * this->func.sphere[j*this->ndims+i];
        //normalize
        norm = sqrt(norm);
        for (uint i=0; i<this->ndims; ++i)
          this->func.sphere[j*this->ndims+i] /= norm;
      }
      
//      cout << "matrix:" << endl;
//      std::copy(this->func.sphere, this->func.sphere+this->ndims*(this->ndims+1),
//              ostream_iterator<float>(cout, " "));
//      cout << endl;

      //special handling for simplex
      if (this->type == LSH_HASH_TYPE_SPH_SIMPLEX)
      {
        //compute temp values
        float d1 = (this->ndims + 1 - sqrt(this->ndims+1)) / 
                (this->ndims * (this->ndims+1));
        float d2 = (1 - sqrt(this->ndims+1)) / this->ndims - d1;

        //compute vertices locations and rotate them
        float* v = new float[this->ndims * (this->ndims+1)];
        //loop on vertices: 1->d
        for (uint j=0; j<this->ndims; ++j)
          for (uint i=0; i<this->ndims; ++i)
            v[j*this->ndims + i] = (j==i? 1. : 0.) - d1;
        //last vertex
        for (uint i=0; i<this->ndims; ++i)
          v[this->ndims*this->ndims + i] = d2;

        //now rotate
        float* rot = new float[this->ndims * (this->ndims+1)];
        for (uint j=0; j<this->ndims+1; ++j)
          for (uint i=0; i<this->ndims; ++i)
          {
            float p = 0;
            for (uint k=0; k<this->ndims; ++k)
              p += this->func.sphere[k*this->ndims + i] * v[j*this->ndims + k];
             rot[j*this->ndims + i] = p;
          }

        //copy rotated vertices matrix into a
        copy(rot, rot+this->ndims*(this->ndims+1), this->func.sphere);

        //cleat memory
        delete [] v;
        delete [] rot;
      }
    
//      cout << "xx=[" ;
//      std::copy(this->func.sphere, this->func.sphere+this->ndims*(this->ndims+1),
//              ostream_iterator<float>(cout, " "));
//      cout << "];" << endl;
      
      break;    
      
    //Binary hashing with Gaussian Kernels
    case LSH_HASH_TYPE_BIN_GAUSS:
      //b: uniform [0 2pi]
      this->func.bingauss.b = (float)RAND(0, 2*PI);
      //t: uniform [-1 1]
      this->func.bingauss.t = (float)RAND(-1, 1);
      //w: independent gaussian with variance opt.w
      double l = sqrt(opt.w);
      this->func.bingauss.w = new float[(uint)opt.ndims];      
      for (uint i=0; i<opt.ndims; ++i)
        this->func.bingauss.w[i] = (float)(NRAND * l);
      
      break;
  }
}

//------------------------------------------------------------------------
/// computes the bucket id for this data
template <class T>
// uint64_t getId(LshFunc& func, const T* data, uint ndims, const LshOptions& opt,
//         float norm1, float norm2)
uint64_t getId(LshFunc& func, Data<T>& data, uint pid, const LshOptions& opt,
        float norm1, float norm2)

{
  float val;
  uint64_t ret;
  uint i;
  
  //stuff for non-sparse data
  T* point; uint ndims;
  if (!data.isSparse())
  {
    pair<T*,uint> p = data.getPoint(pid);
    point = p.first;
    ndims = p.second;
  }
  //sparse
  else
  {
    ndims = data.getSpPointDim(pid);
  }
  //rest should work just fine if not sparse, should handle sparse differently
  //inside each one
  
  //which hash function
  switch(func.type)
  {
    case LSH_HASH_TYPE_HAM:
      //the bit is 1 or 0
      if (opt.bitsperdim == 0)
        val = point[func.func.ham]==1;
      else
      {
        // compact binary data
        int m = func.func.ham % opt.bitsperdim;
        int d = (int) func.func.ham / opt.bitsperdim;         
        val = (float)( static_cast<uint64_t>(point[d]) >> m 
                  & (static_cast<uint64_t>(1)) );
//         cout << "m: " << m << " d: " << d << endl;
      }
      ret = (uint64_t) val;
      break;
      
    case LSH_HASH_TYPE_L1:
      //which cell is it in?
      if (opt.norm)
        val = floor(((float)point[func.func.l1.dim]/norm1 - func.func.l1.s + 1.) 
          / opt.w);
      else
        val = floor(((float)point[func.func.l1.dim] - func.func.l1.s) / opt.w);
      ret = (uint64_t) val;
      break;

    case LSH_HASH_TYPE_L2:
      //project the data point
      for (i=0, val=0; i<ndims; ++i)
      {
        val += (float)point[i] * func.func.l2.r[i];
//        norm += (float)point[i] * (float)point[i];
      }      
      //add offset to make values 0->2
      if (opt.norm)
      {
        norm2 = sqrt(norm2);
        val = (val / norm2) + 1.0; //norm2;
      }
      //which cell
//      cout << " val = " << val << " b=" << func.func.l2.b << endl;
      val = floor((val + func.func.l2.b) / opt.w);
      ret = (uint64_t) val;
      break;

    case LSH_HASH_TYPE_COS:
      //dot product
      for (val=0, i=0; i<ndims; ++i)
        val += (float)point[i] * func.func.cos[i];
      //get sign
      val = val>=0 ? 1 : 0;
      ret = (uint64_t) val;
      break;
      
    case LSH_HASH_TYPE_MHASH:
      //init
      ret = 0xffffffffffffffffl;
      uint64_t h;
      //loop 
      for (i=0; i<ndims; ++i)
      {
        //get hash
        h = ((uint64_t)func.func.minhash.a * (uint64_t)point[i] + 
                (uint64_t) func.func.minhash.b) % MIN_HASH_P;
        //compare min
        if (h<ret)  ret = h;
      }
//      //compute hash values
//      uint64_t* h = new uint64_t[ndims];
//      for (i=0; i<ndims; ++i)
//        h[i] = ((uint64_t)func.func.minhash.a * (uint64_t)point[i] + 
//                (uint64_t) func.func.minhash.b) % MIN_HASH_P;
//      
////      cout << "  d:"; copy(data, data+ndims, ostream_iterator<T>(cout, " ")); cout << endl;
////      cout << "  h:"; copy(h, h+ndims, ostream_iterator<uint64_t>(cout, " ")); cout << endl;
//              
//      //sort and keep the minimum
//      uint64_t* m = min_element(h, h+ndims);
//      ret = *m;
//      
//      //clear
//      delete [] h;

      break;
      
      case LSH_HASH_TYPE_SPH_SIMPLEX:
      {
        //compute inner product with all vertices and get the max
        float vmax = -10;
        uint64_t idmax = 0;
        //loop on vertices
        uint j;
        for (i=0; i<ndims+1; ++i)
        {
          //get dot-product
          for (j=0, val=0.; j<ndims; ++j)
            val += func.func.sphere[i*ndims + j] * (float)point[j];
          val /= sqrt(norm2);
          //compare max
          if (val >= vmax)
          {
            vmax = val;
            idmax = i;
          }                   
        }
        
        //return
        ret = idmax;
        break;
      }
      case LSH_HASH_TYPE_SPH_ORTHOPLEX:
      {
        //compute inner product with all vertices and get the max
        float vmax = 0.;
        uint64_t idmax = 0;
        //loop on vertices
        uint j;
        for (i=0; i<func.ndims; ++i)
        {
          //get dot-product
          for (j=0, val=0.; j<ndims; ++j)
            val += func.func.sphere[i*ndims + j] * (float)point[j];
          val /= sqrt(norm2);
          //compare max
          if ((val>=0? val : -val) >= (vmax>=0? vmax : -vmax))
          {
            vmax = val;
            idmax = i;
          }                   
        }
        
        //return
        ret = vmax>0 ? idmax : idmax + ndims;
        break;
      } 
      case LSH_HASH_TYPE_SPH_HYPERCUBE:
        //compute inner products
        ret = 0;
        for (i=0; i<ndims; ++i)
        {
          //get dot-product
          float t = 0;
          for (uint j=0; j<ndims; ++j)
            t += func.func.sphere[i*func.ndims + j] * (float)point[j];
          //if +ve, update with 2^i-1
          if (t>=0)
            ret += (1L << (i-1));
        }
        break;
    //Binary hashing with Gaussian Kernels
    case LSH_HASH_TYPE_BIN_GAUSS:
        //dot product of w with x: w . x + b
        float v;
        
        //sparse?
        if (!data.isSparse())
          for (i=0; i<ndims; ++i)
            v += func.func.bingauss.w[i] * (float)point[i];
        //sparse
        else
          for (i=0; i<ndims; ++i)
          {
            //get sparse point val
            uint row; T val;
            data.getSpPointVal(pid, i, val, row);
            //update dot product
            v += func.func.bingauss.w[row] * (float)val;
          }        
        
        //normalize if required by L2 norm
        if (opt.norm)
          v /= sqrt(norm2);
        
//         cout << "w.x=" << v << endl;
        
        //cos(w.x + b) + t
        v = (float)cos(v + func.func.bingauss.b) + func.func.bingauss.t;
//         cout << " cos(w.x+b)+t=" << v;
        //quantize with Q(v) = sign(v)
        v = v>=0 ? 1 : -1;
        //get binary
        v = 0.5 * (1 + v);
        ret = (uint64_t) v;
//         cout << " final=" << v << endl;
        
        break;
      
  }
  
//  cout << "  val = " << ret  << endl;
//  << " a=" << func.func.minhash.a << " b=" << 
//          func.func.minhash.b << endl;
  return ret;
}


//------------------------------------------------------------------------
// Stream overloads for LshFunc
ostream& operator<<(ostream& s, LshFunc& f)
{
  //write type and ndims
  s.write((char*)&f.type, sizeof(f.type));
  s.write((char*)&f.ndims, sizeof(f.ndims));
  
  //write function
  switch(f.type)
  {
    case LSH_HASH_TYPE_HAM:
      s.write((char*) &f.func.ham, sizeof(f.func.ham));
      break;
      
    case LSH_HASH_TYPE_L1:
      s.write((char*) &f.func.l1.dim, sizeof(f.func.l1.dim));
      s.write((char*) &f.func.l1.s, sizeof(f.func.l1.s));
      break;

    case LSH_HASH_TYPE_L2:
      s.write((char*) &f.func.l2.b, sizeof(f.func.l2.b));
      s.write((char*) f.func.l2.r, sizeof(float)*f.ndims);
      break;

    case LSH_HASH_TYPE_COS:
      s.write((char*) f.func.cos, sizeof(float)*f.ndims);
      break;
      
    case LSH_HASH_TYPE_MHASH:
      s.write((char*) &f.func.minhash, sizeof(f.func.minhash));
      break;
      
    case LSH_HASH_TYPE_SPH_SIMPLEX:
    case LSH_HASH_TYPE_SPH_ORTHOPLEX:
    case LSH_HASH_TYPE_SPH_HYPERCUBE:
      s.write((char*) f.func.sphere, sizeof(float)*f.ndims*(f.ndims+1));
      break;
  }
}

istream& operator>>(istream& s, LshFunc& f)
{
  //read type and ndims
  s.read((char*)&f.type, sizeof(f.type));
  s.read((char*)&f.ndims, sizeof(f.ndims));
  
  //read function
  switch(f.type)
  {
    case LSH_HASH_TYPE_HAM:
      s.read((char*) &f.func.ham, sizeof(f.func.ham));
      break;
      
    case LSH_HASH_TYPE_L1:
      s.read((char*) &f.func.l1.dim, sizeof(f.func.l1.dim));
      s.read((char*) &f.func.l1.s, sizeof(f.func.l1.s));
      break;

    case LSH_HASH_TYPE_L2:
      s.read((char*) &f.func.l2.b, sizeof(f.func.l2.b));
      f.func.l2.r = new float[f.ndims];
      s.read((char*) f.func.l2.r, sizeof(*f.func.l2.r)*f.ndims);
      break;

    case LSH_HASH_TYPE_COS:
      f.func.cos = new float[f.ndims];
      s.read((char*) f.func.cos, sizeof(*f.func.l2.r)*f.ndims);
      break;

    case LSH_HASH_TYPE_MHASH:
      s.read((char*) &f.func.minhash, sizeof(f.func.minhash));
      break;      
      
    case LSH_HASH_TYPE_SPH_SIMPLEX:
    case LSH_HASH_TYPE_SPH_ORTHOPLEX:
    case LSH_HASH_TYPE_SPH_HYPERCUBE:
      f.func.sphere = new float[f.ndims * (f.ndims+1)];
      s.read((char*) f.func.sphere, sizeof(float)*f.ndims*(f.ndims+1));
      break;
  }  
}

//------------------------------------------------------------------------
// Stream overloads for LshFunc
ostream& operator<<(ostream& s, LshFuncs& f)
{
  //first write the size
  uint z = f.size();
  s.write((char*) &z, sizeof(z));
  //then write the list of functions
  for (uint i=0; i<z; ++i)
    s << f[i];
}

istream& operator>>(istream& s, LshFuncs& f)
{
  //first read the size
  uint z;
  s.read((char*)&z, sizeof(z));
  //create the list and load
  f.funcs.resize(z);
  for (uint i=0; i<z; ++i)
    s >> f[i];
}

//------------------------------------------------------------------------
void LshBucketId::addId(uint64_t val, int pos, const LshOptions& opt)
{
  //string id
  if (this->str)
  {
    //make a string stream
    stringstream ss;
    //convert the value to string
    ss << setw((uint)opt.nbits) << val;
    //append this value to the string id
    this->id.s += ss.str();
  }
  //double id
  else
  {
//    //shift the input val the right number of bits
//    int i;
//    double t = 1;
//    for (i=0; i < opt.nbits * pos; ++i)
//      t *= 2;
//    t *= (double)val;
//    //add the value to the id
//    this->id.d += t;
    
//    //shift val the right number of bits
//    this->id.d += (val << opt.nbits*pos);
    
    //compute the hash for the new value
    //the new value f(AB) = f( f(A) * f(t^64) ) + f(B)
    // where A = this->id.d and B = val
    // and f(t^64) is HASH_POLY_REM specific to the HASH_POLY used
//    this->id.d = hash(val) + hash(this->id.d * HASH_POLY_REM);
//    cout << " " << this->id.d;
    if (opt.hwidth == 0)
      this->id.d = (val * HASH_POLY_A[pos % HASH_POLY_A_NUM] % HASH_POLY) + 
              (this->id.d * HASH_POLY_REM % HASH_POLY);
    else
      this->id.d = (this->id.d << opt.hwidth) | val;
//    cout << " + " << val <<  " -> " << this->id.d << endl;
  }
}

//------------------------------------------------------------------------
// LshBucketId getId(LshFuncs& funcs, const T* data, uint ndims,
//       const LshOptions& opt, float norm1, float norm2)
template <class T>
LshBucketId getId(LshFuncs& funcs, Data<T>& data, uint pid,
      const LshOptions& opt, float norm1, float norm2)
{
  //create a bucket id with the correct representation
  LshBucketId id(opt.str);
  
  //loop on the individual functions
  for (uint i=0, s=funcs.size(); i<s; ++i)
  {
    //get the value for this function
    uint64_t val = getId(funcs[i], data, pid, opt, norm1, norm2);
    
    //combine into the Id
    if (s>1)
      id.addId(val, i, opt);
    else
      id.id.d = val;
  }
  
  //return
  return id;
}

//------------------------------------------------------------------------
/// stream overloads
ostream& operator<<(ostream& s, LshBucketId& b)
{
  //put the type
  s.write((char*) &b.str, sizeof(b.str));
  //put the id
  if (b.str)
    s << b.id.s;
  else
    s.write((char*) &b.id.d, sizeof(b.id.d));
}

istream& operator>>(istream& s, LshBucketId& b)
{
  //read the type
  s.read((char*) &b.str, sizeof(b.str));
  //put the id
  if (b.str)
    s >> b.id.s;
  else
    s.read((char*) &b.id.d, sizeof(b.id.d));
}


//------------------------------------------------------------------------
// Stream overloads for LshBucket
ostream& operator<<(ostream& s, LshBucket& b)
{
  //write the bucket id
  s << b.id;
  //put the size of list of points
  uint z = b.size();
  s.write((char*) &z, sizeof(z));
  //put the ponits
  for (uint i=0; i<z; ++i)
    s.write((char*) &(b.plist[i]), sizeof(b.plist[i]));
}

istream& operator>>(istream& s, LshBucket& b)
{
  //read the bucket id
  s >> b.id;
  //get the size of list of points
  uint z;
  s.read((char*) &z, sizeof(z));
  b.resize(z);
  //get the ponits
  for (uint i=0; i<z; ++i)
    s.read((char*) &b.plist[i], sizeof(b.plist[i]));
}

//------------------------------------------------------------------------
void LshFuncs::create(LshOptions& opt)
{
  //allocate
  this->funcs.resize(opt.nfuncs);

  //loop on the list of functions and create
  for (uint i=0; i<opt.nfuncs; ++i)
    this->funcs[i].create(opt);
}

//------------------------------------------------------------------------
/// find a specific bucket in the list
bool LshBuckets::find(const LshBucket& b, LshBucketIt& bit, uint& id)
{
  bool found;
  LshBucketList* blist;
  
  //check if not fixed size
  if (this->fixed == false)
  {
    blist = &(this->varBuckets);
  }
  //fixed size table, so just take the mod and go to the bucket and get its 
  //list
  else
  {
    //get the index
    id = b.id.id.d % this->fixedBuckets.size(); //opt.tablesize;
    //get the iterator of the bucket head
    blist = &(this->fixedBuckets[id]);
//    //now search for it in the list
//    bit = lower_bound(bhead->buckets.begin(), bhead->buckets.end(), b);
//    //always found
//    found = true;
  }
  
  // binary search using lower_bound
  bit = lower_bound(blist->begin(), blist->end(), b);

  //check if found or not
  if (bit!=blist->end() && *bit == b)
    found = true;
  else
    found = false;
  
  //return
  return found;
}

//------------------------------------------------------------------------
/// insert a bucket into the list
LshBucketIt LshBuckets::insert(LshBucket& b, LshBucketIt bit, uint id)
{
  //insert into the list
  if (this->fixed)
    bit = this->fixedBuckets[id].insert(bit, b);
  else
    bit = this->varBuckets.insert(bit, b);
  
  return bit;
}


//------------------------------------------------------------------------
/// stream overloads for LshBucketList
ostream& operator<<(ostream& s, LshBucketList& b)
{
  //write size
  uint z = b.size();
  s.write((char*) &z, sizeof(z));
  //write the list of buckets
  for (LshBucketIt bs=b.begin(), be=b.end(); bs!=be; bs++)
    s << *bs;  
}
istream& operator>>(istream& s, LshBucketList& b)
{
  //read size
  uint z;
  s.read((char*) &z, sizeof(z));
  //allocate
  b.resize(z);
  //read the list of buckets
  for (LshBucketIt bs=b.begin(), be=b.end(); bs!=be; bs++)
    s >> *bs;  
}

//------------------------------------------------------------------------
/// stream overloads for LshBuckets
ostream& operator<<(ostream& s, LshBuckets& b)
{
  //write type
  s.write((char*) &b.fixed, sizeof(b.fixed));
  //type?
  if (fixed)
  {
    //write size
    uint z = b.size();
    s.write((char*) &z, sizeof(z));
    //loop on lists and write
    for (LshBucketListIt bs=b.fixedBuckets.begin(), be=b.fixedBuckets.end();
      bs!=be; bs++)
      s << *bs;
  }
  else
    s << b.varBuckets;
}

istream& operator>>(istream& s, LshBuckets& b)
{
  //read type
  s.read((char*) &b.fixed, sizeof(b.fixed));
  //type?
  if (fixed)
  {
    //read number of lists
    uint z;
    s.read((char*) &z, sizeof(z));  
    //resize
    b.fixedBuckets.resize(z);
    for (LshBucketListIt bs=b.fixedBuckets.begin(), be=b.fixedBuckets.end();
      bs!=be; bs++)
      s >> *bs;
  }
  else
    s >> b.varBuckets;
}

//------------------------------------------------------------------------
/// create functions for this table
void LshTable::createFuncs(LshOptions& opt)
{
  //create the functions
  this->funcs.create(opt);
}

//------------------------------------------------------------------------
/// stream overloads
ostream& operator<<(ostream& s, LshTable& t)
{
  //write funcs and buckets
  s << t.funcs;
  s << t.buckets;
}

istream& operator>>(istream& s, LshTable& t)
{
  //read
  s >> t.funcs;
  s >> t.buckets;
}

//------------------------------------------------------------------------
/// stream overloads for LshTables
ostream& operator<<(ostream& s, LshTables& t)
{
  //write size
  uint z = t.size();
  s.write((char*) &z, sizeof(z));
  //write the list of tables
  for (uint i=0; i<z; ++i)
    s << t[i];  
}

istream& operator>>(istream& s, LshTables& t)
{
  //read size
  uint z;
  s.read((char*) &z, sizeof(z));
  //allocate
  t.resize((uint)z);
  //read the list of buckets
  for (uint i=0; i<z; ++i)
    s >> t[i];  
}
  
//------------------------------------------------------------------------
// void insertPoint(LshTable& lshtab, T* point, uint ndims, uint pid, 
//         LshOptions& opt, float norm1, float norm2)
template <class T>
void insertPoint(LshTable& lshtab, Data<T>& data, uint inpid, uint pid,
        LshOptions& opt, float norm1, float norm2)
{
//  if (opt.verbose)
//    cout << "Inserting point " << pid << endl;
  
  // get the bucket id
  LshBucketId bid = getId(lshtab.funcs, data, inpid, opt, norm1, norm2);
  
  // make a bucket and search for it
  LshBucket b(bid);
  bool found;
  LshBucketIt bit;
  uint id;
  found = lshtab.buckets.find(b, bit, id);
//  LshBucketIt bit = lshtab.buckets.find(b, found, opt);
  
//  cout << "bucket id = " << bid.id.d << endl;
  
  //if not found, isnert it
  if (!found)
    bit = lshtab.buckets.insert(b, bit, id);
//    bit = blist->insert(b, bit);
//    bit = lshtab.buckets.insert(b, bit);
  
  //now we have an iterator for the bucket, so insert the point in there
  bit->insert(pid);
}

//------------------------------------------------------------------------
/// insert a set of points into the table
template <class T>
void insertPoints(LshTable& lshtab, Data<T>& data, uint idshift, 
        LshOptions& opt, float* norms1, float* norms2)
{
  //loop on the points and insert
  uint npoints = data.size();
//   pair<T*, uint> p;
  if (opt.norm)
    for (uint i=0; i<npoints; ++i)
    {
      insertPoint(lshtab, data, i, i+idshift, opt, norms1[i], norms2[i]);//       //get the pointer
//       p = data.getPoint(i);
//       //insert
//       insertPoint(lshtab, p.first, p.second, i+idshift, opt, norms1[i], norms2[i]);
    }
  else
    for (uint i=0; i<npoints; ++i)
    {
      insertPoint(lshtab, data, i, i+idshift, opt, 0, 0);
//       //get the pointer
//       p = data.getPoint(i);
//       //insert
//       insertPoint(lshtab, p.first, p.second, i+idshift, opt, 0, 0);
    }
    
  //print size of table
  if (opt.verbose)
    cout << "Table has " << lshtab.buckets.size() << " buckets" << endl << flush;
  
//  //print bucket ids
//  for (uint i=0; i<lshtab.buckets.size(); ++i)
//    cout << " id = " << lshtab.buckets[i].id.id.d << endl;
}

//------------------------------------------------------------------------
//get norm of the data to save time
template<class T>
void normDataInit(LshOptions& opt, Data<T>& data, float* &n1, float* &n2)
{
  //compute norms for the data
  if (opt.norm)
  {
    n1 = new float[data.size()];
    n2 = new float[data.size()];
    data.norm(n1, n2);
  }
}

template<class T>
void normDataEnd(LshOptions& opt, Data<T>& data, float* &n1, float* &n2)
{
  if (opt.norm)
  {
    delete [] n1;
    delete [] n2;
  }
}

//------------------------------------------------------------------------
/// insert points into the list of tables
template <class T>
void insertPoints(LshTables& lshtabs, Data<T>& data, uint idshift, LshOptions& opt)
{
  //compute norms for the data
  float* norms1;
  float* norms2;
  normDataInit(opt, data, norms1, norms2);
  
  //loop on the tables and isnert
  for (uint i=0; i<lshtabs.size(); ++i)
    insertPoints(lshtabs[i], data, idshift, opt, norms1, norms2);
  
  //cleat memory
  normDataEnd(opt, data, norms1, norms2);
}

//------------------------------------------------------------------------
// /// look for a point in the table
// void searchPoint(LshTable& lshtab, T* point, uint ndims, LshPointList& pl, 
//         LshOptions& opt, float norm1, float norm2)template <class T>
template <class T>
void searchPoint(LshTable& lshtab, Data<T>& data, uint pid, LshPointList& pl, 
        LshOptions& opt, float norm1, float norm2)
{
  //get the bucket id for that point
  LshBucketId bid = getId(lshtab.funcs, data, pid, opt, norm1, norm2);
  
  //search for this bucket
  LshBucket b(bid);
  bool found;
  LshBucketIt bit;
  uint id;
  found = lshtab.buckets.find(b, bit, id);
//  LshBucketIt bit = lshtab.buckets.find(b, found, opt);

  //check if found
  if (found && !bit->plist.empty())
  {
    //insert points
    pl.insert(pl.end(), bit->plist.begin(), bit->plist.end());
//    //loop on the points in that bucket and fill in retids
//    uint i;
//    for (i=0; i<nret && i<bit->size(); ++i)
//      retids[i] = (*bit)[i];
  }
}

//------------------------------------------------------------------------
/// look for a set of points in the table
template <class T>
void searchPoints(LshTable& lshtab, Data<T>& data, LshPointList* pls, 
        LshOptions& opt, float* norms1, float* norms2)
{
  //loop on points and search
  uint npoints = data.size();
  pair<T*, uint> p;
  if (opt.norm)
    for (uint i=0; i<npoints; ++i)
    {
      searchPoint(lshtab, data, i, pls[i], opt, norms1[i], norms2[i]);
//       //get the data pointer
//       p = data.getPoint(i);
//       //search
//       searchPoint(lshtab, p.first, p.second, pls[i], opt, norms1[i], norms2[i]);
    }
  else
    for (uint i=0; i<npoints; ++i)
    {
      searchPoint(lshtab, data, i, pls[i], opt, 0, 0);
//       //get the data pointer
//       p = data.getPoint(i);
//       //search
//       searchPoint(lshtab, p.first, p.second, pls[i], opt, 0, 0);
    }    
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
/// look for a set of points in the table
template <class T>
void searchPoints(LshTables& lshtabs, Data<T>& data, 
        LshPointList* pls, LshOptions& opt)
{
  //compute norms for the data
  float* norms1;
  float* norms2;
  if (opt.norm)
  {
    norms1 = new float[data.size()];
    norms2 = new float[data.size()];    
    data.norm(norms1, norms2);
  }
  
  //loop on the tables and search within the tables
  for (uint i=0; i<lshtabs.size(); ++i)
    searchPoints(lshtabs[i], data, pls, opt, norms1, norms2);
  
  //delete memory
  if (opt.norm)
  {
    delete [] norms1;
    delete [] norms2;
  }
}


//------------------------------------------------------------------------
/// return the k-nearest neighbors
template <class Tret, class T>
void getKnn(Lsh& lsh, Data<T>& lshData, Data<T>& sdata, 
        uint k, DistanceType dist, uint* ids, Tret* dists)
{
  //normalize data
  float* norms1,* norms2;
  normDataInit(lsh.opt, sdata, norms1, norms2);

//   cout << "size1:" << lshData.size() << " size2:" << sdata.size() << endl;
  //filter for search data
  DataFilter sfilter(1);
  
  //loop on the search data
  for (uint i=0, size=sdata.size(); i<size; ++i)
  {
//     //get the point
//     pair<T*,uint> p = sdata.getPoint(i);
            
    //point list
    LshPointList pl;
    if (lsh.opt.norm)
      //loop on the tables
      for (uint t=0; t<lsh.opt.ntables; ++t)
        //get the points in the same bucket
        searchPoint(lsh.tables[t], sdata, i, pl, lsh.opt, norms1[i], norms2[i]);
//         searchPoint(lsh.tables[t], p.first, p.second, pl, lsh.opt, norms1[i], norms2[i]);
    else
      //loop on the tables
      for (uint t=0; t<lsh.opt.ntables; ++t)
        //get the points in the same bucket
        searchPoint(lsh.tables[t], sdata, i, pl, lsh.opt, 0, 0);
//         searchPoint(lsh.tables[t], p.first, p.second, pl, lsh.opt, 0, 0);
      

    //get the filter
//    DataFilter filter(pl);
//    filter.resize(pl.size());
//    for (uint f=0; f<pl.size(); f++) filter[f] = pl[f];
    //get rid of duplicates
    sort(pl.begin(), pl.end());
    LshPointIt it = unique(pl.begin(), pl.end());
    pl.resize(it - pl.begin());
    
    //print pl
//     cout << "point: " << i << endl;
//     for (uint f=0; f<pl.size(); f++) cout << pl[f] << " "; cout << endl;
    
    
    //set the filter for Lsh Data
    lshData.setFilter(&pl);
    
    //set filter for sdata
    sfilter[0] = i;
    sdata.setFilter(&sfilter);
    
    //get Knn
    knn(dists+(i*k), ids+(i*k), k, sdata, lshData, dist);
    
    
    //update ids to get the real ones
    for (uint f=0, fs=pl.size(), j=i*k; f<fs && f<k; ++f, ++j)
      ids[j] = pl[ids[j]];
    
    //reset filter
    lshData.clearFilter();
    sdata.clearFilter();
    pl.clear();
  }
  
  //clear memoery
  sfilter.clear();
  normDataEnd(lsh.opt, sdata, norms1, norms2);  
}

//------------------------------------------------------------------------
template <class T>
void getBucketId(Lsh& lsh, Data<T>& data, BucketIdList* ids)
{
  //normalize data
  float* norms1,* norms2;
  normDataInit(lsh.opt, data, norms1, norms2);
  
  //loop on points
  for (uint i=0; i<data.size(); ++i)
  {
    //allocate ids
    ids[i].reserve(lsh.opt.ntables);
    
//     //get the data pointer
//     pair<T*,uint> p = data.getPoint(i);

    //loop on tables
    if (lsh.opt.norm)
      for (uint t=0; t<lsh.tables.size(); ++t)
      {
        //get bucket id
        LshBucketId bid = getId(lsh.tables[t].funcs, data, i, 
                lsh.opt, norms1[i], norms2[i]);
        //put it
        ids[i].push_back(bid.id.d);
      } //for t    
    else
      for (uint t=0; t<lsh.tables.size(); ++t)
      {
        //get bucket id
        LshBucketId bid = getId(lsh.tables[t].funcs, data, i, 
                lsh.opt, 0, 0);
        //put it
        ids[i].push_back(bid.id.d);
      } //for t    
      
  } //for i
  
  //clear memoery
  normDataEnd(lsh.opt, data, norms1, norms2);
}

//------------------------------------------------------------------------
template <class T>
void getFuncVal(Lsh& lsh, Data<T>& data, LshFuncValList* vals)
{
  //normalize data
  float* norms1,* norms2;
  normDataInit(lsh.opt, data, norms1, norms2);
  
  //loop on points
  for (uint i=0; i<data.size(); ++i)
  {
    //allocate ids
    vals[i].reserve(lsh.opt.ntables * lsh.opt.nfuncs);
    
//     //get the data
//     pair<T*,uint> p = data.getPoint(i);
    //loop on tables
    for (uint t=0; t<lsh.tables.size(); ++t)
    {
      //loop on the functions
      for (uint f=0; f<lsh.opt.nfuncs; ++f)
      {
        //get bucket id
        LshFuncVal_t v = (LshFuncVal_t) getId(lsh.tables[t].funcs[f], 
               data, i, lsh.opt, norms1[i], norms2[i]);
        //put it
        vals[i].push_back(v);
      } //for f
    } //for t    
  } //for i
  
  //clear memoery
  normDataEnd(lsh.opt, data, norms1, norms2);
}


//------------------------------------------------------------------------
void getBucketPoints(Lsh& lsh, BucketIdList& bids, uint table, LshPointList* pls)
{
  //check valid table id
  if (table >= lsh.tables.size()) return;

  //loop on the bucket ids  
  for (uint b=0; b<bids.size(); b++)
  {
    //get the bucket with that id
    LshBucketId bid; bid.id.d = bids[b];
    LshBucket bucket(bid);
    
    //search for that bucket and get the iterator
    LshBucketIt bit; uint id;
    bool found = lsh.tables[table].buckets.find(bucket, bit, id);
//    cout << "bucket: " << b << " found: " << found << endl;;
    //check if found then add points
    if (found)
      pls[b].insert(pls[b].end(), bit->plist.begin(), bit->plist.end());    
  }
}
  
//------------------------------------------------------------------------
/// insert points into the Lsh
template <class T>
void insertPoints(Lsh& lsh, Data<T>& data, uint idshift)
{
  //insert in the tables
  insertPoints(lsh.tables, data, idshift, lsh.opt);
}
  
//------------------------------------------------------------------------
/// look for points in the Lsh
template <class T>
void searchPoints(Lsh& lsh, Data<T>& data, LshPointList* pls)
{
  //search for poits in the tables
  searchPoints(lsh.tables, data, pls, lsh.opt);
}

//------------------------------------------------------------------------
/// initialize
void Lsh::init()
{
  //set the random number seed
//   srand(0xffff00);  
  srand(this->opt.seed);
  
//   randEngine.seed(0xffff00);
  randEngine.seed(this->opt.seed);
}
  
//------------------------------------------------------------------------
/// create the Lsh functions to use
void Lsh::createFuncs()
{
  //create the tables
  this->tables.resize(opt.ntables);
  
  //now create the functions
//  cout << "table size: " << tables.size() << endl;
  for (uint i=0; i<this->tables.size(); ++i)
    this->tables[i].createFuncs(this->opt);
  
  //allocate buckets if fixed size
  if (this->opt.tablesize > 0)
    for (uint i=0; i<this->tables.size(); ++i)
    {
      this->tables[i].buckets.fixed = true;
      this->tables[i].buckets.resize(this->opt.tablesize);
    }
}

//------------------------------------------------------------------------
/// stream overloads for Lsh
ostream& operator<<(ostream& s, Lsh& l)
{
  //write options and tables
  s << l.opt << l.tables;
}

istream& operator>>(istream& s, Lsh& l)
{
  s >> l.opt >> l.tables;
}

//------------------------------------------------------------------------
/// load and save for Lsh
void Lsh::load(string filename)
{
  //open the file for opening
  ifstream f;
  f.open(filename.c_str(), ifstream::binary);
  //read
  f >> *this;  
  //close file
  f.close();  
}

void Lsh::save(string filename)
{
  //open the file for writing
  ofstream f;
  f.open(filename.c_str(), ofstream::binary);
  //read
  f << *this;
  //close file
  f.close();  
}

//------------------------------------------------------------------------
// Compute Stats
void LshTable::computeStats(LshOptions& opt)
{
  //array to hold sizes of buckets
  vector<int> sizes;
  vector<int> listsizes;
  
  //loop on the buckets and fill points
  this->stats.nFullBuckets = 0;  
  this->stats.meanCollision = 0;
  //variable table size
  if (opt.tablesize == 0)
  {
    for (uint i=0; i<this->buckets.varBuckets.size(); ++i)
      if (this->buckets.varBuckets[i].size() > 0)
      {
        this->stats.nFullBuckets ++;
        sizes.push_back(this->buckets.varBuckets[i].size());
      }
  }
  //fixed table size, so we have to go through every bucket's list to 
  //count collisions
  else
  {
    //looop on bucket list in table
    for (uint i=0; i<this->buckets.fixedBuckets.size(); ++i)
    {      
      if (this->buckets.fixedBuckets[i].size() > 0)
        listsizes.push_back(this->buckets.fixedBuckets[i].size());
      //loop on bucket list within each bucket (buckets with the same mod id)
      for (LshBucketIt bs = this->buckets.fixedBuckets[i].begin(), 
            be = this->buckets.fixedBuckets[i].end(); bs != be; bs++)
        if (bs->size() > 0)
        {
          this->stats.nFullBuckets ++;
          sizes.push_back(bs->size());
        }     
    }
  }
        
  int size = sizes.size();
  
  //get mean and median
  double mean = 0., mean2 = 0., median=0.;
  for (uint i=0; i<size; ++i)
  {
    mean += sizes[i];
    mean2 += sizes[i] * sizes[i];
  }
  mean /= size;
  mean2 = sqrt((mean2/size) - (mean*mean));
  
  //sort and get median
  sort(sizes.begin(), sizes.end());
  median = size % 2 == 0 ? (sizes[size/2] +  sizes[size/2-1])/2 : 
    sizes[(size-1)/2];
    
  //mean collision size
  double cmean=0.;
  cmean = listsizes.size();
//  for (uint i=0; i<listsizes.size(); ++i)
//    cmean += listsizes[i];
//  cmean /= listsizes.size();
    
  //return
  this->stats.meanBucketSize = (int) mean;
  this->stats.stdBucketSize = (int) mean2;
  this->stats.medianBucketSize = (int) median;
  this->stats.meanCollision = (int) cmean;
  
  //clear
  sizes.clear();
  listsizes.clear();
}

//------------------------------------------------------------------------
// Compute Stats
void Lsh::computeStats()
{
  //loop on tables and compute stats
  for (uint i=0; i<this->opt.ntables; ++i)
    this->tables[i].computeStats(this->opt);    
}
