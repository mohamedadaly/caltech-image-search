function [stats, meanStats] = ccvLshStats(lsh)
% CCVLSHSTATS returns stats for the input LSH index
%
% INPUTS
% ------
% lsh       - the input lsh
%
% OUTPUTS
% -------
% stats     - the stats for the LSH index, a cell array one per table
%             .nFullBuckets     -> the number of full buckets
%             .meanBucketSize   -> the mean bucket size
%             .medianBucketSize -> median
%             .stdBucketSize    -> the std deviation
%             .meanCollision    -> the number of full bucket lists
% meanStats - the mean of stats over the tables
%
% See also ccvLshClean ccvLshCreate ccvLshInsert ccvLshLoad ccvLshSave
% ccvLshSearch ccvLshBucketId ccvLshFuncVal ccvLshBucketPoints ccvLshKnn
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%call the mex file
stats = mxLshStats(lsh);

%get mean
meanStats = struct('nFullBuckets', mean([stats.nFullBuckets]), ...
  'meanBucketSize', mean([stats.meanBucketSize]), ...
  'medianBucketSize', mean([stats.medianBucketSize]), ...
  'sdBucketSize', mean([stats.stdBucketSize]), ...
  'meanCollision', mean([stats.meanCollision]));

