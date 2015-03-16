function ccvInvFileSave(ivf, file)
% CCVINVFILESAVE saves the structure to disk
%
% INPUTS
% ------
% ivf       - the input inverted file
% file      - the file name
%
% OUTPUTS
% -------
%
% See also ccvInvFileCompStats ccvInvFileLoad ccvInvFileInsert
% ccvInvFileClean ccvInvFileSearch
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%call the mex file
mxInvFileSave(ivf, file);

