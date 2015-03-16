function mhkm = ccvHkmExport(hkm, getids)
% CCVHKMEXPORT exports the input hkm structure to matlab, so that it can be
% saved to disk or manipulated
%
% INPUTS
% ------
% hkm       - the input hkm
% getids    - [1] return the ids (used for ccvHkmKnn) or just the 
%             means (used for ccvHkmLeafIds)
%
% OUTPUTS
% -------
% nhkm       - the output matlab structure
%
% See also ccvHkmImport ccvHkmCreate ccvHkmClean ccvHkmKnn ccvHkmClean
%   ccvHkmLeafIds
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%defaults
if ~exist('getids','var') || isempty(getids), getids = 1; end;
   
%call the mex file
mhkm = mxHkmExport(hkm, getids);

