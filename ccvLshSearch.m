function ids = ccvLshSearch(lsh, points, nret)
% CCVLSHSEARCH searches the LSH index and returns indices of points in the
% same bucket for every point
%
% INPUTS
% ------
% lsh       - the input lsh
% points    - the input points, either a cell array with a vector per point
%             or a matrix [ndimsXnpoints]
% nret      - [0] the number of ids to return per point, or 0 to return all
%             points
%
% OUTPUTS
% -------
% ids       - the output ids, either a cell array of one vector per point
%             (if nret==0) or a matrix of [nretXnpoints]
%
% See also ccvLshClean ccvLshCreate ccvLshInsert ccvLshLoad ccvLshSave
% ccvLshBucketId ccvLshFuncVal ccvLshStats ccvLshBucketPoints ccvLshKnn
 
% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%default
if ~exist('nret','var') || isempty(nret), nret = 0; end;

%call the mex file
ids = mxLshSearch(lsh, points, nret);

