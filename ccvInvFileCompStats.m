function ccvInvFileCompStats(ivf, weight, norm)
% CCVINVFILECOMPSTATS computes stats for the input inverted file, so that
% it's ready for search. This function MUST be called after inserting any
% new data, and BEFORE calling ccvInvFileSearch
%
% INPUTS
% ------
% ivf       - the input inverted file created before
% weight    - ['none'] the weighting to use
%             'none'  -> don't weight counts values
%             'bin'   -> binarize the counts
%             'tf'    -> term frequency, divide each count by the number of 
%                         words in the doc
%             'tfidf' -> use tf-idf weighting
% norm      - ['none'] the normalization to use
%             'none' -> no normalization
%             'l0'   -> normalize by the number of words
%             'l1'   -> normalize so that the histogram sum = 1
%             'l2'   -> normalize so that histogram sum square = 1
%
% OUTPUTS
% -------
%
% See also ccvInvFileInsert ccvInvFileLoad ccvInvFileSave
% ccvInvFileClean ccvInvFileSearch
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if ~exist('weight','var') || isempty(weight), weight = 'none'; end;
if ~exist('norm','var') || isempty(norm), norm = 'none'; end;

%call the mex file
mxInvFileCompStats(ivf, weight, norm);

