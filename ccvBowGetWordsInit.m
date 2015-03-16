function gws = ccvBowGetWordsInit(words, type, cluster, tparams, cparams)
% CCVBOWGETWORDSINIT initializes quantizing words e.g. returns the kd-tree
% to use for quantization
% 
% INPUTS
% ------
% words   - the words to use for quantization, output from ccvBowGetDict
% type    - the type of dictionary
%           'flat'    - a flat dictionary from the data
%           'class'   - make a different dictionary per class
%           'spatial' - use a spatial pyramid
% cluster - the clustering method
%           'akmeans'   - use approximate k-means for clustering
%           'lsh'       - use LSH for clustering
% tparams - parameters for the type of dictionary
% cparams - parameters for the clustering method
%
% OUTPUTS
% -------
% gws     - the return structure to use for quantization
%
% See also ccvBowGetDict
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010


switch type
  %flat dictionary
  case {'flat', 'lsh-multi'}
    %cluster 
    gws.dict = getDict(words, cluster, cparams);
    
  %per-class
  case 'per-class'
    %no. of classes
    ncs = length(words);
    %loop
    gws.dict = cell(1, ncs);
    for c=1:ncs
      gws.dict{c} = getDict(words{c}, cluster, cparams);
    end;
    

end; %switch type

gws.type = type;
gws.cluster = cluster;
gws.tparams = tparams;
gws.cparams = cparams;



% -----------------------------------------------------------------------
% clusters the input data
function dict = getDict(words, cluster, params)

switch cluster
  case 'akmeans'
    dict = ccvKdtCreate(words, params{3:end});
    
  case 'lsh'
    dict.lsh = ccvLshCreate(words.params{:});
%     dict.ids = words.ids;
    dict.nwords = words.nwords;
    
  case 'xkmeans'
    %get the lsh for computing signatures
    seed = 123;
    if isfield(params,'seed'), seed = params.seed; end;
    dict.lsh = ccvLshCreate(params.ndir, 1, 'cos','l2',0,size(words.means,1),0,1,seed);    
    
    dict.ndir = params.ndir;
    dict.class = params.class;
    
  case 'hkmeans'
    dict = ccvHkmImport(words);
    
end; %switch cluster
