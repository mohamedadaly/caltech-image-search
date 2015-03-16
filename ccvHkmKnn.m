function [ids, dists] = ccvHkmKnn(hkm, data, sdata, k, nchecks)
% CCVHKMKNN returns the k-nearest neighbors from the input sdata to the
% input data (which was used to create hkm)
%
% INPUTS
% ------
% hkm       - the input hkm
% data      - the input data
% sdata     - the search data
% k         - [1] the desired number of nearest neighbors
% nchecks   - number of backtracking steps. leave out or [] means use the
%             value specified in ccvHkmCreate
%
% OUTPUTS
% -------
% ids       - the ids of the nearest points in data
% dists     - the distances of the nearsest points
%
% See also ccvHkmImport ccvHkmExport ccvHkmClean ccvHkmCreate ccvHkmClean
%   ccvHkmLeafIds
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%defaults
if ~exist('k','var') || isempty(k), k = 1; end;
   
%call the mex file
if ~exist('nchecks','var') || isempty(nchecks)
  [ids dists] = mxHkmKnn(hkm, data, sdata, k);
else
  [ids dists] = mxHkmKnn(hkm, data, sdata, k, nchecks);
end;

