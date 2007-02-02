function [Bmat, BestPrinCompn, Emat, MinE, Wmat, Qmat, Pmat] = plsc(Xmat, Ymat, PCnum, varargin);
% PLSC -- compute vectors of W, P, Q, and coefficent, error (noise)term 
% by PLS(partial least square)
%
% Usage:
% ------
% [B] = plsc(Xmat, Ymat, 3);
% [B bestPC] = plsc(X, Y, 5, 'LOO');
% [B bestPC Emat] = plsc(X, Y, 5, 'Monte Carlo');
%
% Input and output arguments ([]'s are optiional):
% Inputs:
% Xmat: matrix: independent variable block
% Ymat: matrix: dependent variable block
% PCnum: integer: the specify for maximum number of principal components
%
% Output:
% Bmat: matrix: Coefficent B
% BestPrinCompn: the best number of principal components
% Emat: matrix: Error (noise) part of Y
% MinE: real: the sum of error squre under validation with bestprincompn
%				it is empty if none validation specified
% Wmat: matrix: calculated W vector
% Qmat: matrix: calculated Q vector
% Pmat: matrix: calculated P vector
%
% Valid argument IDs and corresponding values. Values which are unambiguous
% (marked with '*') can be given without the preceeding argID
% 'LOO':    Leave-one-out process will be invoked
% 'Cross-validate': it invokes cross-validation
% 'Monte Carlo':    A monte Carlo validation will be invoked (20% sampls
% for each time)

% Yun-De Xiao, March 21st 2003
% SIMPLS Algorithm (de Jong, 1993)
% --------------------------------------------------------------------

[nCmpNum, nVarNum] = size(Xmat);    % nCmpNum, nVarNum: number of compounds and variables, respectly
if nCmpNum ~= size(Ymat, 1)
    disp('Check your data - X and Y sample number is not fit');
    return;
end

% Algorithm
% For each h=1, ..., c, where A0=X'Y, M0=X'X, C0=I, and c given
%
% 1. compute qh, the dominant eigenvector of Ah'Ah
% 2. wh=Ahqh, ch=wh'Mhwh, wh=wh/sqrt(ch), and store wh into W as a column
% 3. ph=Mhwh, and store ph into P as a column
% 4. qh=Ah'wh, and store qh into Q as a column
% 5. vh=Chph, and vh=vh/||wh||
% 6. Ch+1=Ch-vhvh' and Mh+1=Mh-phph'
% 7. Ah+1=ChAh
% T = XW and B = WQ' ----- Y = XB + E or Y = TQ' + E  and  X = TP + F
% F is unexplained part of the X scores

szProcess = 'Fix';  % General PLS regression using specify principal componenets

% Check the invoking arguments
if size(varargin, 2) > 0
    argok = 0;
    if size (varargin, 2) == 1
        switch varargin{1},
            % ambiguous values
            case {'Fix', 'LOO', 'Cross-validate', 'Monte Carlo'}, 
                szProcess = varargin{1};
                argok = 1;
            otherwise argok = 0;
        end
    end
    if ~argok
        disp('(PLS) Ignoring invalid argument');
    end
end

switch szProcess
    case 'Fix'
        BestPrinCompn = PCnum;
        MinE = [];
    case {'LOO', 'Cross-validate', 'Monte Carlo'},
        if nCmpNum < 4
            disp ('The data set size is too small for building validation model');
            return;
        end
        nMaxNum = PCnum;
        ESquareSum = zeros(nMaxNum, 1);
        
        % ________________________
        % Define the loop number
        switch szProcess
            case 'LOO'
                nLoop = nCmpNum;
            case {'Cross-validate', 'Monte Carlo'},
                if nCmpNum < 10
                    nLoop = floor(nCmpNum/2);
                elseif nCmpNum <20
                    nLoop = floor(nCmpNum/4);
                elseif nCmpNum < 100
                    nLoop = 8;
                else
                    nLoop = 10;
                end
                if strcmp(szProcess, 'Monte Carlo') == 1
                    nLoop = 2*nLoop;
                end
        end
        % --------------------------
        
        % __________________________
        % Find the best number of principal components
        
        for nVarify=1:nLoop
            % Get training and test set
            trainX = Xmat;
            trainY = Ymat;
            switch szProcess
                case 'LOO'
                    testX = Xmat(nVarify, :);
                    testY = Ymat(nVarify, :);
                    trainX(nVarify, :) = [];
                    trainY(nVarify, :) = [];
                case 'Cross-validate'
                    nLeft = mod(nCmpNum, nLoop);
                    nGroup = floor(nCmpNum/nLoop);
                    if nVarify < nLeft
                        nGroup = nGroup + 1;
                    end
                    nBegin = nGroup * (nVarify-1)+1;
                    if nVarify > nLeft
                        nBegin = nBegin + nLeft;
                    end
                    testX = Xmat(nBegin:nBegin+nGroup-1, :);
                    testY = Ymat(nBegin:nBegin+nGroup-1, :);
                    trainX(nBegin:nBegin+nGroup-1, :) = [];
                    trainY(nBegin:nBegin+nGroup-1, :) = [];
                case 'Monte Carlo'
                    nGroup = floor(nCmpNum*2/nLoop);
                    index = randperm(nCmpNum);
                    index = sort(index(1:nGroup));
                    testX = Xmat(index, :);
                    testY = Ymat(index, :);
                    trainX(index, :) = [];
                    trainY(index, :) = [];
            end
            Amat = trainX.'*trainY;
            Mmat = trainX.'*trainX;
            Cmat = eye(nVarNum);
            Wmat = [];
            Pmat = [];
            Qmat = [];
            
            for nCompn=1:nMaxNum
                tmpQmat = eigs(Amat.'*Amat);    % Get the eigenvectors
                qh = tmpQmat(:, 1);
                wh = Amat*qh;
                ch = wh.'*Mmat*wh;
                wh = wh/sqrt(ch);
                Wmat(:, nCompn) = wh;
                ph = Mmat*wh;
                Pmat(:, nCompn) = ph;
                qh = Amat.'*wh;
                Qmat(:, nCompn) = qh;
                vh = Cmat*ph;
                vh = vh/norm(vh);
                Cmat = Cmat - vh*vh.';
                Mmat = Mmat - ph*ph.';
                Amat = Cmat*Amat;
                Bmat = Wmat*Qmat.';
                Emat = testY - testX*Bmat;
                Emat = sum(Emat.^2);
                ESquareSum(nCompn) = ESquareSum(nCompn) + sum(Emat);
            end
        end
        BestPrinCompn = 1;
        MinE = ESquareSum(1);
        for nCompn=1:nMaxNum
            if ESquareSum(nCompn) < MinE
                MinE = ESquareSum(nCompn);
                BestPrinCompn = nCompn;
            end
        end
end

% -------------------------------------------
% Generate the final PLS model
nCompnNum = BestPrinCompn;
Amat = Xmat.'*Ymat;             % A0=X'Y
Mmat = Xmat.'*Xmat;             % M0=X'X
Cmat = eye(nVarNum);            % C0=I

% Initial W, P and Q matrix
Wmat = [];
Pmat = [];
Qmat = [];

% For each principal component
for nCompn=1:nCompnNum
    tmpQmat = eigs(Amat.'*Amat);    % Get the eigenvectors
    qh = tmpQmat(:, 1);
    wh = Amat*qh;
    ch = wh.'*Mmat*wh;
    wh = wh/sqrt(ch);
    Wmat(:, nCompn) = wh;
    ph = Mmat*wh;
    Pmat(:, nCompn) = ph;
    qh = Amat.'*wh;
    Qmat(:, nCompn) = qh;
    vh = Cmat*ph;
    vh = vh/norm(vh);
    Cmat = Cmat - vh*vh.';
    Mmat = Mmat - ph*ph.';
    Amat = Cmat*Amat;
end
Bmat = Wmat*Qmat.';
Emat = Ymat - Xmat*Bmat;
%-------------------------------------------------------------------------