function ccvHkmClean(hkm)
% CCVHKMCLEAN clears the memory for teh input hkm
%
% INPUTS
% ------
% hkm       - the input hkm, which comes from ccvHkmCrate or ccvHkmImport
%
% OUTPUTS
% -------
%
% See also ccvHkmImport ccvHkmExport ccvHkmCreate ccvHkmKnn ccvHkmClean
%   ccvHkmLeafIds
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%call the mex file
mxHkmClean(hkm);


