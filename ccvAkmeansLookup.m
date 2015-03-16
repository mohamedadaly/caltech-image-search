function [ids, dists] = ccvAkmeansLookup(akmeans, searchdata)
% AKMEANS computes kmeans clustering on the input data using approximate
% nearest neighbors computed using randomized kd-trees
%
% Inputs:
% -------
% akmeans     - the input akmeans struct
% searchdata  - the input search data
% 
% Outputs:
% --------
% ids         - the id of the closest mean to each input search point
% dists       - the distances to the closest mean
%
% See also CCVAKMEANSCREATE, CCVKDTREECREATE, CCVKDTREELOOKUP
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

ids = [];
dists = [];

%if empty return
if isempty(searchdata), return; end;

%lookup in the kdtree of means
switch akmeans.type
  case 'kdtree'
    [ids, dists] = ccvKdtreeLookup(akmeans.kdt, akmeans.means, searchdata, 1);
  case 'kdt'
    [ids, dists] = ccvKdtKnn(akmeans.kdt, akmeans.means, searchdata, 1);
end;

