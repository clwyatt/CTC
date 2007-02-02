function [som_tbl, pls_tbl] = results(fname, modname, varargin)

% SUPERVISED SOM CLASSIFICATION
%
% [results] = results(fname, 'a4b2');
% [results] = results(fname, 'a4b2', 'cyl');
% 
%	Input and output arguments ([]'s are optional): 
%	fname		(string)  Specifies the source training data file
%	modname		(string) Specifies the source model file
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
%	resulttbl	(struct)  SOM results in table
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
% Copyright 2002 Yun-De Xiao

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Ersin: commented out rand state seeding
%rand('state',sum(100*clock));	%re-seed random number generator


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% check arguments

% default values
type			= 'null';
maptype			= 'big';		% 'big' 'normal' 'small'
neighbor		= 'gaussian';	% 'gaussian' 'cutgauss' 'ep' 'bubble'
shapes			= 'sheet';		% 'sheet' 'cyl' 'toroid'
binnum = 3;

%% varargin
i=1;
while i<=size(varargin,2), 
	argok = 1;
	if ischar(varargin{i}), 
		switch varargin{i}, 
			% unambiguous values
			case {'ganglion', 'muscle', 'a4b2', 'd2', 'd3', 'dhfr', 'topliss', 'a7'},
				type = varargin{i};
				if strcmp(type, 'dhfr') == 1
					i = i + 1;
					binnum = varargin{i};
				end
			case {'gaussian', 'cutgauss', 'ep', 'bubble'},
				neighbor = varargin{i};
			case {'sheet', 'cyl', 'toroid'},
				shapes = varargin{i};
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(somstep2) Ignoring invalid argument: #' num2str(i+1)]); 
	end
	i = i+1; 
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% source data import according to data 'type'

dataDir		= '/lama/pentad/datasets/paper/data';
modelDir	= '/lama/pentad/datasets/paper/model';
outDir		= '/lama/pentad/datasets/paper/out';

outfile			= strcat(outDir,'/som_result_', type, '_', neighbor, '_', shapes, '_', fname);
pos = strfind(outfile, '.');
outfile = [outfile(1:pos), 'out'];
fname		= [dataDir, '/', fname];

switch type
	case ('ganglion')
		cutoffs    = [20 45];
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 1, 'cutoffs', cutoffs, 'ex', 2);
	case ('muscle')
		cutoffs    = [20 45];
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 2, 'cutoffs', cutoffs, 'ex', 1);
	case ('a4b2')
		cutoffs    = [250 1000];
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 'bioact', ...
												'start', 'binning', 2, 'cutoffs', cutoffs);
	case ('topliss')
		cutoffs    = [1.5 2.5 3.5];
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 'bioact', ...
												'start', 'binning', 2, 'cutoffs', cutoffs);
	case ('d2')
		cutoffs    = [100 500];
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 'bioact', ...
												1, 'cutoffs', cutoffs, 'ex', 2);
	case ('d3')
		cutoffs    = [100 500];
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 'bioact', ...
												2, 'cutoffs', cutoffs, 'ex', 1);
	case ('dhfr')
		switch binnum
			case (3)
				cutoffs = [6.75 7.75];
			case (4)
				cutoffs = [6.5 7.46 7.9];
			case (5)
				cutoffs = [6.30 7.21 7.68 7.98];
			case (6)
				cutoffs = [6.17 6.80 7.47 7.76 8.03];
		end
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 'bioact', ...
												'start', 'cutoffs', cutoffs);
	case ('a7')
		cutoffs    = [100 600 2000];
		[CmpNames VarNames xData yData nBins] = qsarimport(fname, 'bioact', ...
												1, 'cutoffs', cutoffs);
	otherwise
		fprintf(1,'Err: "%s" is an unrecognized bioactivity type.\n', type);
		break;
end
%bindif = max(nBins) - min(nBins);
bindif = 2;

fid = fopen(modname);
tline = fgetl(fid);
Index = [];
partMask = [];
bModel = 0;
kk = 1;
som_stats = [];
pls_stats = [];
while tline ~= -1
	[str tline] = strtok(tline, '	');
	if strcmp(str, 'Descriptors:') == 1
		Index = [];
		while(~isempty(tline))
			[str tline] = strtok(tline, '	');
			if isempty(tline)
				str = str(1:length(str)-1);
			end
			ID = find(strcmp(VarNames, str) == 1);
			Index = [Index ID];
		end
		Index = unique(Index);
		bModel = 1;
	end
	if strcmp(str, 'Partition_Mask:') == 1
		partMask = [];
		while(~isempty(tline))
			[str tline] = strtok(tline, '	');
			partMask = [partMask str2num(str)];
		end
		bModel = bModel + 1;
	end
	if bModel == 2
		bModel = 3;
		colNames = VarNames(Index);
		[aa bb] = size(Index)
		dataTst = xData(find(partMask==1), Index);
		dataTrn = xData(find(partMask==0), Index);

		binTst = nBins(find(partMask==1));
		binTrn = nBins(find(partMask==0));

		rowsTst = CmpNames(find(partMask==1));
		rowsTrn = CmpNames(find(partMask==0));

		bioTrn = yData(find(partMask==0));
		bioTst = yData(find(partMask==1));
		sD = som_data_struct(dataTrn, 'name', 'Input Data Set', ...
				'comp_names', colNames, 'labels', cellstr(num2str(binTrn)));
		sD = som_normalize(sD,'var');			%normalize variance to unity
		sD = som_normalize(sD,'logistic');		%normalize range to logistic fxn

		tD = som_data_struct(dataTst, 'name', 'Data to Predict', ...
				'comp_names', colNames, 'labels', rowsTst);
		for i=1:size(sD.data,2)
			tD.data(:,i) = som_norm_variable(tD.data(:,i), sD.comp_norm(i), 'do');
			tD.comp_norm{i} = sD.comp_norm{i};
		end

		sM = som_supervised(sD, 'mapsize', maptype, 'tracking', 0, 'shape', shapes, 'neigh', neighbor);
		class = str2num(char(sM.labels));

		somqtrn  = getsomquality(sM, sD, class);
		somqtst  = getsomquality(sM, tD,  class);
		[qe, te] = som_quality(sM, sD);
		som_stats{kk} = calcstats(somqtst, binTst, somqtrn, binTrn, bindif, size(sD.data,2), qe, te, 0);
		% PLS parts
		maxlv = 7;
		if maxlv > min(size(sD.data))
			maxlv = min(size(sD.data));
		end
		[press, cumpress] = crossval(sD.data, bioTrn, 'nip', 'loo', maxlv);
		oldcumpress = cumpress(1);
		bestLv = 1;
		sd = std(bioTrn);
		% Select the number of the best principal components
		for lv=2:maxlv
			if 100.0*(oldcumpress-cumpress(lv))/(length(bioTrn)*(sd^2)) > 1
				oldcumpress = cumpress(lv);
				bestLv = lv;
				continue
			end
			break;
		end
		if bestLv ~= 1
			bestLv = bestLv - 1;
		end
		qsq = 1 - sqrt(cumpress(bestLv)/length(bioTrn))/sd;
		[b, ssq, p, q, r, t, u, v] = simpls(sD.data, bioTrn, bestLv);
		calc = sD.data*b.';
		calc = calc(:, end);
		rsq = 1 - sqrt((calc - bioTrn).'* (calc - bioTrn) / length(bioTrn))/sd;
		plstrn = bincutoff(calc, cutoffs);
		prd = tD.data*b.';
		prd = prd(:, end)
		PrdRsq = 1 - sqrt((prd - bioTst).'*(prd - bioTst) / length(bioTst)) / std(bioTst);
		plstst = bincutoff(prd, cutoffs);
		pls_stats{kk} = calcstats(plstst, binTst, plstrn, binTrn, bindif, bestLv, qsq, PrdRsq, rsq);
		kk = kk + 1;
	end
	tline = fgets(fid);
end
fclose(fid);
som_tbl = zeros(10, length(som_stats));
pls_tbl = zeros(10, length(pls_stats));
for i=1:length(som_stats)
	som_tbl(1, i) = som_stats{i}.test;
	som_tbl(2, i) = som_stats{i}.train;
	som_tbl(3, i) = som_stats{i}.comps;
	som_tbl(4, i) = som_stats{i}.fptest;
	som_tbl(5, i) = som_stats{i}.fntest;
	som_tbl(6, i) = som_stats{i}.fptrain;
	som_tbl(7, i) = som_stats{i}.fntrain;
	som_tbl(8, i) = som_stats{i}.qe;
	som_tbl(9, i) = som_stats{i}.te;
	ssom_tbl(10, i) = som_stats{i}.re;
	pls_tbl(1, i) = pls_stats{i}.test;
	pls_tbl(2, i) = pls_stats{i}.train;
	pls_tbl(3, i) = pls_stats{i}.comps;
	pls_tbl(4, i) = pls_stats{i}.fptest;
	pls_tbl(5, i) = pls_stats{i}.fntest;
	pls_tbl(6, i) = pls_stats{i}.fptrain;
	pls_tbl(7, i) = pls_stats{i}.fntrain;
	pls_tbl(8, i) = pls_stats{i}.qe;
	pls_tbl(9, i) = pls_stats{i}.te;
	pls_tbl(10, i) = pls_stats{i}.re;
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [qual] = calcstats(resultsTst, binTst, resultsTrn, binTrn, bindif, numcomps, qe, te, re);

	bindif = 2;
	qual = struct('test',    sum(binTst==resultsTst)/length(binTst), ...
		'train',   sum(binTrn==resultsTrn)/length(binTrn), ...
		'comps',   numcomps, ...
		'fptest',  sum((binTst-resultsTst-bindif)>=0)/length(binTst), ...
		'fntest',  sum((binTst-resultsTst+bindif)<=0)/length(binTst), ...
		'fptrain', sum((binTrn-resultsTrn-bindif)>=0)/length(binTrn), ...
		'fntrain', sum((binTrn-resultsTrn+bindif)<=0)/length(binTrn), ...
		'qe', qe, 'te', te, 're', re);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [results] = getsomquality(sM, sD, class);

	for i=1:size(sD.data,1)
		results(i,1) = class(find(som_hits(sM, sD.data(i,:), 'crisp')));
	end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [bins] = bincutoff(yData, cutoffs)
	bins = zeros(length(yData), 1);
	for num=1:length(yData)
		bins(num) = length(cutoffs) + 1;
		for class=1:length(cutoffs)
			if yData(num) < cutoffs(class)
				bins(num) = class;
				break;
			end
		end
	end