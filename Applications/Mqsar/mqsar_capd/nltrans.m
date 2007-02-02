function [newx, expterm, logterm, sqtterm, meanmin, stdmax] = nltrans(Xmat, varargin);

% NLTRANS -- non linear transform and scaling
%
% Usage:
% ------
% [NewX, ExpTerm, LogTerm, SqtTerm] = nltrans(Xmat, 'Square', 'Square root', 'Exp');
% [newx, expterm, logterm, sqtterm, means, tstd ] = nltrans(X, 'Exp', 'Log', 'Inv', 'norm');
% [newx, expterm, logterm, sqtterm, means, tstd ] = nltrans(X, 'Exp', 'Log', 'Inv', 'range');
%
% Input and output arguments ([]'s are optiional):
% Inputs:
% Xmat: matrix: independent variable block
%
% Output:
% newx:		matrix:	the new matrix after data transformation/scaling
% expterm:	array:	identify exponent transformed iterms, 1 -- transformed, 0 -- unable
% logterm:	array:	identify the natural logarithm transformed iterms, 1 - transformed, 0 -unable
% sqtterm:	array:	identify the square root transformed iterms, 1 - transformed, 0 - unable
% meanmin:	array:	the variable mean value for normalization or min for range scaling
% stdmax:		array:	standard deviation for normalization or max for range scaling
%
% Valid argument IDs and corresponding values. Values which are unambiguous
% (marked with '*') can be given without the preceeding argID
% 'norm':	*string:	normarization
% 'range':	*string:	range scaling
% 'Square':	*string:	add square term to data set
% 'Square root': *string: add square root term to data set
% 'Exp':	*string:	add exponent term to data set
% 'Inv':	*string:	add inverse term to data set
% 'Log':	*string:	add the natural logarithm term to data set

% Yun-De Xiao, April 26th 2003
% --------------------------------------------------------------------

% default argument
NewX = Xmat;
expterm = [];
logterm = [];
sqtterm = [];
scaling = 0;
bSq = 0;	% square term
bSqt = 0;	% square root term
bExp = 0;	% exponent term
bLog = 0;	% the natural logarithm term
bInv = 0;	% the inverse term

% Check the invoking arguments
i = 1;
while i <= size(varargin, 2)
    argok = 1;
    if ischar(varargin{i}),
        switch varargin{i},
            % unambiguous values
            case {'norm'},
				scaling = 1;
			case {'range'},
				scaling = 2;
            case {'Square'},
                bSq = 1;
			case {'Square root'},
				bSqt = 1;
            case {'Exp'},
                bExp = 1;
            case {'Inv'},
                bInv = 1;
            case {'Log'}
                bLog = 1;
            otherwise argok = 0;
        end
    end
    if ~argok
        disp(['(NPLS) Ignoring invalid argument: #', num2str(i)]);
    end
    i = i + 1;
end

% Add square term
if bSqu
    tmpData = [tmpData Xmat.^2];
end


% Add exponent term
if bExpon
	ExpTerm = zeros(size(Xmat, 2), 1);
    for i=1:size(Xmat, 2)
        xmax = max(abs(Xmat(:, i)));
        if xmax < 50
            ExpTerm(i) = 1;
            tmpData = [tmpData exp(Xmat(:, i))];
        end
    end
end

% Add inverse term
tmp = [];
if bInv
	[r c] = size(Xmat);
    for i=1:r
        for j=1:c
            if abs(Xmat(i, j)) < 1.0e-20
                tmp(i, j) = 1.0e20;
            else 
                tmp(i,j) = 1/Xmat(i, j);
            end
        end
    end
    tmpData = [tmpData, tmp];
    tmp = [];
end

% Add logarithm term
/////////////////////////////////////////////////////////
if bLog
	LogTerm = zeros(size(Xmat, 2), 1);
    for i=1:size(Xmat, 2)
        xmin = min((Xmat(:, i)));
        if xmin <= 0
            LogTerm(i) = abs(xmin) + 1.0e-10;
        end
        tmpData = [tmpData, log(Xmat(:, i) + LogTerm(i))];
    end
end

MeanStdTerm = zeros(size(Xmat, 2), 2);
if bScaling
    for i=1:size(tmpData, 2)
        xmean = mean(tmpData(:, i));
        xstd = std(tmpData(:, i));
        tmpData(:, i) = (tmpData(:, i) - xmean)/xstd;
		MeanStdTerm(i, 1) = xmean;
		MeanStdTerm(i, 2) = xstd;
    end
end
[B, BestPrinCompn, E, MinE, W, Q, P] = plsc(tmpData, Ymat, PCnum, szProcess);

Fitness = zeros(2, 1);
Fitness = [];
Fitness(1) = 1 - sqrt(sum(sum(E.^2))/(size(Xmat, 1) - 1))/sum(std(Ymat));
if ~isempty(MinE)
    Fitness(2) = 1 - sqrt(MinE/(size(Xmat, 1) - 1))/sum(std(Ymat));
end