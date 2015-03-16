function old = ccvRandSeed(seed, op, type)
% CCVRANDSEED will set/restore the seed for the defeault random stream
% 
% INPUTS
% ------
% seed  - the required seed to set, or the old seed
% op    - the required operation
%         'set' -> sets the seed and type 
%         'restore -> restores the old value from previous call to
%           ccvRandSeed, where this value is in 'seed' input
% type  - the type of generator to set as default, look at
%         RandStream.create for details
%       {'mt19937ar'}
%
% OUTPUTS
% -------
% old   - the old seed
%
% See also 
%

if ~exist('type','var') || isempty(type), type = 'mt19937ar'; end;

switch op
  case 'set'
    old = RandStream.setDefaultStream(RandStream.create(type,'Seed',seed));
  case 'resotre'
    RandStream.setDefaultStream(seed);
end;
