function lids = ccvHkmLeafIds(hkm, data)
% CCVHKMLEAFIDS returns the leaf ids for the input data. The leaf ids range
% from 1 -> nlevels^nbranches of the HKM, and are used for generating
% visual words (clustering in general)
%
% INPUTS
% ------
% hkm       - the input hkm
% data      - the input data
%
% OUTPUTS
% -------
% lids       - the output leaf ids
%
% See also ccvHkmImport ccvHkmExport ccvHkmClean ccvHkmCreate ccvHkmClean
%   ccvHkmKnn
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%call the mex file
lids = mxHkmLeafIds(hkm, data);

