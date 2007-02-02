function [B, BestPrinCompn, E, Fitness, ExpTerm, LogTerm, W, Q, P, MeanStdTerm] = npls(Xmat, Ymat, PCnum, varargin);

% NPLS -- generate linear or non-linear PLS model
%
% Usage:
% ------
% [B] = npls(Xmat, Ymat, 3);
% [B bestPC] = npls(X, Y, 5, 'LOO');
% [B bestPC E] = npls(X, Y, 5, 'Monte Carlo', 'Nonlinear');
% [B BestPC E Fitness = npls(X, Y, 6, 'LOO', 'Scaling');
%
% Input and output arguments ([]'s are optiional):
% Inputs:
% Xmat: matrix: independent variable block
% Ymat: matrix: dependent variable block
% PCnum: integer: the specify or maximum number of principal components
%
% Output:
% B: matrix: Coefficent B
% BestPrinCompn: the best number of principal components
% E: matrix: Error (noise) part of Y
% Fitness: real:    Q-square in general, except for Monte Carlo
% W: matrix: calculated W vector
% Q: matrix: calculated Q vector
% P: matrix: calculated P vector
%
% Valid argument IDs and corresponding values. Values which are unambiguous
% (marked with '*') can be given without the preceeding argID
% 'LOO':    Leave-one-out process will be invoked
% 'Cross-validate': it invokes cross-validation
% 'Monte Carlo':    A monte Carlo validation will be invoked (20% sampls
% for each time)
% 'Nonlinear':  non-linear PLS will be invoked (include X^2, 1/X, log(X+c),exp(X))
% 'Scaling':    normarization scaling
% 'Square':     add square term to data set
% 'Exponent':   add exponent term to data set
% 'Inverse':    add inverse term to data set
% 'Logarithm':  add the natural logarithm term to data set

% Yun-De Xiao, March 24th 2003
% --------------------------------------------------------------------
% Check the invoking arguments

% default is linear non-scaling specify principal component number PLS model
szProcess = 'Fix';
bNonlinear = 0;
bScaling = 0;
bSqu = 0;
bExpon = 0;
bInv = 0;
bLog = 0;

i = 1;
while i <= size(varargin, 2)
    argok = 1;
    if ischar(varargin{i}),
        switch varargin{i},
            % unambiguous values
            case {'Fix', 'LOO', 'Cross-validate', 'Monte Carlo'}, 
                szProcess = varargin{i};
            case {'Nonlinear'},
                bNonlinear = 1;
            case {'Scaling'},
                bScaling = 1;
            case {'Square'},
                bSqu = 1;
            case {'Exponent'},
                bExpon = 1;
            case {'Inverse'},
                bInv = 1;
            case {'Logarithm'}
                bLog = 1;
            otherwise argok = 0;
        end
    end
    if ~argok
        disp(['(NPLS) Ignoring invalid argument: #', num2str(i)]);
    end
    i = i + 1;
end

% Add all the nonlinear terms into the data set
if bNonlinear
    bSqu = 1;
    bExpon = 1;
    bInv = 1;
    bLog = 1;
end

tmpData  = Xmat;
% Add square term
if bSqu
    tmpData = [tmpData Xmat.^2];
end

% Add exponent term
ExpTerm = zeros(size(Xmat, 2), 1);
if bExpon
    for i=1:size(Xmat, 2)
        xmax = max(abs(Xmat(:, i)));
        if xmax < 50
            ExpTerm(i) = 1;
            tmpData = [tmpData exp(Xmat(:, i))];
        end
    end
end

% Add inverse term
[r c] = size(Xmat);
tmp = [];
if bInv
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
LogTerm = zeros(size(Xmat, 2), 1);
if bLog
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
