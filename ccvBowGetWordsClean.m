function ccvBowGetWordsClean(gws)
% CCVBOWGETWORDSCLEAN cleans memory after computing word quantizations
% 
% INPUTS
% ------
% gws     - the GetWords strcture to use, output from ccvBowGetWordsInit
%
% OUTPUTS
% -------
%
% See also ccvBowGetDict
%

% Author: Mohamed Aly <malaa at vision d0t caltech d0t edu>
% Date: October 6, 2010


switch gws.type
  %flat dictionary
  case 'flat'
    %cluster 
    cleanDict(gws.dict, gws.cluster);
    
  %per-class
  case 'per-class'
    for c=1:length(gws.dict), cleanDict(gws.dict{c}, gws.cluster); end;
      
    

end; %switch type




% -----------------------------------------------------------------------
% clusters the input data
function cleanDict(dict, cluster)

switch cluster
  case 'akmeans'
    ccvKdtClean(dict);    
    
  case 'lsh'
    ccvLshClean(dict.lsh);
    
  case 'xkmeans'
    ccvLshClean(dict.lsh);
    
  case 'hkmeans'
    ccvHkmClean(dict);
    
end; %switch cluster
