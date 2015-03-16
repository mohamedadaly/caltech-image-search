function hkm = ccvHkmCreate(data, ni, nl, nb, dist, usekdt, ntrees, nchecks, ...
  nkdtrees, varrange, meanrange, cycle, maxbins, sample)
% CCVHKMCREATE creates a hierarchical k-means structure. It can also
% implement Approximate K-means (when levels = 1 and usekdt=1) or normal
% K-means (when levels=1 and usekdt=0)
%
% INPUTS
% ------
% data      - the input data
% ni        - [100] the number of iterations
% nl        - [1] number of levels
% nb        - [5] number of branches i.e. branching factor per level
% dist      - 'l2' the distance function to use
% usekdt    - [0] use a kd-tree or exhaustive search for k-means
% ntrees    - [1] number of HKM trees to use
% nchecks   - [ntrees] number of backtracking steps to use
% nkdtrees  - [4] the number of trees
% varrange  - [0.8] the range for variance within the max to choose the dimension from
% meanrange - [0] the range of mean to choose from
% cycle     - [0] cycle through the dimensions or just choose the max
% maxbins   - [50] the maximum number of bins to use for searching kd-trees
% sample    - [250] the number of samples to use to estimate the variance 
%             0 means use all points
%
% OUTPUTS
% -------
% hkm       - the output structure
%
% See also ccvHkmImport ccvHkmExport ccvHkmClean ccvHkmKnn ccvHkmLeafIds
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%defaults
if ~exist('ni','var') || isempty(ni), ni = 100; end;
if ~exist('nl','var') || isempty(nl), nl = 1; end;
if ~exist('nb','var') || isempty(nb), nb = 5; end;
if ~exist('usekdt','var') || isempty(usekdt), usekdt = 0; end;
if ~exist('dist','var') || isempty(dist), dist = 'l2'; end;
if ~exist('ntrees','var') || isempty(ntrees), ntrees = 1; end;
if ~exist('nchecks','var') || isempty(nchecks), nchecks = ntrees; end;

if ~exist('nkdtrees','var') || isempty(nkdtrees), nkdtrees = 4; end;
if ~exist('varrange','var') || isempty(varrange), varrange = 0.8; end;
if ~exist('meanrange','var') || isempty(meanrange), meanrange = 0; end;
if ~exist('cycle','var') || isempty(cycle), cycle = 0; end;
if ~exist('maxbins','var') || isempty(maxbins), maxbins = 50; end;
if ~exist('sample','var') || isempty(sample), sample = 250; end;
   
%call the mex file
hkm = mxHkmCreate(data, ni, nl, nb, dist, ntrees, nchecks, ...
  usekdt, nkdtrees, varrange, meanrange, cycle, maxbins, sample);

