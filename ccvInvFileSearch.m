function [ids dists] = ccvInvFileSearch(ivf, data, weight, norm, dist, nret, ...
  ovlponly, verbose)
% CCVINVFILESEARCH searches the inverted file structure for the input data
% and returns the nearest neighbors together with the distance. 
% This function MUST be called AFTER calling ccvInvFileCompStats with the
% same weight and norm, otherwise the results are undefined.
%
% INPUTS
% ------
% ivf       - the input inverted file created before
% data      - the data to search for, a cell array with one vector per
%             document having the word ids of its conents
% weight    - ['none'] the weighting to use
%             'none'  -> don't weight counts values
%             'bin'   -> binarize the counts
%             'tf'    -> term frequency, divide each count by the number of 
%                         words in the doc
%             'tfidf' -> use tf-idf weighting
% norm      - ['none'] the normalization to use
%             'none' -> no normalization
%             'l0'   -> normalize by the number of words
%             'l1'   -> normalize so that the histogram sum = 1
%             'l2'   -> normalize so that histogram sum square = 1
% dist      - ['l1'] the distance function to use
%               'l1'    -> l1 distance
%               'l2'    -> l2 distance
%               'ham'   -> hamming distance
%               'cos'   -> cos distance (dot product)
%               'jac'   -> Jacquard distance
% nret      - [0] the number of documents to return, 0 to return all
%             nearest neighbors
% ovlponly  - [1] return only documents with overlapping documents, or all
%             documents. If ovlponly=0 and nret=0, then nret is set
%             to the number of documents in the inverted file, and a matrix
%             is returned
% verbose   - [0] verbose or not
%
% OUTPUTS
% -------
% ids       - the sorted ids of nearest documents, either a cell array (if
%             nret==0 or ovlponly==0) or a matrix [nretXndocs]. 
%             0 ids means empty id.
% dists     - the distances to nearest documents
%
% See also ccvInvFileInsert ccvInvFileLoad ccvInvFileSave
% ccvInvFileClean ccvInvFileCompStats
%
  
% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010
 
if ~exist('weight','var') || isempty(weight), weight = 'none'; end;
if ~exist('norm','var') || isempty(norm), norm = 'none'; end;
if ~exist('dist','var') || isempty(dist), dist = 'l1'; end;
if ~exist('nret','var') || isempty(nret), nret = 0; end;
if ~exist('ovlponly','var') || isempty(ovlponly), ovlponly = 1; end;
if ~exist('verbose','var') || isempty(verbose), verbose = 0; end;

%call the mex file
[ids dists] = mxInvFileSearch(ivf, data, weight, norm, dist, nret, ...
  ovlponly, verbose);
