function ivf = ccvInvFileExtraInsert(ivf, data, nwords, idshift)
% CCVINVFILEEXTRAINSERT inserts data to an inverted file structure
%
% INPUTS
% ------
% ivf       - [0] the input inverted file created before, or 0 to create a
%             new one
% data      - the input data, a cell array with one vector per document
%             containing the word ids of its content
% nwords    - the number of words
% idshift   - [0] a value to add to the ids, in case of filling in more
%             than one call
%
% OUTPUTS
% -------
% ivf       - the output inverted file
%
% See also ccvInvFileCompStats ccvInvFileLoad ccvInvFileSave
% ccvInvFileClean ccvInvFileSearch
%
 
% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if ~exist('ivf','var') || isempty(ivf), ivf = 0; end;
if ~exist('idshift','var') || isempty(idshift), idshift = 0; end;

%call the mex file
ivf = mxInvFileExtraFill(ivf, data, nwords, idshift);

