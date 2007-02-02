
function [qual] = modelstats(calc_bins, bins, bindif);
% --------------------------------------------
% Get the statistic result

qual = struct('correct_rate',   sum(bins==calc_bins)/length(bins), ...
		'false_pos', sum((bins-calc_bins-bindif)>=0)/length(bins), ...
		'false_neg', sum((bins-calc_bins+bindif)<=0)/length(bins));