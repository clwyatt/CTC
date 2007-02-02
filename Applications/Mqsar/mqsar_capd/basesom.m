function [stats, outclass] = basesom(data, varnames, varargin);		

% BASESOM basic SOM training
%
% [stats] = basesom(X, Bins, varnames, 'gaussian', 'sheet', 'big');
% [stats outclass] = basesom(X, Bins, varnames, 'supervised');
% 
%	Input and output arguments ([]'s are optional): 
%	data	(matrix)	training data matrix
%	bins	(vector)	training sample's categories
%	varnames(vector)	list of variable names
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
%	stats	(struct)	the statistical result
%   outclass(vector)    the calculated bins for the data
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
%	maptype		*(string)	Specifies the som maptype, default is 'big', options: 'big', 'normal', 'small'
%	neighbor	*(string)	Specifies the som neighbor, default is 'gaussian,
%							Options:	'gaussian' 'cutgauss' 'ep' 'bubble'
%	shapes		*(string)	Specifies the som shapes, default is 'sheet', 
%							Options:	'sheet', 'cyl', 'toroid'
%	bins		*(string)	specifies the binnings by following array
%	supervised	*(string)	Specifies supervised training SOM
%	unsupervised*(string)	Spedifies unsupervised SOM trianing

%
% Copyright 2003, Yun-De Xiao

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Ersin: commented out rand state seeding
%rand('state',sum(100*clock));	%re-seed random number generator
warning off;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% default values
maptype			= 'big';		% 'big' 'normal' 'small'
neighbor		= 'gaussian';	% 'gaussian' 'cutgauss' 'ep' 'bubble'
shapes			= 'sheet';		% 'sheet' 'cyl' 'toroid'

% default values
supervised = 'supervised';
bins = [];

%% Check arguments
i=1;
while i<=size(varargin,2), 
	argok = 1;
	if ischar(varargin{i}), 
		switch varargin{i}, 
			% unambiguous values
			case {'gaussian', 'cutgauss', 'ep', 'bubble'},
				neighbor = varargin{i};
			case {'sheet', 'cyl', 'toroid'},
				shapes = varargin{i};
			case {'big', 'normal', 'small'},
				maptype = varargin{i},
			case {'supervised'}, supervised = varargin{i};
			case {'unsupervised'}, supervised = varargin{i};
			case {'bins'}, i = i + 1; bins = varargin{i};
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(basesom) Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end

% bindif     = max(bins) - min(bins);
bindif = 2;

if isempty(bins)
	bins = [1:size(data, 1)];
	bins = bins.';
end

% build and normalize som data structures (sD)
sD = som_data_struct(data, 'name', 'Input Data Set', 'comp_names',	varnames, ...
		'labels',		cellstr(num2str(bins)));
sD = som_normalize(sD,'var');			% normalize variance to unity
sD = som_normalize(sD,'logistic');		% normalize range to logistic fxn

switch(supervised)
	case 'supervised'
		sM= som_supervised(sD, 'mapsize', maptype, 'tracking', 0, 'shape', shapes, 'neigh', neighbor);
		mapclass = str2num(char(sM.labels));

		% gather statistics on the model
		class = getsomquality(sM, sD, mapclass);
		outclass = class;
		[qe, te] = som_quality(sM, sD);  % qe: mean quantization error; te: topographic error
		stats = calcstats(class, bins, bindif, size(sD.data,2), qe, te);
		if isnan(stats.false_pos)
			stats.false_pos = 0;
		end
		if isnan(stats.false_neg)
			stats.false_neg = 0;
		end

	case 'unsupervised'
		som_gui(sD)
end


% --------------------------------------------
% Get the statistic result
function [qual] = calcstats(calc_bins, bins, bindif, numcomps, qe, te);

qual = struct('correct_rate',   sum(bins==calc_bins)/length(bins), ...
		'varnum',   numcomps, ...
		'false_pos', sum((bins-calc_bins-bindif)>=0)/length(bins), ...
		'false_neg', sum((bins-calc_bins+bindif)<=0)/length(bins), ...
		'quantiz_err', qe, 'topog_err', te);

% --------------------------------------------
% Excutes to find the category of data (sD)
function [results] = getsomquality(sM, sD, class);

for i=1:size(sD.data,1)
	results(i,1) = class(find(som_hits(sM, sD.data(i,:), 'crisp')));
end

