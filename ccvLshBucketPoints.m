function ids = ccvLshBucketPoints(lsh, buckets, table, nret)
% CCVLSHBUCKETPOINTS returns the ids of points in input buckets
%
% INPUTS
% ------
% lsh       - the input lsh
% buckets   - a vector of input bucket ids
% table     - the table id, where the first table is 1. If empty or 0, then
%             buckets must be [ntablesxnbuckets] where ntables is the
%             number of tables in the LSH index.
% nret      - [0] the number of ids to return per point, or 0 to return all
%             points
%
% OUTPUTS
% -------
% ids       - the output ids, either a cell array of one vector per bucket
%             (if nret==0) or a matrix of [nretXnbuckets]
%
% See also ccvLshClean ccvLshCreate ccvLshInsert ccvLshLoad ccvLshSave
% ccvLshBucketId ccvLshFuncVal ccvLshStats ccvLshSearch ccvLshKnn
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%default
if ~exist('nret','var') || isempty(nret), nret = 0; end;

%call the mex file
ids = mxLshBucketPoints(lsh, buckets, table-1, nret);

