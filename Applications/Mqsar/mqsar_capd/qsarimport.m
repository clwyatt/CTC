function [rownames,colnames, xdata, ydata, ynames, bins] = qsarimport(fname, varargin);
% Generic QSAR/QSPR data import routine. Read the row headers and the column
% headers of the file. The rest of the data is treated as numeric data. 
% The data includes X and Y block, and routine returns the X and Y data and descriptor and 
% compound names, and may also return bin information if available. 
%
% [tNames, tHeader, xData] = qsarimport(fname);
%
% [tNames, tHeader, xData, yData] = qsarimport(fname, 'bioact', 1);
% [tNames, tHeader, xData, yData, Bins] = qsarimport(fname, 'bioact', 1, 'binning', 2);
% [tNames, tHeader, xData, yData, Bins] = qsarimport(fname, 'bioact', 1, 'binning', 2);
% [tNames, tHeader, xData, yData, Bins] = qsarimport(fname, 'bioact', 1);
% [tNames, tHeader, xData, yData, Bins, center] = qsarimport(fname, 'bioact', 1);
% [tNames, tHeader, xData, yData] = qsarimport(fname, 'bioact', 'end');
%
%	Input and output arguments ([]'s are optional): 
%	fname		(string)  Specifies the source data file
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
% rownames	:	Row headers, 1st column of the file
% colnames	:	Column headers, 1st row of the file, excluding Y block title,
%				bin and redundant column name.
% xdata		:	X block numeric data part of the file.
% ydata		:	Y block numeric data.
% bins		:	binning information
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
%	'bioact'	*(string)  specifies where the bioactivity column is located
%			   'end', specifies the last column
%	'binning'	*(string)  specifies a user-defined column of binning classifications
%				i.e. 'binning', 2; specifies the 2nd column of datafile
%					contains the molecule binning
%   'cutoffs'   (vector)  binning cutoffs; i.e. 'cutoffs', [6.75 7.52]
%  	'balance'	*(string) binning data set based on bioact balance
%	'range'		*(string) binning data set based on bioact range
%
% Ersin Bayram, May 2004
%---------------------------------------------------------------

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% check arguments

% default values
ydata = [];
ynames = [];
bins = [];
bioact = [];
binning = [];
ex = [];
balance = -1;
range = -1;
cutoffs = [];
sortby = 'ascending';   %  add for default low end is positive end

%% varargin
i=1;
while i<=size(varargin,2), 
	argok = 1;
	if ischar(varargin{i}), 
		switch varargin{i}, 
			% ambiguous values
			case {'bioact'},	i=i+1;	bioact	= varargin{i}; 
			case {'binning'},	i=i+1;	binning	= varargin{i}; 
            case {'cutoffs'},	i=i+1;	cutoffs	= varargin{i};
            case {'ex'},		i=i+1;	ex		= varargin{i};     
            case {'balance', 'Balance', 'BALANCE'},	i=i+1;	balance	= varargin{i};    
            case {'range', 'Range', 'RANGE'},		i=i+1;	range	= varargin{i};
            case {'Bin Sort'}, i = i + 1; sortby = varargin{i};            
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(qsarimport) Ignoring invalid argument: #' num2str(i+1)]); 
	end
	i = i+1; 
end

[rownames, colnames, xdata] = dataimport(fname);
rownames(1) = [];
colnames(1) = [];
delIndex = [];

if ~isempty(bioact)
	if ischar(bioact)
		switch(bioact)
			case {'start', 'Start', 'START'},
				ydata = xdata(:, 1);
                ynames = colnames(1);
				delIndex = [delIndex, 1];
			case {'end', 'End', 'END'},
				ydata = xdata(:, end);
				nEnd = size(xdata, 2);
                ynames = colnames(end);
				delIndex = [delIndex, nEnd];
		end
	else
		ydata = xdata(:, bioact);
        ynames = colnames(bioact);
		delIndex = [delIndex, bioact];
	end
end
if ~isempty(binning)
	bins = xdata(:, binning);
	delIndex = [delIndex, binning];
end

% Exclude the user specified columns
if ~isempty(ex)
	delIndex = [delIndex, ex];
end

xdata(:, delIndex) = [];
colnames(delIndex) = [];

if ~isempty(cutoffs) & isempty(binning)
	if bioact < 0
		disp(['please specify the bioact item!']);
	end
	bins = ones(length(ydata), 1);
	for nCount=1:length(ydata)
		nBin = length(cutoffs) + 1;
		for nN=1:length(cutoffs)
			if ydata(nCount) < cutoffs(nN)
				nBin = nN;
				break;
			end
		end
		bins(nCount) = nBin;
	end
    
    % add for higher value for positive
    %     if strcmp(sortby, 'descending')
    %         nBin = length(cutoffs) + 2;
    %         bins = nBin - bins;
    %     end
end
if balance > 0
	if bioact < 0
		disp(['please specify the bioact item!']);
	end
	[bins, cutoffs] = binning(ydata, balance);
end
if range > 0
	if bioact < 0
		disp(['please specify the bioact item!']);
	end
	[bins, cutoffs] = binning(ydata, range, 'range');
end

