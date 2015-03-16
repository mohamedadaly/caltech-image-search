function [words, nwords] = ccvBowGetDict(data, labels, locs, nwords, type, cluster, ...
  tparams, cparams, init, dfile)
% CCVBOWGETDICT computes the dictionary given the input data
% 
% INPUTS
% ------
% data    - the input data, one point per column [ndimsXnpoints]
% labels  - the labels for the data points
% locs    - the locations for the data points [2Xnpoints]
% nwords  - the number of words required
% type    - the type of dictionary
%           'flat'    - a flat dictionary from the data
%           'class'   - make a different dictionary per class
%           'spatial' - use a spatial pyramid
% cluster - the clustering method
%           'akmeans'   - use approximate k-means for clustering
%           'lsh'       - use LSH for clustering
%           'hkmeans'   - use Hierarchical K-means
% tparams - parameters for the type of dictionary
% cparams - parameters for the clustering method
%
% OUTPUTS
% -------
% words   - the output words
% nwords  - the output total no. of words
%
% See also ccvRansacAffine
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if ~exist('init','var'), init = []; end;
if ~exist('dfile','var'), dfile = []; end;

%global file for temp processing
global tempdictfile
tempdictfile = dfile;

switch type
  %flat dictionary
  case 'flat'
    if isfield(tparams, 'seed')
      if strcmp(class(data),'Composite')
        spmd 
          %init random
          old = ccvRandSeed(tparams.seed, 'set'); 
          data=data(:,randperm(size(data,2))); 
          %restore
          ccvRandSeed(old, 'restore');
        end;        
      else
        %init random
        old = ccvRandSeed(tparams.seed, 'set'); 
        data=data(:,randperm(size(data,2))); 
        %restore
        ccvRandSeed(old, 'restore');
      end;      
    end;
    
    %cluster 
    [words, nwords] = clusterData(data, nwords, cluster, cparams, init);  
    
  case 'lsh-multi'
    [words nwords] = clusterData(data, nwords, cluster, cparams, init);
    words.tparams = tparams;
    
  %multiple dictionaries
  case 'multiple'
    %init random
    old = ccvRandSeed(1234, 'set');
    %loop on number of dictionaries
    for d=1:tparams.ndict
      %get random permutation of features
      ids = randperm(size(data,2));
      %get dict for that permutation
      [words{d} nwords{d}] = clusterData(data(:,ids), nwords, cluster, cparams, init);
    end;
    %total words
    nwords = sum(nwords);
    %restore
    ccvRandSeed(old, 'restore');
    
  %make a separate dictionary for every class  
  case 'per-class'
    %unique claseses    
    nc = tparams.nc;
    cs = tparams.cs;
    %allocate output
    words = cell(1, nc);
    cnwords = zeros(1, nc);
    %loop on classes
    for ci=1:nc
      c = cs(ci);
      %build dictionary with data for that label
      [words{ci} cnwords(ci)] = clusterData(data(:, labels==c), nwords, cluster, cparams, init);
    end;
    %sum total words
    nwords = max(cnwords);

end; %switch type



% -----------------------------------------------------------------------
% clusters the input data
function [w, nw] = clusterData(feats, nwords, cluster, params, init)

switch cluster
  case 'akmeans'
    if strcmp(class(feats),'Composite')
      spmd npoints=size(feats,2); npoints=gplus(npoints,1); end
      npoints = npoints{1};
%       npoints=0; for i=1:length(feats), npoints=npoints+size(feats{i},2); end;
    else
      npoints = size(feats,2);
    end;
    %do we need to cluster?
    if npoints <= nwords
      w = feats;
    else
      %get the akmeans
      akmeans = ccvAkmeansCreate(feats, nwords, params{:});
      w = akmeans.means;
      ccvAkmeansClean(akmeans);
    end;    
    %set the class of words as that of feats
%       w = feval(class(feats), w);      
    nw = size(w, 2);
    
  case 'lsh'
    %set the dimension
    params{6} = size(feats,1);
    %put the parameters
    w.params = params;     
    w.nwords = nwords;
    
%     lsh = ccvLshCreate(params{:});
%     w.ids = unique(ccvLshBucketId(lsh, feats));
%     ccvLshClean(lsh);
    
    nw = nwords;
    
  case 'xkmeans'
    
    parallel = 0;
    try if matlabpool('size')>0, parallel = 1; end; catch end;
    
    %init    
    [ndims npoints] = size(feats);
    k = params.k;
    maxiter = params.maxiter;
    eps = params.eps;
    
    %compute the random directions
    seed = 123;
    if isfield(params,'seed'), seed = params.seed; end;
    lsh = ccvLshCreate(params.ndir, 1, 'cos','l2',0,ndims,0,1,seed);
    rnd = ccvRandSeed(seed, 'set');
%     w.dir = randn(params.ndir, size(feats,1), 'single');
    
    %compute signatures for input feats
    fprintf(' getting signatures..\n');
    rsig = ccvLshBucketId(lsh, feats);
    rsig = feval(params.class,rsig);
    datasig = zeros(1, npoints, params.class);
    for i=1:params.ndir
      datasig = bitor(bitshift(datasig,1), rsig(i,:));
    end;
    
    if parallel
      ss = Composite();
      len = ceil(npoints/length(ss));
      for l=1:length(ss)
        ss{l} = datasig((l-1)*len+1 : min(l*len, npoints));
      end;
%       spmd
%         %distribute
%         ss = codistributed(datasig, codistributor1d(2));
%         %get local part
%         xx=getLocalPart(ss);        
%       end;
    end;
    
    %initial means: random
    if ~exist('init','var') || isempty(init)
      mm = randperm(npoints); 
      means = feats(:, mm(1:k));
      meanssig = datasig(mm(1:k));
      meandists = zeros(1, k, 'single');
    %from input
    else
      means = init.means;
      meanssig = init.meanssig;
    end;
    oldmeans = means;
    
    iter=1; cont = 1;
    fprintf('  starting k-means..\n');
    while cont && iter<=maxiter
      ittic = tic;
      %get closest means
      nntic = tic;
      if ~parallel
        [ids dists] = ccvKnn(datasig, meanssig,1,'xor',1);
      else
        spmd
          [ii dd] = ccvKnn(ss, meanssig,1,'xor',1);
%           size(ss)
%           [ii dd] = ccvKnn(ss, meanssig,1,'xor',1);
        end;
        ids = cell2mat(ii(:)');
        dists = cell2mat(dd(:)');
      end;
      fprintf('  nn %.2f min ', toc(nntic)/60);
      
      %compute new means
      mtic = tic;
      parfor m=1:k
        %get ids for this mean
        mids = ids==m;
        %check if empty
        if ~any(mids)
          means(:,m) = oldmeans(:,m);
          meandists(m) = 0;
        else
          means(:,m) = mean(feats(:,mids), 2);  
          meandists(m) = sum(dists(mids));
        end;
      end;
      fprintf('means %.2f min\n', toc(mtic)/60);
      
      meandist(iter) = mean(meandists);
      
      %recompute mean sigs
      rsig = ccvLshBucketId(lsh,means);
      rsig = feval(params.class, rsig);
      meanssig = zeros(1, k, params.class);
      for i=1:params.ndir
        meanssig = bitor(bitshift(meanssig,1), rsig(i,:));
      end;
      
      fprintf('  iter %d: dist=%f took %.2f min\n', iter, meandist(iter), toc(ittic)/60);
            
      %check
      if iter>1 && abs(meandist(iter)-meandist(iter-1)) <= eps
        cont = 0;
      end;
      iter = iter + 1;
      
    end; %while
    
    %save the means
    w.means = means; %eval(sprintf('%s(means);', class(feats)));
    w.meanssig = meanssig;
    w.class = params.class;
    nw = size(means,2);
    
    ccvRandSeed(rnd,'restore');

    %clear lsh
    ccvLshClean(lsh);    
    
  case 'hkmeans'
    %get the hkmeans
    hkmeans = ccvHkmCreate(feats, params{:});
    %create the saveable struct
    w = ccvHkmExport(hkmeans, 0);
    %clean memory
    ccvHkmClean(hkmeans);    
    nw = nwords;
        
end; %switch cluster

