function ndata = ccvNormalize(data, type, mx)
% CCVNORMALIZE normalizes the input data with the specified type
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
% See also ccvNorm

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if nargin<2, type = 'l2'; end;
if ~exist('mx','var'), mx = 1; end;

if mx && exist(['mxNormalize.' mexext], 'file')
  ndata = mxNormalize(data, type);
  return;
end;

switch type
  case 'l1'
    ndata = data ./ repmat(sum(data,1), size(data,1), 1);
    
  case 'l2'
    ndata = data ./ repmat(sqrt(sum(data.^2,1)), size(data,1), 1);
    
  otherwise
    ndata = data;
end;

