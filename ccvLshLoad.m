function lsh = ccvLshLoad(file)
% CCVLSHLOAD loads a saved Lsh index
%
% INPUTS
% ------
% file      - the input file name
%
% OUTPUTS
% -------
% lsh       - the output lsh
%
% See also ccvLshClean ccvLshCreate ccvLshInsert ccvLshSave ccvLshSearch
% ccvLshBucketId ccvLshFuncVal ccvLshStats ccvLshBucketPoints ccvLshKnn

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%call the mex file
lsh = mxLshLoad(file);

