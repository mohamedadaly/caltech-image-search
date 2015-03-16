function [akmeans] = ccvAkmeansCreate(data, k, maxiter, type, ntrees, ...
  varrange, meanrange, maxdepth, minvar, cycle, dist, maxbins, ...
  sample, mex, matlabout, seed, verbose)
% CCVAKMEANS computes kmeans clustering on the input data using 
% approximate nearest neighbors computed using randomized kd-trees
%
% Inputs:
% -------
% data        - the input data, with one column per data point
% k           - the number of means required
% maxiter     - [1000] max number of iterations to go for
% type        - ['kdt'] the type of Kdtree implementation to use
%               'kdt'    -> use ccvKdt* functions
% ntrees      - [1] the number of trees to generate
% varrange    - [0.8] the range of variance to choose for when randomizing
% meanrange   - [0] the range of mean to choose for when randomizing
% maxdepth    - [0] the maximum depth of the tree, unlimited if 0 or empty
% minvar      - [0] the minimum variance that must be there to split the
%               node
% cycle       - [0] whether to cycle through dimensions before repeating (1),
%               or reuse them regardless (0)
% dist        - ['l2'] the distance function to use
%               'hamming' -> hamming distance
%               'l1'      -> L1 distance (city block)
%               'l2'      -> L2 euclidean distance
%               'arccos'  -> arccos distance
%               'cos'     -> cosine distance
%               'xor'     -> hamming distance for packed binary inputs
% maxbins     - [25] the maximum number of bins to consider. It looks up
%               in the set of trees and adds branches in a priority queue
%               for all trees, ranked according to their distance from the
%               decision boundary, and then expands them in turn. If "0",
%               the default is the number of trees.
% sample      - [200] the size of the random sample to use to get the mean
%               and variance estimate at each node. 0 means don't sample
% mex         - [1] use the mex interface instead of the matlab interface
% matlabout   - [1] use the matlab output or the mex output if using the
%               mex interface i.e. return the kdt as a matlab array of
%               nodes or return the pointers
% verbose     - [1] print messages or not
% 
% Outputs:
% --------
% akmeans     - the return structure
%
% See also CCVAKMEANSLOOKUP, CCVAKMEANSCLEAN, CCVKDTCREATE, CCVBOWGETDICT
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010


if nargin<3 || isempty(maxiter),    maxiter = 1000;   end;
if nargin<4 || isempty(type),       type = 'kdt';  end;
if nargin<5 || isempty(ntrees),     ntrees = 1;       end;
if nargin<6 || isempty(varrange),   varrange = 0.8;     end;
if nargin<7 || isempty(meanrange),  meanrange = 0;    end;
if nargin<8 || isempty(maxdepth),   maxdepth = 0;     end;
if nargin<9 || isempty(minvar),     minvar = 0;       end;
if nargin<10 || isempty(cycle),     cycle = 0;       end;
if nargin<11 || isempty(dist),      dist = 'l2';      end;
if nargin<12 || isempty(maxbins),   maxbins = 25; end;
if nargin<13 || isempty(sample),    sample = 200;       end;
if nargin<14 || isempty(mex),       mex = 1;          end;
if nargin<15 || isempty(matlabout), matlabout = 0;    end;
if nargin<17 || isempty(verbose), 	verbose = 1;      end;

if ~exist('seed','var'), seed = 123; end;

%temp file
global tempdictfile
temp = false;
if ~isempty(tempdictfile), temp = true; end;

parallel = 0;
try if matlabpool('size')>0, parallel=1; end; catch end;

%size
if strcmp(class(data),'Composite')
  spmd [ndims npoints]=size(data); npoints=gplus(npoints,1); end
  ndims = ndims{1};
  npoints = npoints{1};
else
  [ndims, npoints] = size(data);
end;

if isdistributed(data)
  dclass= class(gather(data(1)));
elseif strcmp(class(data),'Composite')
  spmd dclass = class(data); end; dclass=dclass{1};
else
  dclass = class(data);
end;

if verbose, fprintf(' %d points\n', npoints); end;

%load means it exist in temp file
if temp && exist(tempdictfile, 'file')
  tt = load(tempdictfile);
  means = tt.means;
  oldmeans = means;
  mdist = tt.mdist;
  iiter = tt.iter;
  
%no temp file, random means
else
  iiter = 0;
  
  %get initial random means
  rnd = ccvRandSeed(seed, 'set');
  means = randperm(npoints);
  if isdistributed(data)
    means = gather(data(:, means(1:k)));
  elseif strcmp(class(data), 'Composite')
    spmd 
      [nd np]=size(data);  np=gcat(np); 
      %codistributor
      codist = codistributor1d(2,np, [nd sum(np)]);
      [s e] = codist.globalIndices(2, labindex);
      %ids on this lab
      m = means(1:k);
      m = m(m>=s & m<=e);
      means = data(:,m-s+1);
      means=gcat(means);
    end;
    means = means{1};
  else
    means = data(:, means(1:k));
  end;
  oldmeans = means;
  % meandists = zeros(1, k);
  ccvRandSeed(rnd, 'restore');
end; %if exist

%type of kdtree
switch type
  case 'flann'
    kdt = struct('nret',maxbins, 'dist','l2', 'ntrees',ntrees, ...
      'params',struct('checks', maxbins), 'flann',[]);
  case 'kdt'
    
end;


%init
if parallel
  spmd
    %distribute
%     ddata = codistributed(data, codistributor1d(2));
%     ddata = getLocalPart(ddata);
    if ~iscodistributed(data) && isreplicated(data)
      data = codistributed(data, codistributor1d(2));
    end;
    if iscodistributed(data)
      data = getLocalPart(data);
    end
  end;
end;

% diff = zeros(1, k);
%loop
cont = 1;
iter = iiter+1;
while cont && iter<=maxiter
  if verbose %&& (iter==1 || ~mod(iter, round(0.1*maxiter)))
    fprintf(1, '  Iter %d: ', iter); 
  end;  
  ittic = tic;
  
  %get rkdtrees for the means
  nntic = tic;
  switch type
    case 'kdt'
      if ~parallel
        %create      
        kdt = ccvKdtCreate(means, ntrees, varrange, meanrange, ...
          maxdepth, minvar, cycle, dist, maxbins, sample);
        %lookup and get nearest mean to every point
        [ids dists] = ccvKdtKnn(kdt, means, data, 1);
      else      
%         dd = distributed(data);
        spmd
          %create      
          kdt = ccvKdtCreate(means, ntrees, varrange, meanrange, ...
            maxdepth, minvar, cycle, dist, maxbins, sample);
          %search
          [ids dists] = ccvKdtKnn(kdt, means, data, 1);
%           [ii dd] = ccvKdtKnn(kdt, means, getLocalPart(data), 1);
%           [ii dd] = ccvKdtKnn(kdt, means, ddata, 1);
          %clear
          ccvKdtClean(kdt);
        end;
%         %gather
%         ids = cell2mat(ii(:)');
%         dists = cell2mat(dd(:)');
      end;        

    case 'flann'
      %create
      kdt.flann = flann_build_index(means, struct('algorithm','kdtree', ...
        'trees',kdt.ntrees, 'random_seed',1234));
      %lookup
      ids = flann_search(kdt.flann, data, 1, kdt.params);
%       %get neatest neighbors      
%       ids = zeros(1, npoints);
%       for i=1:npoints
%         [d, ids(i)] = ccvKnn(data(:,i), means(:,res(:,i)), 1, dist);
%         ids(i) = res(ids(i),i);        
%       end;
  end;  
  if verbose, fprintf('nn %.2f min ', toc(nntic)/60); end;
  
  %compute new means
  meantic = tic;
  if ~parallel
    [meandists,~] = ccvSumIndexed(dists, ids, k);
    %get sums and divide
    [means hist] = ccvSumIndexed(data, ids, k);
    %compute means
    means = feval(dclass, single(means) ./ single(repmat(hist, ndims, 1)));
  else
    spmd
      %sum distances
      [md,~] = ccvSumIndexed(dists, ids, k);
      meandists = gop(@plus, md, 1);
      md=[];
      %reduce
      [m, h] = ccvSumIndexed(data, ids, k);
      means = gop(@plus, m, 1);
      m=[];
%       meansl = feval(dclass, meansl);
      hist = gop(@plus, h, 1);  
      h=[];
      if labindex==1
        %compute mean
        means = feval(dclass, means ./ single(repmat(hist, ndims, 1)));
      end;
%       [m h md] = deal([]);
    end;
    %combine
    hist = hist{1};
    means = means{1};
    meandists = meandists{1};
%     clear histl meansl meandistsl
  end;
  
%   %compute mean
%   means = feval(dclass, single(means) ./ single(repmat(hist, ndims, 1)));
  %check empty means
  em = hist==0;
  means(:,em) = oldmeans(:,em);

%   parfor m=1:k
%     %get ids for this mean
%     mids = ids==m;
%     %check if empty
%     if ~any(mids)
%       means(:,m) = oldmeans(:,m);
%       meandists(m) = 0;
%     else
%       means(:,m) = mean(data(:,mids), 2);  
%       meandists(m) = sum(dists(mids));
%     end;
%   end;
  if verbose, fprintf('mean %.2f min ', toc(meantic)/60); end;
  
  mdist(iter) = mean(meandists);
  
  if verbose, fprintf('dist=%f, total %.2f min %s\n', mdist(iter), toc(ittic)/60, datestr(now)); end;
  
  %check if done
  if (iter>1 && abs(mdist(iter)-mdist(iter-1))<=1e-4) || iter==maxiter
    cont = 0;
  
  else
    %save to oldmeans
    oldmeans = means; %     oldids = ids;
    %clear the kdtree
    if ~parallel
      switch type
        case 'kdt',     ccvKdtClean(kdt);
        case 'flann',   flann_free_index(kdt.flann);
      end;
    end
    
    %save to temp
    if temp      
      save(tempdictfile, 'means','iter','mdist');
    end;
  end; 
  
  %inc iteration
  iter = iter + 1;  
end; %while

%build final if parallel
if parallel
  switch type
    case 'kdt',      
      kdt = ccvKdtCreate(means, ntrees, varrange, meanrange, ...
          maxdepth, minvar, cycle, dist, maxbins, sample);
    case 'flann',   flann_free_index(kdt.flann);
  end;
end;

%delete temp file
delete(tempdictfile);

akmeans.means = feval(dclass,means);
akmeans.ids = ids;
akmeans.type = type;
akmeans.kdt = kdt;


