function norm = ccvNorm(data, type, mx)
% CCVNORM returns the norm of the input data
% 
% INPUTS
% ------
% data    - the input data, with one column per example
% type    - ['l2'] the normalization type
%           'l1'  - L1 normalization
%           'l2'  - L2 normalization
%           
% 
% OUTPUTS
% -------
% ndata   - the normalized data
% 
% See also ccvNormalize

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if nargin<2, type = 'l2'; end;
if ~exist('mx','var'), mx = 1; end;

if mx && exist(['mxNorm.' mexext], 'file')
  norm = mxNorm(data, type);
  return;
end;

switch type
  case 'l1'
    norm = sum(data,1);
    
  case 'l2'
    norm = sqrt(sum(data.^2,1));
    
  otherwise
    error(sprintf('unknown norm type: %s', type));
end;

