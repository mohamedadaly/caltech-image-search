function lsh = ccvLshCreate(ntables, nfuncs, htype, dist, norm, ndims, ...
  w, tsize, seed, hwidth, bitsperdim)
% CCVLSHCREATE creates an LSH index
%
% INPUTS
% ------
% ntables   - [1] the number of tables
% nfuncs    - [5] the number of hash functions per table
% htype     - ['l2'] the type of hash function to use
%             'ham' -> use hamming distance, h = x[i]
%             'l1'  -> use l1 distance, h = floor((x[i]-b) / w)
%             'l2'  -> use l2 distance, h = floor((x . r - b) / w)
%             'cos' -> use cons ditance, h = sign(x . r)
%             'min-hash'  -> use min-hash function, h = min(perm(x))
%             'sph-sim'   -> use spherical simplex function
%             'sph-orth'  -> use spherical orthoplex
%             'sph-hyp'   -> use shperical hypercube
%             'bin-gauss' -> binary gaussian kernels
% dist      - ['l2'] the distance type to use
%             'l1'      -> l1 distance
%             'l2'      -> l2
%             'hamming' -> hamming
%             'cos'     -> dot-product
%             'arcos'   -> acos of dot-product
%             'bhat'    -> Bhattacharya distance
%             'kl'      -> KL-divergence
%             'jac'     -> Jacquard distance
%             'xor'     -> XOR distance for packed binary numbers
% norm      - [1] normalize or not
% ndims     - the number of dimensions for the input data
% w         - [.25] the size of the bin for 'l2' and 'l1' hash functions
% tsize     - [1000] the size of the table if it's a fixed size table, 
%             or 0 for a variable sized hash table
% seed      - random seed for input
% hwidth    - [0] number of bits of every hash value. In case hwidth~=0,
%             the outputs of the different hash functions are OR'ed
%             together at the right place, with function 1 at place
%             nfuncs*hwidth and function N at place 0 (LSB).
% bitsperdim - [0] number of bits per input dimension. If nonzero, every
%             dimension represents bitsperdim binary bits, in which case,
%             the number of dimensions for Hamming hash function is
%             bitsperdim*ndims. This is useful only for 'ham' hash function.
%
% OUTPUTS
% -------
% lsh       - the output lsh
%
% See also ccvLshClean ccvLshInsert ccvLshLoad ccvLshSave ccvLshSearch
% ccvLshBucketId ccvLshFuncVal ccvLshStats ccvLshBucketPoints ccvLshKnn

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if ~exist('ntables','var') || isempty(ntables), ntables = 1; end;
if ~exist('nfuncs','var') || isempty(nfuncs), nfuncs = 5; end;
if ~exist('htype','var') || isempty(htype), htype = 'l2'; end;
if ~exist('dist','var') || isempty(dist), dist = 'l2'; end;
if ~exist('norm','var') || isempty(norm), norm = 1; end;
if ~exist('w','var') || isempty(w), w = .25; end;
if ~exist('tsize','var') || isempty(tsize), tsize = 1000; end;
if ~exist('seed','var') || isempty(seed), seed = hex2num('ffff00'); end;
if ~exist('hwidth','var') || isempty(hwidth), hwidth = 0; end;
if ~exist('bitsperdim','var') || isempty(bitsperdim), bitsperdim = 0; end;

%call the mex file
lsh = mxLshCreate(ntables, nfuncs, htype, dist, norm, ndims, w, 1, hwidth, ...
  tsize, 0, 0, seed, bitsperdim, 0);

