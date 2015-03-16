function vals = ccvLshFuncVal(lsh, points, cellout)
% CCVLSHFUNCVAL returns the hash function values for the input points
%
% INPUTS
% ------
% lsh       - the input lsh
% points    - the input points, either a cell array with a vector per point
%             or a matrix [ndimsXnpoints]
% cellout   - [0] return the output in a cell array or not. Output is
%             returned in a cell array regardless in case there are more
%             than one table in the LSH index
%
% OUTPUTS
% -------
% vals      - the output function values, either a matrix [nfuncsXnpoints]
%             in case there is only 1 table and cellout==0, otherwise it's
%             a cell array, with one matrix per point
%             vals{i}=[nfuncsXntables]
%
% See also ccvLshClean ccvLshCreate ccvLshInsert ccvLshLoad ccvLshSave
% ccvLshSearch ccvLshBucketId ccvLshStats ccvLshBucketPoints ccvLshKnn
 
% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if ~exist('cellout','var') || isempty(cellout), cellout = 0; end;

%call the mex file
vals = mxLshFuncVal(lsh, points, cellout);

