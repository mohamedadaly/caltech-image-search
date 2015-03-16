function hkm = ccvHkmImport(mhkm)
% CCVHKMIMPORT imports the input matlab hkm structure (output of
% ccvHkmExport) into an hkm structure that is input to the rest of ccvHkm*
% functions
%
% INPUTS
% ------
% nhkm      - the input matlab structure, output of ccvHkmExport
%
% OUTPUTS
% -------
% hkm       - the output hkm structure
%
% See also ccvHkmExport ccvHkmCreate ccvHkmClean ccvHkmKnn ccvHkmClean
%   ccvHkmLeafIds
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%call the mex file
hkm = mxHkmImport(mhkm);

