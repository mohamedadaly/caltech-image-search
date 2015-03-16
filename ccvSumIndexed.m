function [dsum, dhist] = ccvSumIndexed(data, ids, k)
% CCVSUMINDEXED sums input points in data that are indexed by ids and puts
% results in sum. hist counts the number of input points for every output point.
% 
% INPUTS
% ------
% data    - input data [ndims X npoints]
% ids     - ids matrix
% k       - the maximum allowed id
%
% OUTPUTS
% -------
% dsum     - float matrix [ndims X k] that contains the sums of input poitns
%           with similar ids
% dhist    - counts of points having same id [1 X k]
%
% See also 
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%check class of ids
if ~strcmp(class(ids), 'uint32'), ids = uint32(ids); end;

%call mex
[dsum dhist] = mxSumIndexed(data, ids, k);