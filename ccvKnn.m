function [ids, dists] = ccvKnn(data1, data2, k, dist, mex)
% CCVKNN gets the K nearest neighbors for each point in data1 to each point
% in data2
%
% INPUTS
% ------
% data1     - input data 1
% data2     - input data 2, one point per column
% k         - the number of nearest neighbors
% dist      - ['l2'] the distance function to use
%             'hamming' -> hamming distance
%             'l1'      -> L1 distance (city block)
%             'l2'      -> L2 euclidean distance
%             'arccos'  -> arccos distance
%             'cos'     -> cosine distance
%             'bhat'    -> Bhattatcharya Coefficient
%             'kl'      -> Symmetric Kullback-Leibler divergence
% mex       - [1] use the mex implementation or not
%
% OUTPUTS
% -------
% ids       - the outpout indices of the nearest neighbors, with one column
%             per point in data1 [k, n1]
% dists     - the output distances to the nearest neighbors sorted
%             ascendingly, with one column per point in data1 [k, n1]
%
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if nargin<4 || isempty(dist),   dist = 'l2'; end;
if nargin<5 || isempty(mex),    mex = 1; end;


%call mex file if exists
if mex && exist(['mxKnn.' mexext], 'file')
  [dists, ids] = mxKnn(data1, data2, k, dist);
%   ids = single(ids);
  
else
  %get distance
  dists = ccvDistance(data1, data2, dist, 1);
  
  %sort
  [dists, ids] = sort(dists, 1, 'ascend');
  
  %return only first k 
  dists = dists(1:k,:);
  ids = ids(1:k, :);
end;

