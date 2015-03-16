function ccvLshInsert(lsh, points, idshift)
% CCVLSHINSERT inserts data into the Lsh index
%
% INPUTS
% ------
% lsh       - the input lsh
% points    - the input data, either a cell array of one vector per point
%             or a matrix of [ndimsXnpoints] one column per point
% idshift   - [0] a value to add to the ids of thse points, in case
%             of adding data in more than one call
%
% OUTPUTS
% -------
%
% See also ccvLshClean ccvLshCreate ccvLshLoad ccvLshSave ccvLshSearch
% ccvLshBucketId ccvLshFuncVal ccvLshStats ccvLshBucketPoints ccvLshKnn
 
% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if ~exist('idshift','var') || isempty(idshift), idshift = 0; end;

%call the mex file
mxLshInsert(lsh, points, idshift);

