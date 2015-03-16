function DEMO
% DEMO a demo script to show typical usage

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

% Bag of Words demo
bag_of_words();

% Full Representation
full_representation();

%--------------------------------------------------------------------------
% Bag of Words 
%--------------------------------------------------------------------------
function bag_of_words()

% set seed
old_seed = ccvRandSeed(123, 'set');

% define some constants
fprintf('Creating features\n');
num_images = 100;
features_per_image = 1000;
dim = 128;
num_features = num_images * features_per_image;

% create some random features
features = uint8(ceil(rand(dim, num_features) * 255));
labels = reshape(repmat(uint32(1:num_images), features_per_image, 1), [], 1)';

% build the dictionary
dict_type = 'akmeans'; % try also 'hkmeans'
fprintf('Building the dictionary: %s\n', dict_type);
%%
switch dict_type
  % create an AKM dictionary
  case 'akmeans'
    num_words = 100;
    num_iterations = 5;
    num_trees = 2;
    dict_params =  {num_iterations, 'kdt', num_trees};

  % create an HKM dictionary
  case 'hkmeans'
    num_words = 100;
    num_iterations = 5;
    num_levels = 2;
    num_branches = 10;
    dict_params = {num_iterations, num_levels, num_branches};
end; % switch

% build the dictionary
dict_words = ccvBowGetDict(features, [], [], num_words, 'flat', dict_type, ...
  [], dict_params);

%%
% get the words for the features: words is a cell array with an entry for
% every image, which contains the word id for every feature
fprintf('Computing the words\n');
dict = ccvBowGetWordsInit(dict_words, 'flat', dict_type, [], dict_params);
words = cell(1, num_images);
for i=1:num_images
  words{i} = ccvBowGetWords(dict_words, features(:,labels==i), [], dict);
end;
ccvBowGetWordsClean(dict);

%%
% create an inverted file for the images
fprintf('Creating and searching an inverted file\n');
if_weight = 'none';
if_norm = 'l1';
if_dist = 'l1';
inv_file = ccvInvFileInsert([], words, num_words);
ccvInvFileCompStats(inv_file, if_weight, if_norm);

% search for the first two
[ids dists] = ccvInvFileSearch(inv_file, words(1:2), if_weight, if_norm, ...
  if_dist, 5)

% clear inv file
ccvInvFileClean(inv_file);

% restore seed
ccvRandSeed(old_seed, 'restore');

%%
% Min-Hash LSH index
fprintf('Creating and searching a Min-Hash LSH index\n');
ntables = 3;
nfuncs = 2;
dist = 'jac';

% create and insert
lsh = ccvLshCreate(ntables, nfuncs, 'min-hash', dist, 0, 0, 0, 100);
ccvLshInsert(lsh, words, 0);

% search for first two
[ids dists] = ccvLshKnn(lsh, words, words(1:2), 5, dist)

% clear
ccvLshClean(lsh);

end % bag_of_words function

%--------------------------------------------------------------------------
% Full Representation
%--------------------------------------------------------------------------
function full_representation()
%%
% set seed
old_seed = ccvRandSeed(123, 'set');

% define some constants
fprintf('Creating features\n');
num_images = 100;
features_per_image = 1000;
dim = 128;
num_features = num_images * features_per_image;

% create some random features
features = uint8(ceil(rand(dim, num_features) * 255));
labels = reshape(repmat(uint32(1:num_images), features_per_image, 1), [], 1)';

% types of Nearest Neighbor search methods
nn_types = {'kdt', 'hkm', 'lsh-l2', 'lsh-simplex'};

for nni=1:length(nn_types);
%
  % get the type
  type = nn_types{nni};
  
  % create the index
  fprintf('\nCreating index %d: %s\n', nni, type);
  switch type
    % Kd-Tree
    case 'kdt'
      ntrees = 4;
      index = ccvKdtCreate(features, ntrees);

    % Hierarchical K-Means
    case 'hkm'
      nlevels = 4;
      nbranches = 10;
      niterations = 20;
      index = ccvHkmCreate(features, niterations, nlevels, nbranches);
      
    % LSH-L2
    case 'lsh-l2'
      ntables = 4;
      nfuncs = 20;
      index = ccvLshCreate(ntables, nfuncs, 'l2', 'l2', 1, dim, .1, 1000);
      ccvLshInsert(index, features);
      
    % LSH-Simplex
    case 'lsh-simplex'
      ntables = 4;
      nfuncs = 2;
      index = ccvLshCreate(ntables, nfuncs, 'sph-sim', 'l2', 1, dim, .1, 1000);
      ccvLshInsert(index, features);
  end; % switch
  
  % get nearest neighbors for features
  fprintf('Searching for first image\n');
  switch type
    case 'kdt'
      [nnids nndists] = ccvKdtKnn(index, features, features(:,labels==1), 2);
    case 'hkm'
      [nnids nndists] = ccvHkmKnn(index, features, features(:,labels==1), 2);
    case {'lsh-l2', 'lsh-simplex'}
      [nnids nndists] = ccvLshKnn(index, features, features(:,labels==1), 2);
  end; % switch
  
  % get image with maximum number of features
  nnlabels = labels(nnids(1,:));
  counts = histc(nnlabels, 1:num_images);
  [counts cids] = sort(counts, 'descend');
  counts(1), cids(1)
  
  % destroy the index
  switch type
    case 'kdt'
      ccvKdtClean(index);
    case 'hkm'
      ccvHkmClean(index);
    case {'lsh-l2', 'lsh-simplex'}
      ccvLshClean(index);
  end; % switch
end; % for nni

% restore the seed
ccvRandSeed(old_seed, 'restore');

end % full_representation function

end % DEMO function