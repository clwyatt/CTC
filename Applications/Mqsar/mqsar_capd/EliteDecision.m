function Decision = EliteDecision(Results, Weights, bins, bindif, binnum);

%
%   Input arguments
%   Results  (struc)    Structure to hold info on the final models.
%                       Default:   10x1. Elements are as follows:
%	          varsel    (vector)   list of selected variables by GA
%	          stats	    (struct)   the statistical result
%             outclass  (vector)   the calculated bins for the data
%
%   Weights  (Vector)   Weights for the top 10 models which will decide on
%                       the final class
%	bins	(vector)	training sample's categories
%	bindif  (scalar)    bin difference to btwn assigned and actual bin for
%	                    false postive calculations
%   binnum  (scalar)    Total number of bins for classification 

 
ResultSize = length(Weights);
NumComp = length(bins); 
Class = zeros(NumComp,ResultSize);
Votes = zeros(binnum,NumComp);


for i=1:NumComp
	for j = 1: ResultSize
        Votes(Results(j).class, i) = Votes(Results(j).class, i) + Weights(Results(j).class);
    end;
end;    
        
        Class(:,j) = Results(j).class;
        Correct(j) = Results(j).stats.correct_rate
    
    

qual = struct('test_correct_rate',    sum(binTst==resultsTst)/length(binTst), ...
		'correct_rate',   sum(binTrn==resultsTrn)/length(binTrn), ...
		'test_false_pos',  sum((binTst-resultsTst-bindif)>=0)/length(binTst), ...
		'test_false_neg',  sum((binTst-resultsTst+bindif)<=0)/length(binTst), ...
		'varnum',   numcomps, ...
		'false_pos', sum((binTrn-resultsTrn-bindif)>=0)/length(binTrn), ...
		'false_neg', sum((binTrn-resultsTrn+bindif)<=0)/length(binTrn), ..tempgenome(:,Ind(end-i+1).
		'quantiz_err', qe, 'topog_err', te);