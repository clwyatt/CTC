function [colnames, xdata] = qsarfilter(varnames, xmat, corr_thres, std_thres);
% QSAR/QSPR data filter routine. 
% This program will exclude the "user specified" columns, redundancies in terms of hgihly 
% correlated descriptors, and also has a covariance filter. 
%
% [colnames, xdata] = qsarfilter(varnames, xmat, varargin);
%
% [colnames, xdata] = qsarfilter(varnames, xmat, 'ex', 3); 
%
% Input and output arguments
%
% colnames	 :	Column headers, 1st row of the file, excluding Y block title,
%				bin and redundant column name.
% xdata		 :	X block numeric data part of the file.
% corr_thres :  Threhold for correlation based redundant descriptor removal
%               Default: 1.0 -> no removal 
% std_thres  :  Threhold for standard deviation based useless descriptor removal
%                Default: 0.0 -> no removal 
%
% Copyright 2004	Ersin Bayram
%---------------------------------------------------------------

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% check arguments


delIndex = [];
% Eliminate the useless variables due to lack to variation in the numerical
% values
datastd = std(xmat);
delIndex = find(datastd <= std_thres);
xmat(:, delIndex) = [];
varnames(delIndex) = [];

% Correlation based filtering
r = corrcoef(xmat);
[fPart, bPart] = find(abs(r) >= sqrt(corr_thres));	% find significant correlations
ind = find(fPart < bPart);
dIndex = bPart(ind);
dIndex = unique(dIndex);
xmat(:, dIndex) = [];
varnames(dIndex) = [];

xdata = xmat;
colnames = varnames;