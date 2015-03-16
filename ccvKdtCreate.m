function kdt = ccvKdtCreate(data, ntrees, varrange, meanrange, maxdepth, ...
  minvar, cycle, dist, maxbins, sample, bitsperdim)
% CCVKDTCREATE creates a randomized Kd-tree / Kd-forest
% 
% INPUTS
% ------
% data      - the input data
% ntrees    - [1] the number of trees
% varrange  - [0.8] the range for variance within the max to choose the dimension from
% meanrange - [0] the range of mean to choose from
% maxdepth  - [0] a maximum depth for the tree (0 means unlimited), root of
%             the tree has depth 1, so a tree with three levels has depth=3
%             and total of 2^3-1=7 nodes i.e. 2^(depth-1) leaf nodes and
%             2^(depth)-1 total nodes
% minvar    - [0] the minimum variance to consider
% cycle     - [0] cycle through the dimensions or just choose the max
%             variance every iteration
% dist      - ['l2'] the distance to use
% maxbins   - [50] the maximum number of bins to use for searching
% sample    - [200] the number of samples to use to estimate the variance 
%             0 means use all points
% bitsperdim - [0] how many bits per dimension if compact binary input
%
% OUTPUTS
% -------
% kdt       - the output kdtree
%
% See also ccvKdtClean ccvKdtKnn
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%defaults
if ~exist('ntrees','var') || isempty(ntrees), ntrees = 1; end;
if ~exist('varrange','var') || isempty(varrange), varrange = 0; end;
if ~exist('meanrange','var') || isempty(meanrange), meanrange = 0; end;
if ~exist('maxdepth','var') || isempty(maxdepth), maxdepth = 0; end;
if ~exist('minvar','var') || isempty(minvar), minvar = 0; end;
if ~exist('cycle','var') || isempty(cycle), cycle = 0; end;
if ~exist('dist','var') || isempty(dist), dist = 'l2'; end;
if ~exist('maxbins','var') || isempty(maxbins), maxbins = 50; end;
if ~exist('sample','var') || isempty(sample), sample = 200; end;
if ~exist('bitsperdim','var') || isempty(bitsperdim), bitsperdim = 0; end;

%call the mex file
kdt = mxKdtCreate(data, ntrees, varrange, meanrange, maxdepth, minvar, cycle, ...
  dist, maxbins, sample, bitsperdim);

