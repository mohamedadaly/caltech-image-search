function dwords = ccvBowGetWords(words, data, locs, gws)
% CCVBOWGETWORDS computes word quantizations for the input data using the 
% input words computed from ccvBowGetDict
% 
% INPUTS
% ------
% words   - the words used to create the dictionary, input to ccvBowGetDict
% data    - the input data to get words for, one point per column [ndimsXnpoints]
% locs    - the locations for the data points [2Xnpoints]
% gws     - the GetWords strcture to use, output from ccvBowGetWordsInit
%
% OUTPUTS
% -------
% dwords   - the output words for the input data
%
% See also ccvBowGetDict
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010


switch gws.type
  %flat dictionary
  case 'flat'
    %cluster 
    dwords = getWords(words, data, gws.dict, gws.cluster);
    
  case 'lsh-multi'
    %get bucket ids
    dwords = getWords(words, data, gws.dict, gws.cluster);
    %now dwords is ntablesXnpoints matrix, mod by singlek and concat
    dwords = mod(double(dwords)-1, words.tparams.singlek)+1;
    dwords = dwords + repmat(cumsum([0; words.tparams.singlek*ones(size(dwords,1)-1,1)]), 1, size(dwords,2));
    dwords = uint32(dwords(:));
    
  %multiple
  case 'multiple'
    
    
  %per-class
  case 'per-class'
    %loop and get words
    dwords = cell(1, length(gws.dict));
    for c=1:length(gws.dict)
      dwords{c} = getWords(words, data, gws.dict{c}, gws.cluster); 
    end;
    dwords = cell2mat(dwords);
    

end; %switch type



% -----------------------------------------------------------------------
% clusters the input data
function w = getWords(words, data, dict, cluster)

switch cluster
  case 'akmeans'
    w = ccvKdtKnn(dict, words, data, 1);
    
  case 'lsh'
    %get the bucket ids
    w = ccvLshBucketId(dict.lsh, data);
    
%     %get ids of inputs if there, otherwise get closest one
%     nin = find(ismember(w, dict.ids)==0);
%     for i=nin(:)'
%       ii = find(dict.ids>w(i), 1, 'first');
%       if isempty(ii), ii = length(dict.ids); end;
%       w(i) = dict.ids(ii);
%     end;    
%     
%     %take mod the number of words
%     w = uint32(mod(double(w), dict.nwords) + 1);

  case 'xkmeans'
    %compute signatures for data
    rsig = eval(sprintf('%s(ccvLshBucketId(dict.lsh, data));',dict.class));
    datasig = zeros(1, size(data,2), dict.class);
    for i=1:dict.ndir
      datasig = bitor(bitshift(datasig,1), rsig(i,:));
    end;
    
    %get nearest mean: words
    w = ccvKnn(datasig, words.meanssig,1,'xor');
        
  case 'hkmeans'
    %get the leaf ids
    w = ccvHkmLeafIds(dict, data);
    
   %convert to column vector
   w = w(:);
end; %switch cluster
