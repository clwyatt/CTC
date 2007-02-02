
function [qual] = modelstats_test(resultsTst, binTst, resultsTrn, binTrn, bindif, numcomps);
% Get the statistic result with test set
    qual = struct('test_correct_rate',    sum(binTst==resultsTst)/length(binTst), ...
		'correct_rate',   sum(binTrn==resultsTrn)/length(binTrn), ...
		'test_false_pos',  sum((binTst-resultsTst-bindif)>=0)/length(binTst), ...
		'test_false_neg',  sum((binTst-resultsTst+bindif)<=0)/length(binTst), ...
		'false_pos', sum((binTrn-resultsTrn-bindif)>=0)/length(binTrn), ...
		'false_neg', sum((binTrn-resultsTrn+bindif)<=0)/length(binTrn));