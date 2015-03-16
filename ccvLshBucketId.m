function ids = ccvLshBucketId(lsh, points)
% CCVLSHBUCKETID returns the bucket ids for the input ponits
%
% INPUTS
% ------
% lsh       - the input lsh
% points    - the input points, either a cell array with a vector per point
%             or a matrix [ndimsXnpoints]
%
% OUTPUTS
% -------
% ids       - the output bucket ids, a matrix of [ntablesXnpoints]
%
% See also ccvLshClean ccvLshCreate ccvLshInsert ccvLshLoad ccvLshSave
% ccvLshSearch ccvLshFuncVal ccvLshStats ccvLshBucketPoints ccvLshKnn
 
% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%call the mex file
ids = mxLshBucketId(lsh, points);

