function [ids, dists] = ccvLshKnn(lsh, lshData, sData, k, dist)
% CCVLSHKNN returns the ids of the k-nearest neighbor points
%
% INPUTS
% ------
% lsh       - the input lsh
% lshData   - the data in the index, either a cell array of one vector per point
%             or a matrix of [ndimsXnpoints] one column per point
% sData     - the data to search for
% k         - [1] the number of nearest neighbors to return
% dist      - '' the distance function to use, if empty, use the dist
%             defined inside lsh index
% 
%
% OUTPUTS
% -------
% ids       - the output ids, a matrix of [kXns] where ns is the number of
%           points in sData, 0 means empty
% dists     - the distances to the nearest neighbors, Inf means empty
%
% See also ccvLshClean ccvLshCreate ccvLshInsert ccvLshLoad ccvLshSave
% ccvLshBucketId ccvLshFuncVal ccvLshStats ccvLshSearch ccvLshBucketPoints
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%default
if ~exist('k','var') || isempty(k), k = 1; end;
if ~exist('dist','var') || isempty(dist), dist = ''; end;


%call the mex file
[ids dists] = mxLshKnn(lsh, lshData, sData, k, dist);
