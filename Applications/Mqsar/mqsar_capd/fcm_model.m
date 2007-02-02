function [TrPrd, TstPrd,Member_matrix,Dcenter, stats] = fcm_model(tmpX,tmpBins,tmpXTest, TmpBinsTest,Classes,Supervised);
%  Fuzzy c-means based modeling
%
% [Member_matrix, class] = fcm_model(tmpX,tmpBins,genome,userdata.ClassBins,userdata.Cluster)
%
%	Input and output arguments ([]'s are optional): 
%	tmpX	(matrix)	training data matrix
%	tmpBins	(vector)	training sample's categories
%   genome  (vector)    binary string 1's indicate the selected descriptors
%                       to be included in the model 
%	bioact	(vector)	biological activities of compounds
%   ClassBins
%
%	varnames(vector)	list of variable names
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
%	varsel	(vector)	list of selected variables by stepwise
%	stats	(struct)	the statistical result
%   outclass(vector)    the calculated bins for the data
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
% Copyright 2003, Ersin Bayram
%==============================================================


Datain = [tmpX;tmpXTest];
  
[M,N]  =size(Datain);
Dcenter = zeros(Classes,N);
Dbins = zeros(M,1);
[Dcenter,DU,Dobj_fcn] = fcm(Datain,Classes);
maxDU = max(DU);
for i=1:Classes
    index = find(DU(i,:) == max(DU));
    Dbins(index) = i;
end;     

if Supervised 
   %Datain = tmpX;
   Rbins = tmpBins;
   Mbins = Dbins(1:length(tmpBins));
else
   Rbins = [tmpBins;TmpBinsTest];
   Mbins = Dbins;
end;  

Dmaps = perms(1:Classes);
BestMatch = 0;
for i=1:size(Dmaps,1)
    Matches =0;
    for j=1:Classes
        Matches = Matches + sum((Rbins==j).*(Mbins==Dmaps(i,j)));
    end;
    if Matches > BestMatch
       Mapind = i;
       BestMatch = Matches;
    end;
end;    
 
Obins = Dbins;
for i=1:Classes
    Obins(Dbins==Dmaps(Mapind,i)) = i;
end;

TrPrd = Obins(1:length(tmpBins));
TstPrd = Obins(length(tmpBins)+1:end);

Member_matrix= DU;

bindif = 2;
    
stats = struct('test_correct_rate',    sum(TmpBinsTest==TstPrd)/length(TmpBinsTest), ...
		'correct_rate',   sum(tmpBins==TrPrd)/length(tmpBins), ...
		'test_false_pos',  sum((TmpBinsTest-TstPrd-bindif)>=0)/length(TmpBinsTest), ...
		'test_false_neg',  sum((TmpBinsTest-TstPrd+bindif)<=0)/length(TmpBinsTest), ...
		'varnum',   N, ...
        'false_pos', sum((tmpBins-TrPrd-bindif)>=0)/length(tmpBins), ...
		'false_neg', sum((tmpBins-TrPrd+bindif)<=0)/length(tmpBins), ...
        'CMap',Dmaps(Mapind,:));   
        