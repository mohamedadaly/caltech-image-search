function [ids, dists] = ccvKdtKnn(kdt, kdtData, sData, k, tData)
% CCVKDTKNN searches the KD-Tree for the k-nearest neighbors for the input
% data
% 
% INPUTS
% ------
% kdt       - the input kd-tree, output from CCVKDTCREATE
% kdtData   - the input data of the kd-tree, one column per point
%             [ndimsXns]
% sData     - the data to search for
% k         - [1] how many nearest neighbors to return
% tData     - [] if given, use this data to traverse the Kd-tree, while
%             using sData for actually computing distances and getting
%             nearest neighbors
%
% OUTPUTS
% -------
% ids       - the sorted ids of the k-nearest neighbors. 0 means empty
%             value, one column per input point [kXns]
% dists     - the distance to these nearest neighbors, Inf means empty
%             value.
%
% See also ccvKdtClean ccvKdtCreate
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%defaults
if ~exist('k','var') || isempty(k), k = 1; end;

%call the mex file
if nargin<=4
  [ids, dists] = mxKdtKnn(kdt, kdtData, sData, k);
else
  [ids, dists] = mxKdtKnn(kdt, kdtData, sData, k, tData);
end;

