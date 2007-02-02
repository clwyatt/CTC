function [varnamesel, stats, outclass] = stepwisesom(data, bioact, bins, varnames, varargin);		

% STEPWISESOM SOM step-wise regression algorithm, similar to MLR
%
% [varsel, stats] = stepwise(X, Bins, cmpnames, varnames, 'gaussian', 'sheet', 'big');
% 
%	Input and output arguments ([]'s are optional): 
%	data	(matrix)	training data matrix
%	bins	(vector)	training sample's categories
%	bioact	(vector)	biological activities of compounds
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
%	maptype		*(string)	Specifies the som maptype, default is 'big', options: 'big', 'normal', 'small'
%	neighbor	*(string)	Specifies the som neighbor, default is 'gaussian,
%							Options:	'gaussian' 'cutgauss' 'ep' 'bubble'
%	shapes		*(string)	Specifies the som shapes, default is 'sheet', 
%							Options:	'sheet', 'cyl', 'toroid'

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
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(stepwisesom) Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end

bindif     = max(bins) - min(bins);

% build and normalize som data structures (sD)
sD = som_data_struct(data, 'name', 'Input Data Set', 'comp_names',	varnames, ...
		'labels',		cellstr(num2str(bins)));
sD = som_normalize(sD,'var');			%normalize variance to unity
sD = som_normalize(sD,'logistic');		%normalize range to logistic fxn

% choose basis descriptor for regression model, based on linear correlation
A = corrcoef([bioact sD.data]);
[maxcorr, ind] = max(abs(A(1,2:end)));

% build basis som data structures accordingly
basis = som_data_struct(sD.data(:,ind), 'name', 'Basis Set', 'comp_names', varnames(ind), ...
			'labels', cellstr(num2str(bins)), 'comp_norm', {sD.comp_norm{ind}});

% remove selected descriptor from sD
varnames = [varnames(1:ind-1); varnames(ind+1:end)];
sD.data(:,ind)     = [];
sD.comp_names(ind) = [];
sD.comp_norm(ind)  = [];

% build SOM from basis
sM= som_supervised(basis, 'mapsize', maptype, 'tracking', 0, 'shape', shapes, 'neigh', neighbor);
mapclass = str2num(char(sM.labels));

% gather basis statistics on initial regression model
class = getsomquality(sM, basis, mapclass);
outclass = class;
[qe, te] = som_quality(sM, basis);  % qe: mean quantization error; te: topographic error
stats = calcstats(class, bins, bindif, size(basis.data,2), qe, te);

clear A;

% --------------------------------------------------------------------
% forward-entry (of stepwise regression)
% generate random order to insert pairs of descriptors
order = randperm(size(sD.data,2));

for nVar=1:size(sD.data,2)
    addFlag = 0;    % if add new variable in the training, set the flag on (1) 
    % 2. create temp data structs including the pair in question
	newpartdata = sD.data(:,order(nVar));
	newpartvarname  = varnames(order(nVar));
	newpartnorm = [sD.comp_norm(order(nVar))];
		
	tempbasis = som_data_struct([basis.data newpartdata], 'name', 'Basis Set', ...
                'comp_names', [basis.comp_names; newpartvarname], ...
                'labels', cellstr(num2str(bins)), ...
                'comp_norm', [basis.comp_norm; newpartnorm]);

    % 3. build SOM with new train data
	sM    = som_supervised(tempbasis, 'mapsize', maptype, 'tracking',0, 'shape', shapes, 'neigh', neighbor);
	mapclass = str2num(char(sM.labels));
    
    % calculate som statistics for the temp dataset
	class = getsomquality(sM, tempbasis, mapclass);
	[qe, te] = som_quality(sM,tempbasis);
	tstats  = calcstats(class, bins, bindif, size(tempbasis.data,2), qe, te);

    % decide to include or discard pair in question
    if (tstats.correct_rate > stats.correct_rate)
		basis    = tempbasis;
		stats = tstats;
		addFlag = 1;
        outclass = class;
    end
    
	% backwards-removal (of stepwise regression)



%		%% 5. decide to remove or keep descriptor in question
%
	if addFlag      % if add any descriptor in, then try to remove any old out
        bRepeat = 1;
        while bRepeat
            bRepeat = 0;
            % generate random order to remove descriptors
        	rorder  = randperm(size(basis.data,2));
        	rmsteps = size(basis.data,2);

            for j=1:rmsteps
			    % create temp train data structs excluding descriptor in question
    			mask = ones(size(basis.data,2),1);
    			mask(rorder(j)) = 0;
    			tempbasis = som_data_struct([basis.data(:,find(mask))], ...
							'name',	'Basis Set', ...
							'comp_names',	[basis.comp_names(find(mask))], ...
							'labels',		cellstr(num2str(bins)), ...
		    				'comp_norm',	[basis.comp_norm(find(mask))]);

                % build SOM with temp new data
    			sM    = som_supervised(tempbasis, 'mapsize', maptype, 'tracking',0, 'shape', shapes, 'neigh', neighbor);
    			mapclass = str2num(char(sM.labels));

    			class = getsomquality(sM, tempbasis, mapclass);
    			[qe, te] = som_quality(sM, tempbasis);
    			tstats  = calcstats(class, bins, bindif, size(tempbasis.data,2), qe, te);

    			% decide to remove or kep descriptor in question
    			if (tstats.correct_rate > stats.correct_rate)
				    basis = tempbasis;
    				stats = tstats;
                    outclass = class;
                    bRepeat = 1;
                    break;
                end
            end
        end
    end
end
varnamesel = basis.comp_names;
if isnan(stats.pos_fail)
    stats.pos_fail = 0;
end
if isnan(stats.neg_fail)
    stats.neg_fail = 0;
end

% --------------------------------------------
% Get the statistic result
function [qual] = calcstats(calc_bins, bins, bindif, numcomps, qe, te);

qual = struct('correct_rate',   sum(bins==calc_bins)/length(bins), ...
		'varnum',   numcomps, ...
		'pos_fail', sum((bins-calc_bins-bindif)>=0)/sum(xor((bins - calc_bins),0)), ...
		'neg_fail', sum((bins-calc_bins+bindif)<=0)/sum(xor((bins - calc_bins),0)), ...
		'quantiz_err', qe, 'topog_err', te);

% --------------------------------------------
% Excutes to find the category of data (sD)
function [results] = getsomquality(sM, sD, class);

for i=1:size(sD.data,1)
	results(i,1) = class(find(som_hits(sM, sD.data(i,:), 'crisp')));
end

