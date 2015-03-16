function akmeans = ccvAkmeansClean(akmeans)
% CCVAKMEANSCLEAN clears the kdtree within the akmeans structure
%
% Inputs:
% -------
% akmeans     - the input akmeans structure
% 
% Outputs:
% --------
% akmeans     - the return structure
%
% See also CCVAKMEANSLOOKUP, CCVAKMEANSCREATE
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

%clear the kdtree
switch akmeans.type
  case 'kdt',     ccvKdtClean(akmeans.kdt);
  case 'flann',   flann_free_index(akmeans.kdt.flann);
end;