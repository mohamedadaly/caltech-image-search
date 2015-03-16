function ids = ccvKdtPoints(kdt, sData, k)
% CCVKDTPOINTS searches the KD-Tree and returns the ids of the k points at
% the leaves
% 
% INPUTS
% ------
% kdt       - the input kd-tree, output from CCVKDTCREATE
% sData     - the data to search for, [ndimsXnpoints]
% k         - [1] how many neighbors to return. If 0, it returns a cell
%             array with all points found for each input point.
%
% OUTPUTS
% -------
% ids       - the ids of the neighbors. 0 means empty
%             value, one column per input point [kXnpoints]. If there were more 
%             than k points, k points are chosen at random. If k==0, ids is a
%             cell array will all neighbors.
%
% See also ccvKdtClean ccvKdtCreate ccvKdtKnn
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%defaults
if ~exist('k','var') || isempty(k), k = 1; end;

%call the mex file
ids = mxKdtPoints(kdt, sData, k);

