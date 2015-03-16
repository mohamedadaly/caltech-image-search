function [rids, dists, inliers] = ccvBowSpCheck(testLocs, testWords, ...
  trainLocs, trainWords, ids, params)
% CCVBOWSPCHECK performs a spatial check on the input images with their
% nearest neighbors
% 
% INPUTS
% ------
% testLocs    - the locations of features in the test image
% testWords   - the words for the features in the test image
% trainLocs   - cell array of locations in training images
% trainWords  - cell array of words in training images
% ids         - ids of images in train to consider
% params      - the extra parameters passed to ccvRansacAffine
%
% OUTPUTS
% -------
% rids        - the sorted list of ids
% dists       - mean of distances of inliers
% inliers     - number of inliers
%
% See also ccvRansacAffine
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

nids = length(ids);

%init dists with Inf
dists = Inf*ones(1, nids);
inliers = zeros(1, nids);

%loop on the train images
for t=1:nids
  id = ids(t);
  if id==0, continue; end;
  
  %get matches
  matches = [];
  %get common words
  cws = intersect(testWords, trainWords{id}); cws = cws(:)';
  %loop on common words
  for w=cws
    [ii,jj] = meshgrid(find(testWords==w), find(trainWords{id}==w));
    matches = [matches [ii(:)'; jj(:)']];
  end;
  
  %check if we have enough matches
  if size(matches,2)<params{3}, continue; end;
      
  %fit affine transformation
  [h, score, ninliers, inind] = ccvRansacAffine(testLocs, trainLocs{id}, matches, params{:});
  
  %get rid of duplicates
%   ninliers = length(unique(matches(1,inind)));
  
  %save score
  if ~isempty(h)
    dists(t) = score;
    inliers(t) = ninliers;
  end;
end; %for t

%sort based on mean distance
% [dists, ii] = sort(dists, 'ascend');
% inliers = inliers(ii);
[inliers ii] = sort(inliers, 'descend');
dists = dists(ii);
rids = ids(ii);


