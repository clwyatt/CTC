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
%	test		*(string)	Specifies the test set informat (test data set and bins)

%
% Copyright 2003, Yun-De Xiao

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%rand('state',sum(100*clock));	%re-seed random number generator
warning off;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% default values
maptype			= 'big';		% 'big' 'normal' 'small'
neighbor		= 'gaussian';	% 'gaussian' 'cutgauss' 'ep' 'bubble'
shapes			= 'sheet';		% 'sheet' 'cyl' 'toroid'
testbins = [];
tdrive = [];        % 9/23/03   YX add for test set driving

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
			case {'test'},
				i = i+ 1;
				tData = varargin{i};
				i = i + 1;
				testbins = varargin{i};
            case {'tdrive'},
                tdrive = varargin{i};
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(stepwisesom) Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end

%bindif     = max(bins) - min(bins);
bindif = 2;

% build and normalize som data structures (sD)
sD = som_data_struct(data, 'name', 'Input Data Set', 'comp_names',	varnames, ...
		'labels',		cellstr(num2str(bins)));
sD = som_normalize(sD,'var');			%normalize variance to unity
sD = som_normalize(sD,'logistic');		%normalize range to logistic fxn
tvarnames = varnames;

if ~isempty(testbins)	% build and normalize som data structure for test set(tD)
	tD = som_data_struct(tData, 'name', 'Data to Test', 'comp_names', varnames, ...
			'labels', cellstr(num2str(testbins)));
	for i=1:size(sD.data,2)
		tD.data(:, i) = som_norm_variable(tD.data(:,i), sD.comp_norm(i), 'do');
		tD.comp_norm{i} = sD.comp_norm{i};
	end
end

% YX 9/22/03 Add for classic sasom
if isempty(tdrive)      % YX 9/23/03 add for test set driving
    tTmpBins = testbins;
    testbins = [];
else
    tTmpBins = [];
end
% end 9/22/03

% choose basis descriptor for regression model, based on linear correlation
A = corrcoef([bioact sD.data]);
[maxcorr, ind] = max(abs(A(1,2:end)));

% build basis som data structures accordingly
basis = som_data_struct(sD.data(:,ind), 'name', 'Basis Set', 'comp_names', varnames(ind), ...
			'labels', cellstr(num2str(bins)), 'comp_norm', {sD.comp_norm{ind}});
if ~isempty(testbins)
	tBasis = som_data_struct(tD.data(:,ind), 'name', 'Basis Test Set', 'comp_names', varnames(ind), ...
			'labels', cellstr(num2str(testbins)), 'comp_norm', {sD.comp_norm{ind}});
end

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
if isempty(testbins)
	stats = calcstats(class, bins, bindif, size(basis.data,2), qe, te);
else
	tclass = getsomquality(sM, tBasis, mapclass);
	stats = calcstats1(tclass, testbins, class, bins, bindif, size(basis.data,2), qe, te);
end

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

	if ~isempty(testbins)
		tnewpartdata = tD.data(:, order(nVar));
		ttmpbasis = som_data_struct([tBasis.data tnewpartdata], 'name', 'Basis Test Set', ...
				'comp_names', [basis.comp_names; newpartvarname], ...
                'labels', cellstr(num2str(testbins)), ...
                'comp_norm', [basis.comp_norm; newpartnorm]);
	end

    % 3. build SOM with new train data
	sM    = som_supervised(tempbasis, 'mapsize', maptype, 'tracking',0, 'shape', shapes, 'neigh', neighbor);
	mapclass = str2num(char(sM.labels));
    
    % calculate som statistics for the temp dataset
	class = getsomquality(sM, tempbasis, mapclass);
	[qe, te] = som_quality(sM,tempbasis);
	if isempty(testbins)
		tstats = calcstats(class, bins, bindif, size(tempbasis.data,2), qe, te);
	else
		tclass = getsomquality(sM, ttmpbasis, mapclass);
		tstats = calcstats1(tclass, testbins, class, bins, bindif, size(tempbasis.data,2), qe, te);
	end

    % decide to include or discard pair in question
	if isempty(testbins)
		if (tstats.correct_rate > stats.correct_rate)
			basis    = tempbasis;
			stats = tstats;
			addFlag = 1;
			outclass = class;
		end
	else
		if(tstats.test_correct_rate > stats.test_correct_rate)
			basis    = tempbasis;
			tBasis = ttmpbasis;
			stats = tstats;
			addFlag = 1;
			outclass = class;
		end
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
			if length(find(mask)) == 1
				continue;
			end
    			mask(rorder(j)) = 0;
    			tempbasis = som_data_struct(basis.data(:,find(mask)), ...
							'name',	'Basis Set', ...
							'comp_names',	basis.comp_names(find(mask)), ...
							'labels',		cellstr(num2str(bins)), ...
		    				'comp_norm',	basis.comp_norm(find(mask)));
				if ~isempty(testbins)
					ttmpbasis = som_data_struct(tBasis.data(:,find(mask)), ...
							'name',	'Basis Test Set', ...
							'comp_names',	basis.comp_names(find(mask)), ...
							'labels',		cellstr(num2str(testbins)), ...
		    				'comp_norm',	basis.comp_norm(find(mask)));
				end

                % build SOM with temp new data
                %save tempbasis
    			sM    = som_supervised(tempbasis, 'mapsize', maptype, 'tracking',0, 'shape', shapes, 'neigh', neighbor);
    			mapclass = str2num(char(sM.labels));

    			class = getsomquality(sM, tempbasis, mapclass);
    			[qe, te] = som_quality(sM, tempbasis);
    			
    			% decide to remove or kep descriptor in question
				if isempty(testbins)
					tstats = calcstats(class, bins, bindif, size(tempbasis.data,2), qe, te);
					if (tstats.correct_rate > stats.correct_rate)
						basis    = tempbasis;
						stats = tstats;
						outclass = class;
						bRepeat = 1;
						break;
					end
				else
					tclass = getsomquality(sM, ttmpbasis, mapclass);
					tstats = calcstats1(tclass, testbins, class, bins, bindif, size(tempbasis.data,2), qe, te);
					if(tstats.test_correct_rate > stats.test_correct_rate)
						basis    = tempbasis;
						tBasis = ttmpbasis;
						stats = tstats;
						bRepeat = 1;
						outclass = class;
						break;
					end
				end
            end
        end
    end
end

% YX 9/22/03 Add for classic sasom
if ~isempty(tTmpBins)
    tbins = tTmpBins;
    % build and normalize som data structures (sD)
    sD = som_data_struct(data, 'name', 'Input Data Set', 'comp_names',	tvarnames, ...
		'labels',		cellstr(num2str(bins)));
    sD = som_normalize(sD,'var');			%normalize variance to unity
    sD = som_normalize(sD,'logistic');		%normalize range to logistic fxn

	tD = som_data_struct(tData, 'name', 'Data to Test', 'comp_names', tvarnames, ...
			'labels', cellstr(num2str(tbins)));
	for i=1:size(sD.data,2)
		tD.data(:, i) = som_norm_variable(tD.data(:,i), sD.comp_norm(i), 'do');
		tD.comp_norm{i} = sD.comp_norm{i};
	end

    index = [];
    for i=1:length(basis.comp_names);
        for j=1:length(tvarnames)
            if strcmp(tvarnames(j), basis.comp_names(i))
                index = [index, j];
                break;
            end
        end
    end

    tmpbasis = som_data_struct(sD.data(:,index), 'name', 'Basis Set', ...
    	'comp_names', tvarnames(index), 'labels', cellstr(num2str(bins)), ...
    	'comp_norm',  sD.comp_norm(index));

	ttmpbasis = som_data_struct(tD.data(:,index), 'name', 'Basis Test Set', ...
		'comp_names', tvarnames(index), 'labels', cellstr(num2str(tbins)), ...
		'comp_norm',  sD.comp_norm(index));
   
    % build SOM with new train data
    sM = som_supervised(tmpbasis, 'mapsize', maptype, 'tracking',0, 'shape', shapes, 'neigh', neighbor);
	mapclass = str2num(char(sM.labels));

    % calculate som statistics temp data
    class = getsomquality(sM, tmpbasis, mapclass);
    [qe, te] = som_quality(sM, tmpbasis);
    tstats  = calcstats(class, bins, bindif, size(tmpbasis.data,2), qe, te);
    tclass = getsomquality(sM, ttmpbasis, mapclass);
	tstats = calcstats1(tclass, tbins, class, bins, bindif, size(tmpbasis.data,2), qe, te);
    stats = tstats;
    outclass = class;
end
% end 9/22/03


varnamesel = basis.comp_names;
if isnan(stats.false_pos)
    stats.false_pos = 0;
end
if isnan(stats.false_neg)
    stats.false_neg = 0;
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
% Get the statistic result with test set
function [qual] = calcstats1(resultsTst, binTst, resultsTrn, binTrn, bindif, numcomps, qe, te)
    qual = struct('test_correct_rate',    sum(binTst==resultsTst)/length(binTst), ...
		'correct_rate',   sum(binTrn==resultsTrn)/length(binTrn), ...
		'test_false_pos',  sum((binTst-resultsTst-bindif)>=0)/length(binTst), ...
		'test_false_neg',  sum((binTst-resultsTst+bindif)<=0)/length(binTst), ...
		'varnum',   numcomps, ...
		'false_pos', sum((binTrn-resultsTrn-bindif)>=0)/length(binTrn), ...
		'false_neg', sum((binTrn-resultsTrn+bindif)<=0)/length(binTrn), ...
		'quantiz_err', qe, 'topog_err', te);

% --------------------------------------------
% Excutes to find the category of data (sD)
function [results] = getsomquality(sM, sD, class);

for i=1:size(sD.data,1)
	results(i,1) = class(find(som_hits(sM, sD.data(i,:), 'crisp')));
end

