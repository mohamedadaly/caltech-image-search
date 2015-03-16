function dists = ccvDistance(data1, data2, dist, mex)
% CCVDISTANCE computes distance from the given point to the given data
%
% INPUTS
% ------
% data1     - input data 1
% data2     - input data 2, one point per column
% dist      - ['l2'] the distance function to use
%             'hamming' -> hamming distance
%             'l1'      -> L1 distance (city block)
%             'l2'      -> L2 euclidean distance
%             'arccos'  -> arccos distance
%             'cos'     -> cosine distance
%             'bhat'    -> Bhattatcharya Coefficient
%             'kl'      -> Symmetric Kullback-Leibler divergence
% mex       - [1] use the mex implementation or not
%
% OUTPUTS
% -------
% dists     - the output distances to every point in data1 to every point
%             in data2, with one column per point in data1
%
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010

if nargin<3 || isempty(dist),   dist = 'l2'; end;
if nargin<4 || isempty(mex),    mex = 1; end;

if isempty(data1) || isempty(data2), dists = []; return; end;

%call the mex file
if mex && exist(['mxDistance.' mexext], 'file')
  dists = mxDistance(data1, data2, dist);
%matlab implementation  
else
  [ndims1, npoints1] = size(data1);
  [ndims2, npoints2] = size(data2);
  
  dists = zeros(npoints2, npoints1);
  
  %loop on points
  for p=1:npoints1
    point = data1(:,p);
    
    %replicate the point
    pcmp = repmat(point, 1, npoints2);

    %compute distances
    switch dist
      case 'hamming', d = sum(xor(pcmp, data2), 1);
      case 'l1',      d = sum(abs(pcmp - data2), 1);
      case 'l2',      d = sqrt(sum((pcmp - data2).^2, 1));
      case {'arccos','cos'},  
        d = point' * data2;
        d2 = diag(data2'*data2);
        d = d ./ sqrt(point'*point * d2');
        if strcmp(dist,'arccos'), d = acos(d); end;
      case 'bhat',    d = -log2(sum(sqrt(pcmp .* data2),1));
      case 'kl',      
        d = sum(pcmp .* log2(pcmp ./ data2)) + sum(data2 .* log2(data2 ./ pcmp));
        d = d /2;
    end;
  end;
  %put back
  dists(:,p) = d(:);

end;
