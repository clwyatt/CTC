function [prd_class, stats, calc_class] = sompredict(dataTrn, binsTrn, varnamesTrn, ...
		keyvarnames, dataPrd, varnamesPrd, varargin);		

% SOMPREDICT SOM: once get key descriptors, build model, then predict the unknow data set
%
% [prd_class, stats] = stepwise(dataTrn, binsTrn, varnamesTrn, keyvarnames, dataPrd, varnamesPrd, 'testbin', binsPrd, 'gaussian', 'sheet', 'big');
% 
%	Input and output arguments ([]'s are optional): 
%	dataTrn	(matrix)	training data matrix
%	binsTrn	(vector)	training sample's categories
%	varnamesTrn	(vector)	list of training variable names
%	keyvarnames	(vector)	list of key variable names
%	dataPrd	(matrix)	prediction data matrix
%	varnamesPrd	(vector)	list of test set variable names
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
%	prd_class	(vector)	the calculated bins for test set
%	stats	(struct)	the statistical result
%	calc_class	(vector)	the calculated bins for training set
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
%	maptype		*(string)	Specifies the som maptype, default is 'big', options: 'big', 'normal', 'small'
%	neighbor	*(string)	Specifies the som neighbor, default is 'gaussian,
%							Options:	'gaussian' 'cutgauss' 'ep' 'bubble'
%	shapes		*(string)	Specifies the som shapes, default is 'sheet', 
%							Options:	'sheet', 'cyl', 'toroid'
%	testbin		*(string)	Identify the following parameter is known test set bins
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% default values
maptype			= 'big';		% 'big' 'normal' 'small'
neighbor		= 'gaussian';	% 'gaussian' 'cutgauss' 'ep' 'bubble'
shapes			= 'sheet';		% 'sheet' 'cyl' 'toroid'

bTest = 0;	% if the test set bins are known, bTest = 1;
binsPrd = [];
sortgoodtobad = 0;

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
				maptype = varargin{i};
			case {'testbin'},
				i = i + 1;
				binsPrd = varargin{i};
            case {'Bin Sort'},
				i = i + 1; sortgoodtobad = varargin{i};     
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(stepwisesom) Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end


index = zeros(length(keyvarnames),1);
for i=1:length(keyvarnames)
    for j=1:length(varnamesTrn)
		if strcmp(keyvarnames(i), varnamesTrn(j))
			index(i) = j;
			break;
		end
	end
end

if ~isempty(find(index == 0))
	disp(char(keyvarnames(find(index == 0))));
	disp('Please check your training set and the key descriptors');
	prd_class = [];
	stats = [];
	calc_class = [];
	return;
end
data = dataTrn(:, index);

[r c] = size(dataPrd);
dataTe = zeros(r, length(keyvarnames));

% define the key descriptor in the prediction set. If key descriptor is missing, all its value will be zeros
for i=1:length(keyvarnames)
	for j=1:length(varnamesPrd)
		if strcmp(keyvarnames(i), varnamesPrd(j))
            dataTe(:, i) = dataPrd(:, j);
			break;
		end
	end
end

% build and normalize traing/test som data structures (sD) and (tD)
sD = som_data_struct(data, 'name', 'Input Data Set', 'comp_names',	keyvarnames, ...
		'labels', cellstr(num2str(binsTrn)));

% Ersin comment out normalizations    
sD = som_normalize(sD,'var');			%normalize variance to unity
% Ersin add the denormalization to offset normalization
%sD = som_denormalize(sD);
%sD = som_normalize(sD,'logistic');		%normalize range to logistic fxn
% End Ersin

tLabel = 1:r;
tLabel = tLabel.';
tD = som_data_struct(dataTe, 'name', 'Data to Predict', 'comp_names',	keyvarnames, ...
			'labels', cellstr(num2str(tLabel)));
for i=1:size(sD.data,2)
	tD.data(:,i) = som_norm_variable(tD.data(:,i), sD.comp_norm(i), 'do');
	tD.comp_norm{i} = sD.comp_norm{i};
end

% build SOM from basis
sM= som_supervised(sD, 'mapsize', maptype, 'tracking', 0, 'shape', shapes, 'neigh', neighbor);
mapclass = str2num(char(sM.labels));

% gather basis statistics on initial regression model
class = getsomquality(sM, sD, mapclass);
[qe, te] = som_quality(sM, sD);  % qe: mean quantization error; te: topographic error
prd_class = getsomquality(sM, tD, mapclass);

if sortgoodtobad 
    GB = 1; BB = max(binsTrn); 
else 
    GB = max(binsTrn); BB = 1; 
end; 

% Error on test
if ~isempty(binsPrd)
    stats.test_correct_rate = sum(prd_class == binsPrd)/length(binsPrd);
    stats.test_miss = sum((prd_class == BB).*(binsPrd == GB))/length(binsPrd);
    stats.test_falsehit = sum((prd_class == GB).*(binsPrd == BB))/length(binsPrd);
    stats.test_false_pos = sum((prd_class == GB).*(binsPrd ~= GB))/length(binsPrd);
    stats.test_false_neg = sum((prd_class ~= GB).*(binsPrd == GB))/length(binsPrd);
else
    stats.test_correct_rate = 0;
    stats.test_miss = 0;
    stats.test_falsehit = 0;
    stats.test_false_pos = 0;
    stats.test_false_neg = 0;
end;

stats.correct_rate = sum(class == binsTrn)/length(binsTrn);
stats.false_pos = sum((class == GB).*(binsTrn ~= GB))/length(binsTrn);
stats.false_neg = sum((class ~= GB).*(binsTrn == GB))/length(binsTrn);
stats.miss = sum((class == BB).*(binsTrn == GB))/length(binsTrn);
stats.falsehit = sum((class == GB).*(binsTrn == BB))/length(binsTrn);
stats.varnum = size(sD.data,2);
stats.quantiz_err = qe;
stats.topog_err = te;

% --------------------------------------------
% Excutes to find the category of data (sD)
function [results] = getsomquality(sM, sD, class);

for i=1:size(sD.data,1)
	results(i,1) = class(find(som_hits(sM, sD.data(i,:), 'crisp')));
end