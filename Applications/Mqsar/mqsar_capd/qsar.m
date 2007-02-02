function varargout = qsar(varargin)
% QSAR M-file for qsar.fig
%      QSAR, by itself, creates a new QSAR or raises the existing
%      singleton*.
%
%      H = QSAR returns the handle to a new QSAR or the handle to
%      the existing singleton*.
%
%      QSAR('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in QSAR.M with the given input arguments.
%
%      QSAR('Property','Value',...) creates a new QSAR or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before qsar_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to qsar_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @qsar_OpeningFcn, ...
                   'gui_OutputFcn',  @qsar_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);

if nargin & isstr(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT
% End initialization code - DO NOT EDIT

% --- Executes just before qsar is made visible.
function qsar_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to qsar (see VARARGIN)
% Choose default command line output for qsar

global userdata;
warning off;
currdir = pwd;
sepind= findstr(currdir,filesep);
path(path,strcat(currdir(1:sepind(end)),'somtoolbox2'));
% Ersin: Moved the random seed generator to here. If seed is reset based on
% clock too frequently, we might get the same random numbers. The range is
% wide enough to reseed it only once at the beginning of pentad. 

rand('state',sum(100*clock));	%re-seed random number generator

% Initial global set up parameters
userdata.Scaling = 1;
userdata.ComponNum = 3;
userdata.Val = 1;
userdata.Nonlinear = 0;
userdata.Square = 0;
userdata.Exp = 0;
userdata.Inverse = 0;
userdata.Log = 0;
userdata.Fin = 4.0;
userdata.Fout = 4.0;
userdata.ChkCluster = 0;
userdata.Cluster = 1;
userdata.Dist = 1;

% GA related setting
userdata.Descriptors = 30;
userdata.SubsetPopulation = 60;
userdata.Generation = 600;
userdata.Subsets = 1;
userdata.PoolFreq = 0;
userdata.PoolDur = 600;
userdata.MaxMutate = 10;
userdata.MinMutate = 2;
userdata.MaxCrossOver = 10;
userdata.MinCrossOver = 0;
userdata.Elites = 10;
userdata.Pmutation = 0.5;
userdata.Pcross = 0.7;
userdata.MoveTarget = 0;
userdata.EliteDuration =10;

% Data I/O related settings
userdata.CorrFiltThreshold = 1.0;
userdata.StdFiltThreshold = 0.0;
userdata.ClassBins = 3.0;
userdata.chkFixInitial = 0;

% SA related settings
userdata.StartT = 100;
userdata.EndT = 1.0e-6;
userdata.Droprate = 0.9;
userdata.Cycle = 20;
userdata.Mutate = 2;
userdata.ChkFixLen = 0;
userdata.FixLen = 10;
userdata.Norm = 0;

handles.output = hObject;
% Update handles structure
guidata(hObject, handles);

% --- Outputs from this function are returned to the command line.
function varargout = qsar_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);

% Get default command line output from handles structure
varargout{1} = handles.output;

% --- Executes on button press in jobs_build.
function jobs_build_Callback(hObject, eventdata, handles)
set(handles.jobs_build, 'Value', 1);
set(handles.jobs_prediction, 'Value', 0);
%guidata(hObject, handles);

% --- Executes on button press in jobs_prediction.
function jobs_prediction_Callback(hObject, eventdata, handles)
set(handles.jobs_build, 'Value', 0);
set(handles.jobs_prediction, 'Value', 1);
%guidata(hObject, handles);

% --- Executes on button press in jobs_run.
function jobs_run_Callback(hObject, eventdata, handles)
% ------- Verify all parameter
% check if the data file name is valid under building model mode or kNN
% algorithm/SOM is chosen
global userdata;
userdata.szDataName = get(handles.edtFileName, 'String');
userdata.szMethod = get(handles.popMethod, 'String');
userdata.szMethod = userdata.szMethod(get(handles.popMethod, 'Value'));

% Check if the model file name is valid
userdata.szModelName = get(handles.edtModelFile, 'String');
if ~ischar(userdata.szModelName) | isempty(userdata.szModelName);
    szStr = 'Please enter a valid model file name!';
    ShowInf(szStr, handles);
    return;
end

if get(handles.jobs_build, 'Value') | strcmp(userdata.szMethod, 'KNN') | ...
        strcmp(userdata.szMethod, 'SOM') | strcmp(userdata.szMethod, 'FCM') | ...
        strcmp(userdata.szMethod, 'CM') | strcmp(userdata.szMethod, 'NNET')
    if ~ischar(userdata.szDataName) | isempty(userdata.szDataName)
        szStr = 'Please enter a valid data file name!'; 
        ShowInf(szStr, handles);
        return;
    else
        % define the bioact column
        szStr = get(handles.edtFileBioAct, 'String');
        if strcmpi(szStr, 'start')
            userdata.bioact = 1;
        elseif strcmpi(szStr, 'end')
            userdata.bioact = 'end';
        else
            userdata.bioact = str2num(szStr);
        end
    
        % define the binning column
        userdata.bin = str2num(get(handles.edtFileBin, 'String'));
        % define the exclusive columns
        userdata.exc = str2num(get(handles.edtFileExcl, 'String'));
        % define the cutoff arguments
        userdata.cutoff = str2num(get(handles.edtFileCutoff, 'String'));
        
        userdata.ShuffleFlag = get(handles.Check_Shuffle,'Value');
    
        % Read file according the specify options
        [CmpNames, VarNames, Xmat, Ymat, Ynames, Bins] = qsarimport(userdata.szDataName, 'bioact', ...
                                                         userdata.bioact, 'binning', userdata.bin, ...
                                                         'cutoffs', userdata.cutoff, 'ex', userdata.exc);
                                                                         
        [VarNames, Xmat] = qsarfilter(VarNames, Xmat,userdata.CorrFiltThreshold, userdata.StdFiltThreshold);

        if userdata.ShuffleFlag
           RandInd = randperm(length(Ymat)); 
           Ymat = Ymat(RandInd);
           Bins = Bins(RandInd);    
       end    
    end
end

% Clear the information show window
szStr = [];
ShowInf(szStr, handles);

% define the partition file and check if it is needed
szPartition = get(handles.edtMaskFile, 'String');
bPartition = 1;
if ~ischar(szPartition) | isempty(szPartition)
    button = questdlg('Do you want to build model without any test set?', ...
        'Confirm Partition File', 'Yes', 'No', 'No');
    if strcmp(button, 'No')
        ShowInf('Please enter a valid partition file name!', handles);
        return;
    end
    bPartition = 0;
end

% get the optimization handles
userdata.nOpt = get(handles.popOptimization, 'Value');

% ------------------------------------------------
% Executes on building models
if get(handles.jobs_build, 'Value')
    
    userdata.bins = Bins;
    szStr = ['Compounds: ', num2str(length(CmpNames)), '   Variables: ', num2str(length(VarNames))];
    disp(szStr);
    ShowInf(szStr, handles);
    
    if bPartition
        [rownames columnames Maskmat] = qsarimport(szPartition);
        if size(Xmat, 1) ~= size(Maskmat, 1)
            ShowInf(['The partition file does not fit this data set.', ...
                    ' Check your data/partition files and try again.'], handles);
            return;
        end
    end
    
    % Building the specify models
    % Define the non-linear options
    if userdata.Square 
        szSquare = 'Square';
    else
        szSquare = [];
    end
    if userdata.Exp
        szExp = 'Exponent';
    else
        szExp = [];
    end
    if userdata.Inverse
        szInverse = 'Inverse';
    else
        szInverse = [];
    end
    if userdata.Log
        szLog = 'Logarithm';
    else
        szLog = [];
    end
    
    % Define the validation type
    switch userdata.Val
    case 1
        szVal = 'Fix';
    case 2
        szVal = 'LOO';
    case 3
        szVal = 'Cross-validation';
    case 4
        szVal = 'Monte Carlo';
    otherwise
        ShowInf('Validation type error! Please report your problem.', handles);
        return;
    end
    
    switch userdata.Scaling
    case 2 % Normalize to Mahalanobis distance
        xmean = mean(Xmat);
        xstd = std(Xmat);
        Xmat = (Xmat - ones(size(Xmat,1),1)*xmean)./(ones(size(Xmat,1),1)*xstd);
    case 3 % Normalize with respect to data range
        xmin = min(Xmat);
        xmax = max(Xmat);            
        Xmat = (Xmat - ones(size(Xmat,1),1)*xmin)./(ones(size(Xmat,1),1)*(xmax - xmin));
    end
    
    % Normalization data set
    if userdata.Norm
        szNorm = 'Scaling';
    else
        szNorm = [];
    end
    
    if userdata.Cluster == 1
        supervised = 'supervised';
        bins = Bins;
    else
        supervised = 'unsupervised';
        bins = [];
    end
    
    % Ersin Bayram
    if userdata.ChkFixLen
        fix = userdata.FixLen;
    else
        fix = [];
    end
    
    if ~bPartition
        Maskmat = zeros(length(Ymat),1);
    end            
    
    for nModel=1:size(Maskmat, 2)
        index = find(Maskmat(:, nModel));
        
        %Shuffle the bioact and bins if we are running a randomized experiment
        if userdata.ShuffleFlag
           RandInd = randperm(length(Ymat)); 
           Ymat = Ymat(RandInd);
           Bins = Bins(RandInd);    
        end  
          
        % Training set information
        tmpX = Xmat;
        tmpX(index, :) = [];
        tmpY = Ymat;
        tmpY(index, :) = [];
        tmpBins = Bins;
        tmpBins(index) = [];
        tmpNames = CmpNames;
        tmpNames(index) = [];
        
        % test set information
        tmpXTest = Xmat(index, :);
        tmpBinsTest = Bins(index);
        tmpNamesTest = CmpNames(index);
        tmpYTest = Ymat(index, :);
        
        if strcmp(char(userdata.szMethod),'NNET')
            if (userdata.cutoff(1) >= 100)
                userdata.cutoff = log10(userdata.cutoff);
                tmpY = log10(tmpY);
                tmpYTest = log10(tmpYTest);
            end;
        end;
        
        % choose initial genome (descriptor inclusion-mask) configuration
		% 1. build the initial mask as uniformly distributed
		if not(userdata.ChkFixLen)
            switch userdata.nOpt
            case 1 % Non
                genome = ones(size(tmpX,2),1);
            case 2  % GA has more than one solutions so need a multidimensional genome mask   
                genome = zeros(size(tmpX,2),userdata.SubsetPopulation,userdata.Subsets);
                for i = 1:userdata.Subsets
                    for j=1:userdata.SubsetPopulation
                        if not(userdata.chkFixInitial)
                            genome(:,j,i) = round(rand(size(tmpX,2),1));
                        else
                            locs = randperm(size(tmpX,2));
                            genome(locs(1:userdata.Descriptors),j,i) = 1;
                        end;    
                    end;
                end;  
            otherwise
                if not(userdata.chkFixInitial)
                    genome = zeros(size(tmpX,2),1);
                    genome = round(rand(size(tmpX,2),1));
                else
                    locs = randperm(size(tmpX,2));
                    genome(locs(1:userdata.Descriptors)) = 1;
                end;    
            end;    
		else % 2. build the initial mask with user defined number of descriptors 
            switch userdata.nOpt
            case 2 % GA has more than one solutions so need a multidimensional genome mask
                genome = zeros(size(tmpX,2),userdata.SubsetPopulation,userdata.Subsets);
                for i = 1:userdata.Subsets
                    for j=1:userdata.SubsetPopulation
                        locs = randperm(size(tmpX,2));
                        genome(locs(1:userdata.FixLen),j,i) = 1;
                    end;
                end;  
            otherwise
                locs = randperm(size(tmpX,2));
                genome = zeros(size(tmpX,2),1);
                genome(locs(1:userdata.FixLen)) = 1;
            end;		
		end;
        
        % Initialize the method dependent variables. 
        doneflag = 0;
        Tcurrent = userdata.StartT;
        bestgenome = ones(size(genome,1),1);
        bestfitness = 0;
        tcount = 0;
        currgenome = zeros(size(genome));
        tempgenome = genome;
        bestgenome_movtar = [];
        switch userdata.nOpt
        case 2 % GA    
            currAge = ones(userdata.SubsetPopulation,userdata.Subsets);
            currfitness = zeros(userdata.SubsetPopulation,userdata.Subsets);
        otherwise  
            currfitness = 0;
        end;    
        
        % GA related parameters
        genCounter = 1;
        if userdata.MoveTarget
           Results = [];
           Finalgenome = [];
        end; 
        while not(doneflag)         
            % Modeling Step
            % Use the selected descriptors to perform classification
            [G,P,S] = size(tempgenome);
            tempfitness = zeros(P,S);
            
            % Moving Target implementation. Shuffle the
            % training and test data sets every iteration
            % Eldur = floor(userdata.Generation/(userdata.SubsetPopulation*userdata.Subsets/100*userdata.Elites));
            % normally mod was with respect to Eldur
            if userdata.MoveTarget & (mod(genCounter,userdata.EliteDuration) == 0)
                genCounter = genCounter    
                RandInd = randperm(length(Ymat));
                index = find(Maskmat(:, nModel));
                index = RandInd(1:length(index));
                % Training set information
                tmpX = Xmat;
                tmpX(index, :) = [];
                tmpY = Ymat;
                tmpY(index, :) = [];
                tmpBins = Bins;
                tmpBins(index) = [];
                tmpNames = CmpNames;
                tmpNames(index) = [];
                % test set information
                tmpXTest = Xmat(index, :);
                tmpYTest = Ymat(index, :);
                tmpBinsTest = Bins(index);
                tmpNamesTest = CmpNames(index);   

                % Convert the fitness matrices of newgenomes and oldgenomes into vectors 
                fitList =  reshape(currfitness,userdata.SubsetPopulation*userdata.Subsets,1);
                [fitList, Ind] = sort(fitList);
                % Find the indices of the genomes in the original matrix
                % structure
                bestgenome_movtar =  [bestgenome_movtar currgenome(:,Ind(end))];                       
            end;
            
            for ii=1:P
                for jj=1:S
                    
                    switch char(userdata.szMethod)
                    case 'CM'
                        [class, prd,Dcenter, stats] = cm_model(tmpX(:,find(tempgenome(:,ii,jj))),tmpBins,tmpXTest(:,find(tempgenome(:,ii,jj))), ...
                                                    tmpBinsTest, userdata.ClassBins,userdata.Cluster);
                        % Calculate the fitness of the built model 
                        tempfitness(ii,jj) = stats.correct_rate;     
                    case 'FCM'
                        [class, prd,Member_matrix,Dcenter, stats] = fcm_model(tmpX(:,find(tempgenome(:,ii,jj))),tmpBins,tmpXTest(:,find(tempgenome(:,ii,jj))), ...
                                                    tmpBinsTest, userdata.ClassBins,userdata.Cluster);
                        % Calculate the fitness of the built model 
                        tempfitness(ii,jj) = stats.correct_rate;         
                    case 'KNN'

                    case 'MDS'

                    case 'MLR'
                        
                    case 'NMLR'
                        
                    case 'NNET'
                        [TrPrdAct,TrPrdBin, TstPrdAct,TstPrdBin, stats] = nnet_model(tmpX(:,find(tempgenome(:,ii,jj))), tmpY, tmpBins, ...
                                            'Test',tmpXTest(:,find(tempgenome(:,ii,jj))),tmpYTest,tmpBinsTest, ...
                                            'Cutoff',userdata.cutoff,'FF','Fit');
                         % Calculate the fitness of the built model 
                        if not(isempty(tmpXTest))
                           tempfitness(ii,jj) = stats.correct_rate*stats.test_correct_rate;
                        else
                           tempfitness(ii,jj) = stats.correct_rate;
                        end;                             
                    
                    case 'PCR'
                        dist = pairwise(tmpXTest);
                        for i=1:length(dist)
                            dist(i) = 1/dist(i);
                        end

                    case 'PLS'    
                        [B, BestPrinCompn, E, Fitness, Expterm, ...
                         LogTemp, W, Q, P, MeanStd] = npls(tmpX(:,find(tempgenome(:,ii,jj))), tmpY, ...
                                                userdata.ComponNum, szVal, szSquare, szExp, szInverse, szLog, szNorm);

                        % Calculate the fitness of the built model on the test dataset of
                        % the training . For now use the Fitness(1) as I don't
                        % know what exactly each component is. 
                        if not(isempty(tmpXTest))
                           tempfitness(ii,jj) = Fitness(1);
                        else
                           tempfitness(ii,jj) = Fitness(1);
                        end;       
                    case 'SOM' 
                        [prd, stats, class] = som_model(tmpX, tmpBins, VarNames, ...
                                             VarNames(find(tempgenome(:,ii,jj))), tmpXTest, VarNames, 'testbin', tmpBinsTest);
                        % Calculate the fitness of the built model 
                        if not(isempty(tmpXTest))
                           tempfitness(ii,jj) = stats.correct_rate*stats.test_correct_rate;
                        else
                           tempfitness(ii,jj) = stats.correct_rate;
                        end;                             
                    case 'SVM'
                    
                    end   
                    % Map the classifaction of the models with the initial class allocation. 
                    % bin #3 in the initial classification might be bin #2 in the
                    % model classification
                    
                end; % for Subset Population  
            end;     % for Subset

            % Descriptor Selection Step
            switch userdata.nOpt
            case 1  % non
                doneflag = 1;
            case 2  % GA
                %if tcount > Cycle
                %    disp(['GASOM stucked at a local minimum Iteration #:' num2str(genCounter)]);
                %    doneflag = 1;
                %    return;
                %end;  
                [currgenome, currfitness, currAge, ...
                tempgenome, genCounter,tcount] = ga_feature(currgenome, currfitness, currAge, ...
                                            tempgenome, tempfitness,tcount, genCounter, ...
                                            'SubsetPopulation', userdata.SubsetPopulation, ...
                                            'Generation', userdata.Generation, 'Subsets', userdata.Subsets, ...
                                            'PoolFreq', userdata.PoolFreq, 'PoolDur', userdata.PoolDur, ...
                                            'MaxMutate', userdata.MaxMutate, 'MinMutate', userdata.MinMutate, ... 
                                            'MaxCrossOver', userdata.MaxCrossOver, 'MinCrossOver', userdata.MinCrossOver, ...
                                            'Elites', userdata.Elites, 'Pmutation', userdata.Pmutation, ...
                                            'Pcross', userdata.Pcross, 'MoveTarget', userdata.MoveTarget, 'cycle', userdata.Cycle, ...
                                            'fix', fix, 'Cutoff', userdata.cutoff, 'EliteDuration', userdata.EliteDuration);

                if  genCounter > userdata.Generation
                    doneflag = 1;
                end;                            
            case 3  % Hill Climbing
                if userdata.ChkFixLen
                     [tempgenome, bestgenome, bestfitness,tcount] = hill_feature(bestgenome, bestfitness, tempfitness, tempgenome, ...
                                  userdata.Cycle, tcount, userdata.Mutate, 'Fix_Desc_Num', userdata.FixLen);
                else
                     [tempgenome, bestgenome, bestfitness,tcount] = hill_feature(bestgenome, bestfitness, tempfitness, tempgenome, ...
                                  userdata.Cycle, tcount, userdata.Mutate);
                end;
                if tcount > userdata.Cycle
                      doneflag = 1;
                end;    
            case 4  % SA    
                if userdata.ChkFixLen
                     [currgenome, currfitness, bestgenome, bestfitness, tempgenome, Tcurrent,tcount] = sa_feature(currgenome, currfitness, bestgenome, bestfitness, tempfitness, tempgenome, ...
                                 Tcurrent, userdata.Droprate, userdata.Cycle, tcount, userdata.Mutate, 'Fix_Desc_Num', userdata.FixLen); 
                else
                     [currgenome, currfitness, bestgenome, bestfitness, tempgenome, Tcurrent,tcount] = sa_feature(currgenome, currfitness, bestgenome, bestfitness, tempfitness, tempgenome, ...
                                 Tcurrent, userdata.Droprate, userdata.Cycle, tcount, userdata.Mutate); 
                end;
                if Tcurrent <= userdata.EndT
                      doneflag = 1;
                end;    
            case 5  % Stepwise
                disp('Stepwise KNN');
            end
            
            % No need to do this since every genome is fed with a shuffled input data
            % If moving target is selected, then  
            % if userdata.MoveTarget
            %   % Convert the fitness matrices of newgenomes and oldgenomes into vectors 
            %   fitList =  reshape(currfitness,userdata.SubsetPopulation*userdata.Subsets,1);
            %   [fitList, Ind] = sort(fitList);
            %   Results = [];
            %   Finalgenome = [Finalgenome currgenome(:,Ind(end))];
            %   
            %end; 
            
        end; % while not(doneflag)
        
        % Open the model file
        fmodel = fopen(userdata.szModelName, 'w');
        fprintf(fmodel, '%s\t%s\n', char(strcat(userdata.szMethod, ' model')), char(date));
        
        % Check if the output file name is valid
        try
            userdata.szOutName = get(handles.edtOutFile, 'String');
            % Open the output file
            fout = fopen(userdata.szOutName, 'w');
            fprintf(fout, '%s\t%s\n', char(strcat(userdata.szMethod, ' results')), char(date));
        catch
            szStr = 'Please enter a valid output file name!';
            ShowInf(szStr, handles);
            return;
        end
        
        % Best descriptors are found. Create the model for final statistics
        switch userdata.nOpt
        case 2  % GA
           if userdata.MoveTarget
               bestgenome =  bestgenome_movtar;
           else    
               % Convert the fitness matrices of newgenomes and oldgenomes into vectors 
               fitList =  reshape(currfitness,userdata.SubsetPopulation*userdata.Subsets,1);
               [fitList, Ind] = sort(fitList);
               % Get the indices of the elite genomes
               NumElites = round(userdata.SubsetPopulation*userdata.Subsets/100*userdata.Elites);
               % Map the elite indices back to the old and new genome indices 
               eliteInd = Ind(end:-1:end-NumElites+1);
               % Find the indices of the genomes in the original matrix
               % structure
               bestgenome =  currgenome(:,eliteInd);
           end;
        end;
        for ii=1:size(bestgenome,2)
            switch char(userdata.szMethod)
            case 'CM'         
                % Calculate the fitness of the built model
                [Results(ii).class, Results(ii).prd,Dcenter,Results(ii).stats] = cm_model(tmpX(:,find(bestgenome(:,ii))),tmpBins,tmpXTest(:,find(bestgenome(:,ii))), ...
                                                    tmpBinsTest, userdata.ClassBins,userdata.Cluster);
                % Write the results to the model file      
                switch userdata.Scaling
                case 1 % No scaling
                    FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, ...
                    'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd); 
                case 2 % Normalize to Mahalanobis distance
                    FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, 'Mean', xmean(find(bestgenome(:,ii))), ...
                    'Std', xstd(find(bestgenome(:,ii))),'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd);   
                case 3 % Normalize with respect to data range
                    % Don't know exactly when this scling is used, so will
                    % fill in later if it is useful
                end       
            case 'FCM'         
                % Calculate the fitness of the built model
                [Results(ii).class, Results(ii).prd,Member_matrix,Dcenter,Results(ii).stats] = fcm_model(tmpX(:,find(bestgenome(:,ii))),tmpBins,tmpXTest(:,find(bestgenome(:,ii))), ...
                                                    tmpBinsTest, userdata.ClassBins,userdata.Cluster);
                % Write the results to the model file      
                switch userdata.Scaling
                case 1 % No scaling
                    FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, ...
                    'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd); 
                case 2 % Normalize to Mahalanobis distance
                    FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, 'Mean', xmean(find(bestgenome(:,ii))), ...
                    'Std', xstd(find(bestgenome(:,ii))),'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd);   
                case 3 % Normalize with respect to data range
                    % Don't know exactly when this scling is used, so will
                    % fill in later if it is useful
                end   
            case 'KNN'

            case 'MDS'

            case 'MLR'
                
            case 'NMLR'

            case 'NNET'
                [Results(ii).TrPrdAct,Results(ii).TrPrdBin,Results(ii).TstPrdAct, ...
                 Results(ii).TstPrdBin, Results(ii).stats] = nnet_model(tmpX(:,find(bestgenome(:,ii))), ...
                     tmpY, tmpBins, 'Test',tmpXTest(:,find(bestgenome(:,ii))),tmpYTest,tmpBinsTest, ...
                     'Cutoff',userdata.cutoff,'FF','Fit');
                switch userdata.Scaling
                case 1 % No scaling
                    NNETResult(VarNames(find(bestgenome(:,ii))), tmpNames, Results(ii).stats, tmpY,Results(ii).TrPrdAct, ...
                    tmpBins,Results(ii).TrPrdBin, fout, fmodel,'test', nModel, ...
                    tmpNamesTest,tmpYTest,Results(ii).TstPrdAct, tmpBinsTest, Results(ii).TstPrdBin); 
                case 2 % Normalize to Mahalanobis distance
                    NNETResult(VarNames(find(bestgenome(:,ii))), tmpNames, Results(ii).stats, tmpY,Results(ii).TrPrdAct, ...
                    tmpBins,Results(ii).TrPrdBin, fout, fmodel,'test', nModel, ...
                    tmpNamesTest,tmpYTest,Results(ii).TstPrdAct, tmpBinsTest, Results(ii).TstPrdBin, ...
                    'Mean', xmean(find(bestgenome(:,ii))),'Std', xstd(find(bestgenome(:,ii))));   
                case 3 % Normalize with respect to data range
                    % Don't know exactly when this scaling is used, so will
                    % fill in later if it is useful
                end              
            case 'PCR'
                %dmin = min(dist)
                %dmax = max(dist)
                %dsum = 1/sum(dist)
                %diversity = mean(dist)
                disp(1/sum(dist));
                fprintf(fout, '%f\n', 1/sum(dist));
            case 'PLS'  
                [B, BestPrinCompn, E, Fitness, Expterm, LogTemp, W, Q, P, MeanStd] = npls(Xmat(:,find(bestgenome(:,ii))), ...
                    Ymat, userdata.ComponNum, szVal, szSquare, szExp, szInverse, szLog, szNorm);
                for i=1:size(Fitness, 2)
                    if i == 1
                        tmpStr = ['R^2 = ', num2str(Fitness(i))];
                    elseif i == 2
                        tmpStr = [tmpStr '  Q^2 = ', num2str(Fitness(i))];
                    end
                end
                szStr = strvcat(szStr, tmpStr);
                ShowInf(szStr, handles);
                WritePLSModel(fmodel, B, BestPrinCompn, E, Fitness, ...
                    Expterm, LogTemp, W, Q, P, MeanStd);
                WritePLSOut(fout, CmpNames, E, Ymat, BestPrinCompn, Fitness);                                            
            case 'SOM' 
                % Write the results to the model file
                [Results(ii).prd, Results(ii).stats, Results(ii).class] = som_model(tmpX, tmpBins, VarNames, ...
                                                 VarNames(find(bestgenome(:,ii))), tmpXTest, VarNames, 'testbin', tmpBinsTest);
                switch userdata.Scaling
                case 1 % No scaling
                    SomResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, ...
                    'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd); 
                case 2 % Normalize to Mahalanobis distance
                    SomResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, 'Mean', xmean(find(bestgenome(:,ii))), ...
                    'Std', xstd(find(bestgenome(:,ii))),'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd);   
                case 3 % Normalize with respect to data range
                    % Don't know exactly when this scaling is used, so will
                    % fill in later if it is useful
                end         
            case 'SVM' 
            end  
            if not(isempty(tmpXTest))
                       Weights(ii) = Results(ii).stats.test_correct_rate*Results(ii).stats.correct_rate;
            else
                   Weights(ii) = Results(ii).stats.correct_rate;
            end; 
        end; %for ii=1:size(bestgenome,2)
        
        % If there are more than one models reported, combine their results
        % to a single decision classification
        if size(bestgenome,2) > 1
            bindif = 2;
            ResultSize = size(bestgenome,2);
            NumComp = length(tmpBins); 
            binnum = length(userdata.cutoff) + 1;
            Votes = zeros(binnum,NumComp);
            for i=1:NumComp
                for j = 1: ResultSize
                    Votes(Results(j).class(i), i) = Votes(Results(j).class(i), i) + Weights(j);
                end;
            end;    
            [Votes, SortInd] = sort(Votes,1);
            Calc_Bins = SortInd(end,:)';

            NumTestComp = length(tmpBinsTest); 
            if ~isempty(tmpBinsTest)
                Votes_Test = zeros(binnum,NumTestComp);
                for i=1:NumTestComp
                    for j = 1: ResultSize
                        Votes_Test(Results(j).prd(i), i) = Votes_Test(Results(j).prd(i), i) + Weights(j);
                    end;
                end;    
                [Votes_Test, SortInd] = sort(Votes_Test,1);
                Calc_TestBins = SortInd(end,:)';  
            end;
            
            if isempty(tmpBinsTest)
               Decision = modelstats(Calc_Bins, tmpBins, bindif); 
               Decision.class = Calc_Bins;
               szTmp2 = [' Model_TruePos: ', num2str(Decision.correct_rate), ...
                     ' Model_False_Pos: ', num2str(Decision.false_pos), ...
                     ' Model_False_Neg: ', num2str(Decision.false_neg)];
            else   
               Decision = modelstats_test(Calc_TestBins, tmpBinsTest, Calc_Bins, tmpBins, bindif);
               Decision.class = Calc_Bins;
               Decision.test_class = Calc_TestBins;
               szTmp2 = [' Model_TruePos: ', num2str(Decision.correct_rate), ...
                     ' Model_False_Pos: ', num2str(Decision.false_pos), ...
                     ' Model_False_Neg: ', num2str(Decision.false_neg), ...
                     ' Model_Test_TruePos: ', num2str(Decision.test_correct_rate), ... 
                     ' Model_Test_False_Pos: ', num2str(Decision.test_false_pos), ...
                     ' Model_Test_False_Neg: ', num2str(Decision.test_false_neg) ];
            end;
            szTmp3 = ['Weights: ', num2str(Weights)];
            fprintf(fmodel, '\n%s\n%s\n',szTmp3, szTmp2);
			fprintf(fout, '\n%s\n%s\n',szTmp3, szTmp2);   
            fprintf(fout, '%s\t\n', 'Training Set');
            fprintf(fout, '%s\t%s\t%s\n', 'Name', 'Observed', 'Model Calculation');
            for i=1:length(NumComp)
                fprintf(fout, '%s\t%i\t%i\n', char(tmpNames(i)), tmpBins(i), Decision.class(i));
            end
            if ~isempty(tmpBinsTest)
                fprintf(fout, '\n%s\n', 'Test Set');
                fprintf(fout, '%s\t%s\t%s\n', 'Name', 'Observed', 'Model Prediction');
                for i=1:length(NumTestComp)
                    fprintf(fout, '%s\t%i\t%i\n', char(tmpNamesTest(i)), tmpBinsTest(i), Decision.test_class(i));
                end
            end
            fprintf(fout, '\n');
            fprintf(fmodel, '\n');  
            Bar = '------------------------------------------------';
            fprintf(fmodel, '%s\n', Bar);
            fprintf(fout, '%s\n', Bar);
        end;    % if size(bestgenome,2) > 1 
    end % for nModel
    fclose(fmodel);
else
    % XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    % The following block is designed for using the built model(s) to
    % predict the specify external data
    % XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    
    %check if the prediction file is valid
    userdata.szPrdName = get(handles.edtPrdFile, 'String');
    
    bPartition = -1;
    if ~ischar(userdata.szPrdName) || isempty(userdata.szPrdName)
        szStr = 'Please enter a valid prediction file name!'; 
        showinf(szStr, handles);
        return;
    else
        % Read prediction file to get external data
        if userdata.Val == 1
            [PrdCmpNames PrdVarNames, PrdX] = qsarimport(userdata.szPrdName);
        else
            [PrdCmpNames PrdVarNames, PrdX, PrdY, PrdYname, PrdBins] = qsarimport(userdata.szPrdName, 'bioact', 1, 'cutoffs', userdata.cutoff);
        end 
    end
    
    szPartition = get(handles.edtMaskFile, 'String');
    if ~(~ischar(szPartition) || isempty(szPartition))
        [rownames columnames Maskmat] = qsarimport(szPartition);
        bPartition = 1;
    else
        Maskmat = zeros(length(Ymat),1);
    end;   
    
    switch userdata.Scaling
    case 2 % Normalize to Mahalanobis distance
        xmean = mean(Xmat);
        xstd = std(Xmat);
        Xmat = (Xmat - ones(size(Xmat,1),1)*xmean)./(ones(size(Xmat,1),1)*xstd);
    case 3 % Normalize with respect to data range
        xmin = min(Xmat);
        xmax = max(Xmat);            
        Xmat = (Xmat - ones(size(Xmat,1),1)*xmin)./(ones(size(Xmat,1),1)*(xmax - xmin));
    end
    
    fmodel = fopen(userdata.szModelName, 'r');
    
    % Check if the output file name is valid
    try
        userdata.szOutName = get(handles.edtOutFile, 'String');
        % Open the output file
        fout = fopen(userdata.szOutName, 'w');
        fprintf(fout, '%s\t%s\n', char(strcat(userdata.szMethod, ' results')), char(date));
    catch
        szStr = 'Please enter a valid output file name!';
        ShowInf(szStr, handles);
        return;
    end
    
	fprintf(fout, '%s%s\n', 'The predictions of ', char(userdata.szPrdName));
        
    switch char(userdata.szMethod)
    case {'FCM','CM'}    
         nModel = 0;
            tline = fgetl(fmodel);
            while tline ~= -1
                [str tline] = strtok(tline);
                if strcmpi(str, 'Descriptors:')
                    
                    nModel = nModel + 1;              
                    index = find(Maskmat(:, nModel));
                            
                    % Training set information
                    tmpX = Xmat;
                    tmpX(index, :) = [];           
                    tmpBins = Bins;
                    tmpBins(index) = [];
                    
                    varnamesel = [];
                    i = 1;
                    while(~isempty(tline))
                        [str tline] = strtok(tline);
                        varnamesel{i} = (str);
                        i = i + 1;
                    end
                    newind = zeros(length(varnamesel),1);
                    for i=1:length(varnamesel)
                        for j=1:length(VarNames)
                            if strcmp(varnamesel(i), VarNames(j))
                                newind(i) = j;
                                break;
                            end
                        end
                    end

                    if ~isempty(find(newind == 0))
                        disp(char(varnamesel(find(newind == 0))));
                        disp('Please check your training set and the key descriptors');
                        prd = [];
                        stats = [];
                        class = [];
                        return;
                    end
                    data = Xmat(:, newind);

                    [r c] = size(PrdX);
                    dataTe = zeros(r, length(varnamesel));

                    % define the key descriptor in the prediction set. If key descriptor is missing, all its value will be zeros
                    for i=1:length(varnamesel)
                        for j=1:length(PrdVarNames)
                            if strcmp(varnamesel(i), PrdVarNames(j))
                                dataTe(:, i) = PrdX(:, j);
                                break;
                            end
                        end
                    end
                    if strcmp(char(userdata.szMethod),'FCM')
					  [class, prd, Member_matrix,Dcenter,stats] = fcm_model(data, Bins, dataTe, PrdBins, userdata.ClassBins,userdata.Cluster);
                    else
                      [class, prd, Dcenter,stats] = cm_model(data, Bins, dataTe, PrdBins, userdata.ClassBins,userdata.Cluster);
                    end;
                    % Write the results to the model file      
                    switch userdata.Scaling
                    case 1 % No scaling
                        FcmResult(varnamesel, stats, class, CmpNames, Bins, fout, fmodel, ...
                        'test', nModel, PrdCmpNames, PrdBins, prd); 
                    case 2 % Normalize to Mahalanobis distance
                        FcmResult(varnamesel, stats, class, CmpNames, Bins, fout, fmodel, 'Mean', xmean(newind), ...
                        'Std', xstd(newind),'test', nModel, PrdCmpNames, PrdBins, prd);   
                    case 3 % Normalize with respect to data range
                        % Don't know exactly when this scaling is used, so will
                        % fill in later if it is useful
                    end   
                    
                    fprintf(fout, '\n%s\n', 'Prediction Set');
					fprintf(fout, '%s\t%s\t%s\n', 'Name', 'Actual', 'Prediction');
					for i=1:length(PrdCmpNames)
						fprintf(fout, '%s\t%i\t%i\n', char(PrdCmpNames(i)), PrdBins(i), prd(i));
					end
                end
                tline = fgetl(fmodel);
                if isempty(tline)
                    tline = 1;
                end
            end
            fclose(fmodel);   
    case 'KNN'
    case 'MDS'
    case 'MLR'
    case 'NMLR'
    case 'NNET'   
        nModel = 0;
            tline = fgetl(fmodel);
            while tline ~= -1
                [str tline] = strtok(tline);
                LayerStr = 'Hidden Layer:';
                HLind = findstr(tline,LayerStr);
                if ~isempty(HLind)
                    hiddenlayer = str2num(tline(HLind+length(LayerStr):end));
                end;
                if strcmpi(str, 'Descriptors:')
                    
                    nModel = nModel + 1;              
                    index = find(Maskmat(:, nModel));
                            
                    % Training set information
                    %tmpX = Xmat;
                    %tmpX(index, :) = [];           
                    %tmpBins = Bins;
                    %tmpBins(index) = [];
                    
                    varnamesel = [];
                    i = 1;
                    while(~isempty(tline))
                        [str tline] = strtok(tline);
                        varnamesel{i} = (str);
                        i = i + 1;
                    end
                    
                    trvarind = zeros(length(varnamesel),1);
                    for i=1:length(varnamesel)
                        for j=1:length(VarNames)
                            if strcmp(varnamesel(i), VarNames(j))
                                trvarind(i) = j;
                                break;
                            end
                        end
                    end
                    
                    varind = zeros(length(varnamesel),1);
                    for i=1:length(varnamesel)
                        for j=1:length(PrdVarNames)
                            if strcmp(varnamesel(i), PrdVarNames(j))
                                varind(i) = j;
                                break;
                            end
                        end
                    end
                    [TrPrdAct,TrPrdBin,TstPrdAct,TstPrdBin,stats] = nnet_model(Xmat(:,trvarind), ...
                     Ymat, Bins, 'Test',PrdX(:,varind),PrdY,PrdBins, ...
                     'Cutoff',userdata.cutoff,'FF','Fit','Hidden Layer',hiddenlayer);
                    switch userdata.Scaling
                    case 1 % No scaling
                        NNETResult(PrdVarNames(varind), CmpNames, stats,Ymat,TrPrdAct, ...
                        Bins,TrPrdBin, fout, fmodel,'test', nModel, ...
                        PrdCmpNames,PrdY,TstPrdAct, PrdBins, TstPrdBin); 
                    case 2 % Normalize to Mahalanobis distance
                        NNETResult(VarNames(varind), CmpNames, stats,Ymat,TrPrdAct, ...
                        Bins,TrPrdBin, fout, fmodel,'test', nModel, ...
                        PrdCmpNames,PrdY,TstPrdAct, PrdBins, TstPrdBin, ...
                        'Mean', xmean(varind),'Std', xstd(varind));   
                    case 3 % Normalize with respect to data range
                        % Don't know exactly when this scaling is used, so will
                        % fill in later if it is useful
                    end                  
                end
                tline = fgetl(fmodel);
                if isempty(tline)
                    tline = 1;
                end
            end
            fclose(fmodel);    
    case 'PCR'
    case 'PLS'
    case 'SOM'         
        % Ersin
        switch userdata.nOpt
        case 2  % GA              
            bar = '--------';
            Model_No = 0;
            tline = fgetl(fmodel);
            while tline ~= -1       
                Submodel = 0;       
                while isempty(findstr(bar,tline))
              
                    WeightStr = 'Weights:';
                    Wind = findstr(tline,WeightStr);
                    if ~isempty(Wind)
                        Weights = str2num(tline(Wind+length(WeightStr):end));
                    end;
                    
                    [str tline] = strtok(tline);
                    if strcmpi(str, 'Descriptors:')
                        varnamesel = [];
                        i = 1;
                        while(~isempty(tline))
                            [str tline] = strtok(tline);
                            varnamesel{i} = (str);
                            i = i + 1;
                        end
                        
                        Submodel = Submodel+1;                       
                        if Submodel == 1
                            Model_No = Model_No+1;
                            
                            fprintf(fout, '\n%s\n\n',['Model #', num2str(Model_No)]);
                            
                            if userdata.MoveTarget
                                index = [];
                            else    
                                index = find(Maskmat(:, Model_No));
                            end;    
                            % Training set information
                            tmpX = Xmat;
                            tmpX(index, :) = [];                        
                            tmpBins = Bins;
                            tmpBins(index) = [];
                            %Test set information
                            tmpXTest = Xmat(index, :);
                            tmpYTest = Ymat(index);
                            tmpBinsTest = Bins(index);
                        end;
                                                

                        %[Predict(Submodel).prd, Predict(Submodel).stats, class] = sompredict(tmpX, tmpBins, VarNames, varnamesel, PrdX, PrdVarNames);					               
                        
                        fprintf(fout, '%s\n',['Submodel #', num2str(Submodel)]); 
                     
                        if bPartition < 0
                            if userdata.Val == 1
						        [Predict(Submodel).prd, Predict(Submodel).stats, class] = sompredict(Xmat, Bins, VarNames, varnamesel, PrdX, PrdVarNames);
                            else
                                [Predict(Submodel).prd, Predict(Submodel).stats, class] = sompredict(Xmat, Bins, VarNames, varnamesel, PrdX, PrdVarNames, 'testbin', PrdBins);
                                szStr = ['Prediction Correct rate: ', num2str(Predict(Submodel).stats.test_correct_rate), ...
                                 ' Prediction False Positives: ', num2str(Predict(Submodel).stats.test_false_pos), ...
                                 ' Prediction False Negatives: ', num2str(Predict(Submodel).test_false_neg)];
						        fprintf(fout, '%s\n', szStr);
                            end
                        else
                            %[Predict(Submodel).prd,
                            %Predict(Submodel).stats, class] = sompredict(tmpX, tmpBins, VarNames, varnamesel, tmpXTest, VarNames, 'testbin', tmpBinsTest);
                            if userdata.Val == 1
                                [Predict(Submodel).prd, Predict(Submodel).stats, class] = sompredict(tmpX, tmpBins, VarNames, varnamesel,PrdX, PrdVarNames);
                            else
                                [Predict(Submodel).prd, Predict(Submodel).stats, class] = sompredict(tmpX, tmpBins, VarNames, varnamesel,PrdX, PrdVarNames, 'testbin', PrdBins);
                                
                                %statnames = fieldnames(Predict(Submodel).stats)
                                % Predict empty????   
                                szStr = ['Prediction Correct rate: ', num2str(Predict(Submodel).stats.test_correct_rate), ...
                                 ' Prediction False Positives: ', num2str(Predict(Submodel).stats.test_false_pos), ...
                                 ' Prediction False Negatives: ', num2str(Predict(Submodel).stats.test_false_neg)];
						        fprintf(fout, '%s\n', szStr);
                            end
                        end                    
                        
                        szStr = ['Correct rate: ', num2str(Predict(Submodel).stats.correct_rate), ...
                                 ' False Positives: ', num2str(Predict(Submodel).stats.false_pos), ...
                                 ' False Negatives: ', num2str(Predict(Submodel).stats.false_neg), ...
                                 ' and variable number: ', num2str(Predict(Submodel).stats.varnum)];
						fprintf(fout, '%s\n', szStr);
                        
						szTmp = char(varnamesel(1));
						for i=2:length(varnamesel)
							szTmp = [szTmp, '	', char(varnamesel(i))];
						end
						szStr = strvcat(szStr, szTmp);
						fprintf(fout, '%s\n', char(szTmp)); 
						fprintf(fout, '%s\t%s\t%s\n', 'Name', ' Actual', ' Prediction');
						for i=1:length(PrdCmpNames)
							fprintf(fout, '%s\t%i\t%i\n', char(PrdCmpNames(i)), PrdBins(i), Predict(Submodel).prd(i));
						end
                    end
                    tline = fgetl(fmodel);
                    if ~ischar(tline)
                        tline = -1; break; 
                    end
                end;   
                           
                if (Submodel> 0)
                   szStr = ['Collective Decision for Model# ' num2str(Model_No)];
                   fprintf(fout, '%s\n\n', szStr);
                   
				   ResultSize = Submodel;
				   NumComp = length(PrdCmpNames); 
				   if isempty(Wind)
                        Weights = ones(Submodel,1);
                   end;
            
				   binnum = length(userdata.cutoff) + 1;
				   Votes = zeros(binnum,NumComp);
									
				   for i=1:NumComp
						for j = 1: ResultSize                  
                            Votes(Predict(j).prd(i), i) = Votes(Predict(j).prd(i), i) + Weights(j);
                        end;
				   end;    
				   [Votes, SortInd] = sort(Votes,1);  
				   Final_Bins = SortInd(end,:)';
                   bindif = 2;
                   
                   szStr = ['Model Prediction Correct rate: ' num2str(sum(PrdBins==Final_Bins)/length(Final_Bins))];
                   fprintf(fout, '%s\n', szStr);
                   szStr = ['Model Prediction False_pos: ' num2str(sum((PrdBins-Final_Bins-bindif)>=0)/length(Final_Bins))];
                   fprintf(fout, '%s\n', szStr);
                   szStr = ['Model Prediction False_neg: ' num2str(sum((PrdBins-Final_Bins+bindif)<=0)/length(Final_Bins))];
                   fprintf(fout, '%s\n\n', szStr);      
                   
                   ModelStr = ['Model #', num2str(Model_No), '  Actual', '  Prediction'];
                   fprintf(fout, '%s\n', ModelStr);
				   for i=1:length(PrdCmpNames)
						fprintf(fout, '%s\t%i\t%i\n', char(PrdCmpNames(i)), PrdBins(i), Final_Bins(i));
				   end
                   fprintf(fout, '\n%s\n', '------------------------------------------------');
                   tline = fgetl(fmodel);     
                end;         
            end;
            fclose(fmodel);
            
        otherwise           
            nModel = 0;
            tline = fgetl(fmodel);
            while tline ~= -1
                [str tline] = strtok(tline);
                if strcmpi(str, 'Descriptors:')
                    
                    nModel = nModel + 1;              
                    index = find(Maskmat(:, nModel));
                            
                    % Training set information
                    tmpX = Xmat;
                    tmpX(index, :) = [];           
                    tmpBins = Bins;
                    tmpBins(index) = [];
                    
                    varnamesel = [];
                    i = 1;
                    while(~isempty(tline))
                        [str tline] = strtok(tline);
                        varnamesel{i} = (str);
                        i = i + 1;
                    end
					[prd, stats, class] = sompredict(tmpX, tmpBins, VarNames, varnamesel, PrdX, PrdVarNames);
					szStr = [' Correct rate: ', num2str(stats.correct_rate), ...
                                   ' and variable number: ', num2str(stats.varnum)];
					fprintf(fout, '%s\n', szStr);
					szTmp = char(varnamesel(1));
					for i=2:length(varnamesel)
						szTmp = [szTmp, '	', char(varnamesel(i))];
					end
					szStr = strvcat(szStr, szTmp);
					ShowInf(szStr, handles);
					fprintf(fout, '%s\n', char(szTmp));
					fprintf(fout, '%s\t%s\n', 'Name', 'Prediction');
					for i=1:length(PrdCmpNames)
						fprintf(fout, '%s\t%i\n', char(PrdCmpNames(i)), prd(i));
					end
                end
                tline = fgetl(fmodel);
                if isempty(tline)
                    tline = 1;
                end
            end
            fclose(fmodel);
        end;     
    case 'SVM'
    end
    % ---------------------------------------------------------------
    % the end of prediction block
end
fclose(fout);
% The End of run
%---------------------------------------------------------------------

% --- Executes on button press in btnFileName.
function btnFileName_Callback(hObject, eventdata, handles)
[pname] = GetFileName('txt');
set(handles.edtFileName, 'String', pname);

% --- Executes during object creation, after setting all properties.
function edtFileName_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtFileName_Callback(hObject, eventdata, handles)

% --- Executes on button press in btnFilesPartition.
function btnFilesPartition_Callback(hObject, eventdata, handles)
partition(handles);

% --- Executes during object creation, after setting all properties.
function popFilesTarget_CreateFcn(hObject, eventdata, handles)

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

% --- Executes on selection change in popFilesTarget.
function popFilesTarget_Callback(hObject, eventdata, handles)
szBioact = 'Non';
szBin = 'Non';
szExcl = 'Non';
szCutoff = [];
switch get(hObject, 'Value')
    case 2,     % A4b2
        szBioact = 'start';
        szCutoff = '[200 1000]'; %'[100 250]';
    case 3,     % A7
        szBioact = 'start';
        szCutoff = '[100 600 2000]';
    case 4,     % D2
        szBioact = 'start';
        szCutoff = '[100 1000]'; %'[100 500]';
    case 5,     % D3
        szBioact = 'start';
        szCutoff = '[100 1000]'; %'[100 500]';
    case 6,     % DHFR
        szBioact = 'start';
        szCutoff = '[6.75 7.75]';
    case 7,     % Ganglion
        szBioact = 'start';
        szCutoff = '[20 45]';
    case 8,     % Muscle
        szCutoff = '[20 45]';
    case 9,     % Topliss
        szBioact = 'start';
        szCutoff = '[1.5 2.5 3.5]';
end
set(handles.edtFileBioAct, 'String', szBioact);
set(handles.edtFileBin, 'String', szBin);
set(handles.edtFileExcl, 'String', szExcl);
set(handles.edtFileCutoff, 'String', szCutoff);

% --- Executes during object creation, after setting all properties.
function edtFileBioAct_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtFileBioAct_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function edtFileBin_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtFileBin_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function edtFileExcl_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtFileExcl_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function edtFileCutoff_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtFileCutoff_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function edtMaskFile_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtMaskFile_Callback(hObject, eventdata, handles)

% --- Executes on button press in btnMaskFile.
function btnMaskFile_Callback(hObject, eventdata, handles)
[pname] = GetFileName('txt');
set(handles.edtMaskFile, 'String', pname);

% --- Executes during object creation, after setting all properties.
function edtModelFile_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtModelFile_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function edtPrdFile_CreateFcn(hObject, eventdata, handles)

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtPrdFile_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function edtOutFile_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtOutFile_Callback(hObject, eventdata, handles)

% --- Executes on button press in btnModelFile.
function btnModelFile_Callback(hObject, eventdata, handles)

if get(handles.jobs_build, 'Value')
    [pname] =PutFileName('mod');
else
    [pname] =GetFileName('mod');
end;

set(handles.edtModelFile, 'String', pname);

% --- Executes on button press in btnPrdFile.
function btnPrdFile_Callback(hObject, eventdata, handles)
[pname] = GetFileName('txt');
set(handles.edtPrdFile, 'String', pname);

% --- Executes on button press in btnOutFile.
function btnOutFile_Callback(hObject, eventdata, handles)

if get(handles.jobs_build, 'Value')
    [pname] =PutFileName('out');
else
    [pname] =PutFileName('prd');
end;
set(handles.edtOutFile, 'String', pname);

% --- Executes during object creation, after setting all properties.
function popOptimization_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

% --- Executes on selection change in popOptimization.
function popOptimization_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function popMethod_CreateFcn(hObject, eventdata, handles)

if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

% --- Executes on selection change in popMethod.
function popMethod_Callback(hObject, eventdata, handles)

% --- Executes on button press in btnGsom.
function btnGsom_Callback(hObject, eventdata, handles)
gsomgui;

% --- Executes on button press in btnSetup.
function btnSetup_Callback(hObject, eventdata, handles)
setup;

% --------------------------------------------------------------------
function menu_file_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_file_data_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_file_mdl_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_file_prd_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_file_out_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_file_prt_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_file_exit_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_jobs_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_jobs_bld_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_jobs_prd_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_jobs_run_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_cmp_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_cmp_mth_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_cmp_mth_knn_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 1);

% --------------------------------------------------------------------
function menu_cmp_mth_mlr_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 3);

% --------------------------------------------------------------------
function menu_mth_nmlr_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 4);

% --------------------------------------------------------------------
function menu_mth_pls_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 6);

% --------------------------------------------------------------------
function menu_mth_som_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 7);

% --------------------------------------------------------------------
function menu_cmp_gsom_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_cmp_opt_Callback(hObject, eventdata, handles)

% --------------------------------------------------------------------
function menu_cmp_opt_ga_Callback(hObject, eventdata, handles)
set(handles.popOptimization, 'Value', 2);

% --------------------------------------------------------------------
function menu_cmp_opt_hill_Callback(hObject, eventdata, handles)
set(handles.popOptimization, 'Value', 3);

% --------------------------------------------------------------------
function menu_cmp_opt_sa_Callback(hObject, eventdata, handles)
set(handles.popOptimization, 'Value', 4);

% --------------------------------------------------------------------
function menu_cmp_opt_step_Callback(hObject, eventdata, handles)
set(handles.popOptimization, 'Value', 5);

% --- Executes on button press in btnQuit.
function btnQuit_Callback(hObject, eventdata, handles)
close;

% ----------------------------------------------------
% Get the valid file name
function [pname] = GetFileName(ftype)
% Ask user to select the file by popping up a load file window
if strcmp(ftype,'txt')
    %Defaultfile = {'*.txt'; '*.dat'};
   [FileName, FilePath] = uigetfile({'*.txt;*.dat','Text Files  (*.txt, *.dat)'; ...
                          '*.*', 'All Files (*.*)'}, 'Pick a file');
elseif strcmp(ftype,'mod')
  [FileName, FilePath] = uigetfile({'*.mod', 'Model Files (*.mod)'; ...
                          '*.*', 'All Files (*.*)'}, 'Pick a file');
elseif strcmp(ftype,'out')
  [FileName, FilePath] = uigetfile({'*.out', 'Output Files (*.out)'; ...
                          '*.*', 'All Files (*.*)'}, 'Pick a file');
elseif strcmp(ftype,'prd')       
    [FileName, FilePath] = uigetfile({'*.prd', 'Test Files (*.prd)'; ...
    '*.*', 'All Files (*.*)'}, 'Pick a file');
else
   [FileName, FilePath] = uigetfile({'*.*', 'All Files (*.*)'}, 'Pick a file'); 
end;

%Check whether the selected file name is a valid one.
if ischar(FileName) & (~isempty(FileName));
    % cat the user selectd filename with its path
    pname = strcat(FilePath, FileName);
else
    pname = [];
end

% ----------------------------------------------------
% put the valid file name
function [pname] = PutFileName(ftype)
% Ask user to select the file by popping up a load file window

if strcmp(ftype,'txt')
    %Defaultfile = {'*.txt'; '*.dat'};
   [FileName, FilePath] = uiputfile({'*.txt;*.dat','Text Files  (*.txt, *.dat)'; ...
                          '*.*', 'All Files (*.*)'}, 'Pick a file');
elseif strcmp(ftype,'mod')
  [FileName, FilePath] = uiputfile({'*.mod', 'Model Files (*.mod)'; ...
                          '*.*', 'All Files (*.*)'}, 'Pick a file');
elseif strcmp(ftype,'out')
  [FileName, FilePath] = uiputfile({'*.out', 'Output Files (*.out)'; ...
                          '*.*', 'All Files (*.*)'}, 'Pick a file');
elseif strcmp(ftype,'prd')       
    [FileName, FilePath] = uiputfile({'*.prd', 'Test Files (*.prd)'; ...
    '*.*', 'All Files (*.*)'}, 'Pick a file');
else
   [FileName, FilePath] = uiputfile({'*.*', 'All Files (*.*)'}, 'Pick a file'); 
end;

%Check whether the selected file name is a valid one.
if ischar(FileName) & (~isempty(FileName));
    % cat the user selectd filename with its path
    pname = strcat(FilePath, FileName);
else
    pname = [];
end

% --------------------------------------------------------------------
function menu_jobs_setup_Callback(hObject, eventdata, handles)
setup;
%btnSetup_Callback(hObject, eventdata, handles);
% --------------------------------------------------------------------
function menu_help_Callback(hObject, eventdata, handles)


% --------------------------------------------------------------------
function menu_help_about_Callback(hObject, eventdata, handles)

% ---------------------------------------------------------------
function ShowInf(szInformation, handles)
set(handles.txtInf, 'String', szInformation);
drawnow;


% --------------------------------------------------------------------
function menu_cmp_mth_mds_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 2);

% --------------------------------------------------------------------
function menu_cmp_mth_svm_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 8);


% --------------------------------------------------------------------
function menu_cmp_thm_pcr_Callback(hObject, eventdata, handles)
set(handles.popMethod, 'Value', 5);

% --- Executes on button press in Cluster_job.
function Cluster_job_Callback(hObject, eventdata, handles)
% hObject    handle to Cluster_job (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Cluster_job

% ------- Verify all parameter
% check if the data file name is valid under building model mode or kNN
% algorithm/SOM is chosen
global userdata;
userdata.szDataName = get(handles.edtFileName, 'String');
userdata.szMethod = get(handles.popMethod, 'String');
userdata.szMethod = userdata.szMethod(get(handles.popMethod, 'Value'));

% Check if the model file name is valid
userdata.szModelName = get(handles.edtModelFile, 'String');
if ~ischar(userdata.szModelName) | isempty(userdata.szModelName);
    szStr = 'Please enter a valid model file name!';
    ShowInf(szStr, handles);
    return;
end

% Check if the output file name is valid
userdata.szOutName = get(handles.edtOutFile, 'String');
if ~ischar(userdata.szOutName) | isempty(userdata.szOutName)
    szStr = 'Please enter a valid output file name!';
    ShowInf(szStr, handles);
    return;
else
    % Open the output file
    fout = fopen(userdata.szOutName, 'w');
    fprintf(fout, '%s\t%s\n', char(strcat(userdata.szMethod, ' results')), char(date));
end

%userdata.ShuffleFlag = get(handles.Check_Shuffle,'Value');
userdata.ShuffleFlag = get(handles.Check_Shuffle,'Value'); % returns toggle state of Check_Shuffle

userdata.ModelFlag = get(handles.jobs_build, 'Value');

if  userdata.ModelFlag | strcmp(userdata.szMethod, 'KNN') | strcmp(userdata.szMethod, 'SOM')
    if ~ischar(userdata.szDataName) | isempty(userdata.szDataName)
        szStr = 'Please enter a valid data file name!'; 
        ShowInf(szStr, handles);
        return;
    else
        % define the bioact column
        szStr = get(handles.edtFileBioAct, 'String');
        if strcmpi(szStr, 'start')
            userdata.bioact = 1;
        elseif strcmpi(szStr, 'end')
            userdata.bioact = 'end';
        else
            userdata.bioact = str2num(szStr);
        end
    
        % define the binning column
        userdata.bin = str2num(get(handles.edtFileBin, 'String'));
        % define the exclusive columns
        userdata.exc = str2num(get(handles.edtFileExcl, 'String'));
        % define the cutoff arguments
        userdata.cutoff = str2num(get(handles.edtFileCutoff, 'String'));
    
        % Read file according the specify options
        [CmpNames VarNames, Xmat, Ymat, Ynames, Bins] = qsarimport(userdata.szDataName, 'bioact', ...
           userdata.bioact, 'binning', userdata.bin, 'ex', userdata.exc, 'cutoffs', userdata.cutoff); 
    end
end

% Clear the information show window
szStr = [];
ShowInf(szStr, handles);

% define the partition file and check if it is needed
userdata.PartitionFile = get(handles.edtMaskFile, 'String');
bPartition = 1;
if ~ischar(userdata.PartitionFile) || isempty(userdata.PartitionFile)
    button = questdlg('Do you want to build model without any test set?', ...
        'Confirm Partition File', 'Yes', 'No', 'No');
    if strcmp(button, 'No')
        ShowInf('Please enter a valid partition file name!', handles);
        return;
    end
    bPartition = 0;
end


% get the optimization handles
userdata.nOpt = get(handles.popOptimization, 'Value');

% ------------------------------------------------
% Executes on building models
if userdata.ModelFlag
    
    szStr = ['Compounds: ', num2str(length(CmpNames)), '   Variables: ', num2str(length(VarNames))];
    disp(szStr);
    ShowInf(szStr, handles);
    
    if bPartition
        [rownames columnames Maskmat] = qsarimport(userdata.PartitionFile);
        if size(Xmat, 1) ~= size(Maskmat, 1)
            ShowInf(['The partition file does not fit this data set.', ...
                    ' Check your data/partition files and try again.'], handles);
            return;
        end
        % Eliminate the identical variables in each training set
        IdIndex = [];
        for nSet = 1:size(Maskmat, 2)
            ttmpX = Xmat(find(Maskmat(:, nSet) == 0), :);
            datastd = std(ttmpX);
            IdIndex = [IdIndex, find(datastd < 0.01)];
        end
        IdIndex = unique(IdIndex);
        VarNames(IdIndex) = [];
        Xmat(:, IdIndex) = [];
        ttmpX = [];
        IdIndex = [];
    end

else
    % XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    % The following block is designed for using the built model(s) to
    % predict the specify external data
    % XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    
    %check if the prediction file is valid
    userdata.szPrdName = get(handles.edtPrdFile, 'String');
    if ~ischar(userdata.szPrdName) || isempty(userdata.szPrdName)
        szStr = 'Please enter a valid prediction file name!'; 
        ShowInf(szStr, handles);
        return;
    end
    
end

% Generate a script file to launch jobs on daugter nodes
% ------------------------------------------------------

Default = userdata.szModelName;
% To run on a PC this line should be converted to '\'
if isunix
   Pathloc = findstr(Default,'/');
   Currentpath = pwd;
Currentpath = strcat(Currentpath,'/');
else
   Pathloc = findstr(Default,'\');
   Currentpath = pwd;
   Currentpath = strcat(Currentpath,'\');
end;

Dotloc = findstr(Default,'.');

if (isempty(Pathloc))
	if (isempty(Dotloc))
        if userdata.ModelFlag
            Defaultfile = [Currentpath strcat(userdata.szModelName,'.job')];
        else
            Defaultfile = [Currentpath strcat('Prd_', userdata.szModelName) '.job'];
        end;    
    else
        if userdata.ModelFlag
            Defaultfile = [Currentpath strcat( userdata.szModelName(1:Dotloc(end)-1), '.job')];
        else
            Defaultfile = [Currentpath strcat('Prd_', userdata.szModelName(1:Dotloc(end)-1), '.job')];
        end;  
    end;    
else
    Default = Default(1:Pathloc(end-1));
    if (isempty(Dotloc))
        if userdata.ModelFlag
            Defaultfile = [Default strcat(userdata.szModelName(Pathloc(end)+1:end), '.job')];
        else
            Defaultfile = [Default strcat('Prd_', userdata.szModelName(Pathloc(end)+1:end), '.job')];
        end;
    else
        if userdata.ModelFlag
            Defaultfile = [Default strcat(userdata.szModelName(Pathloc(end)+1:Dotloc(end)-1),'.job')];
        else
            Defaultfile = [Default strcat('Prd_', userdata.szModelName(Pathloc(end)+1:Dotloc(end)-1),'.job')];
        end; 
    end
end    


% Ask user to select the output file to write
[scriptfile, pname] = uiputfile(Defaultfile,...
                      'Save the Executable Script File', 50, 50);

if (scriptfile == 0) | (~ischar(scriptfile))  
   szStr = 'Please enter a valid script file name!';
   ShowInf(szStr, handles);
   return;
else 
    userdata.scriptfile = strcat(pname,scriptfile);
end;

Defaultfile = [pname strcat('Prms_',scriptfile(1:end-4),'.mat')];

% Ask user to select the output file to write
[parameterfile, pname] = uiputfile(Defaultfile,...
                      'Save the Parameters to a Mat File', 50, 50);
userdata.Paramfile = strcat(pname,parameterfile);

if (parameterfile~=0)
  save(userdata.Paramfile,'userdata');
else
  szStr = 'Please enter a valid Parameter file name!'
   ShowInf(szStr, handles);
   return;  
end;

%Just to be sure, remove the pre-existing file
fid = fopen(userdata.scriptfile,'rt');
com1= ['rm ' userdata.scriptfile];
if (fid ~= -1)
[r,s] = unix(com1);
end;
fid = fopen(userdata.scriptfile,'wt');


InitMatlab = '/opt/matlab-6.5/bin/glnx86/matlab -nodisplay -nosplash -nojvm <<E_O_F';
path1 = 'path(path,''/home/ebayram/qsar/ErsinQSAR3'');'; 
path2 = 'path(path,''/home/ebayram/qsar/matlab/somtoolbox2'');'; 

export_library = 'export LD_LIBRARY_PATH=/opt/matlab-6.5/sys/os/glnx86:/opt/matlab-6.5/bin/glnx86:/opt/matlab-6.5/extern/lib/glnx86:/opt/matlab-6.5/sys/java/jre/glnx86/jre/lib/i386/native_threads:/opt/matlab-6.5/sys/java/jre/glnx86/jre/lib/i386/hotspot:/opt/matlab-6.5/sys/java/jre/glnx86/jre/lib/i386:/opt/matlab-6.5/sys/opengl/lib/glnx86';
export_license = 'export LM_LICENSE_FILE=$HOME/wfubmc-matlab-license.dat';
clusterrun = ['Result = cluster_run(''' char(userdata.Paramfile) ''')'];
qsar_dir = 'cd $HOME/qsar/ErsinQSAR3';

count = fprintf(fid,'%s\n','#!/bin/bash');
count = fprintf(fid,'%s\n','#PBS -j oe');
count = fprintf(fid,'%s\n','### send job emails to yourself (MUST HAVE THIS LINE)');
count = fprintf(fid,'%s\n','#PBS -m abe'); 
count = fprintf(fid,'%s\n','#PBS -M ebayram@wfubmc.edu');
count = fprintf(fid,'%s\n','#PBS -W group_list=generalGrp');

if userdata.ModelFlag
	count = fprintf(fid,'%s\n','### Request a maximum real, wall clock time limit of 500 hours');
	count = fprintf(fid,'%s\n','#PBS -l walltime=50:00:00');
	count = fprintf(fid,'%s\n','### Request a maximum CPU processing time limit of 500 hours');
	count = fprintf(fid,'%s\n','#PBS -l cput=50:00:00');
else
    count = fprintf(fid,'%s\n','### Request a maximum real, wall clock time limit of 10 hours');
	count = fprintf(fid,'%s\n','#PBS -l walltime=3:00:00');
	count = fprintf(fid,'%s\n','### Request a maximum CPU processing time limit of 10 hours');
	count = fprintf(fid,'%s\n','#PBS -l cput=3:00:00');
end;

count = fprintf(fid,'%s\n','###  Request one random virtual processor with ethernet :myrenet for myrenet ones');
count = fprintf(fid,'%s\n\n','#PBS -l nodes=1:ethernet');
count = fprintf(fid,'%s\n\n',qsar_dir);
count = fprintf(fid,'%s\n\n',export_library);
count = fprintf(fid,'%s\n\n',export_license);
count = fprintf(fid,'%s\n',InitMatlab);
count = fprintf(fid,'%s\n',path1);
count = fprintf(fid,'%s\n',path2);
count = fprintf(fid,'%s\n',clusterrun);
%count = fprintf(fid,'%s\n','quit');
count = fprintf(fid,'%s\n','E_O_F');

fclose(fid);

% Command to make the script executable
makeexe = ['chmod 777 ' userdata.scriptfile];
[r,s] = unix(makeexe);


% --- Executes on button press in Check_Shuffle.
function Check_Shuffle_Callback(hObject, eventdata, handles)
% hObject    handle to Check_Shuffle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of Check_Shuffle


% --- Executes on button press in Random_Mask.
function Random_Mask_Callback(hObject, eventdata, handles)
% hObject    handle to Random_Mask (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%
% This function reads the model and mask files for a specific run, and 
% finds the best and the median model using the Test correct Rates of 
% the models, and then egenrates the mask files for these two cases by 
% simply repeating the correpsonding partiton column from the original 
% mask file 9 times to have a 90 percent confidence. 
% 
% Inputs:
% -------
% Randoms   : Number of Randoms needed for the confidence interval. 
%             Default value is 9 for 90%
%
%
% Ersin Bayram, August 2003
%---------------------------------------------------------------

% Later will make this a user selected variable. 
% For now set to 9 for 90% confidence runs
%Randoms=  9;
Randoms = str2double(get(handles.Random_No,'String'));     

Defaultfile = '/home/matlab/SBIR/*.*';
[ModelName, FilePath] = uigetfile({Defaultfile, 'Model Files {*.mod}'; ...
    '*.*', 'All Files (*.*)';}, 'Pick the Actual Model File to Load');
modelfile = strcat(FilePath,ModelName);


if (ModelName ~= 0)
	fid   = fopen(modelfile,'r');
	ModelMat = fread(fid);
    ModelMat = char(ModelMat');
else
   display('Invalid File Name! Try again ');
   return;
end;

Matchstr = 'Test Correct rate: ';
Match_Ind = findstr(ModelMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Now we are right at the starting point of each Numeric Test Correct Value
% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TP_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        TP_Str = strcat(TP_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   True_Pos(i) = str2num(TP_Str);
end;   

% Find the models with the mean and best test correct rates
Mean_TP = mean(True_Pos);
%Difference = Mean_TP - True_Pos
[Sorted,Ind] = sort(abs(Mean_TP - True_Pos));
[Best_TP,Best_Ind] = max(True_Pos);

% Pick the first ones if we have moreo than one
Mean_Ind = Ind(1);
Best_Ind = Best_Ind(1);

% Now read the mask file. 

Defaultfile = '/home/matlab/SBIR/Data/*.*';
[FileName, FilePath] = uigetfile({Defaultfile, 'Mask Files {*.dat, *.txt}'; ...
    '*.*', 'All Files (*.*)';}, 'Pick the Model''s Mask File');
maskfile = strcat(FilePath,FileName);

if (FileName ~= 0)
  [rownames, colnames, tData] = dataimport(maskfile);
  rownames(1) =  [];
  colnames(1) =  [];
else
   display('Invalid File Name! Try again ');
   return;
end;


% Generate theMask file for the average Model found for randomization

Defaultfile = strcat(FilePath,ModelName,'_AvgRnd_',FileName);

[FName, FPath] = uiputfile(Defaultfile,'Mask File for Randomized Average');
newmaskfile = strcat(FPath,FName);


if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask, '%s', 'Name');
else
   display('Invalid File Name! Try again ');
   return;
end;

for nSet=1:Randoms
    fprintf(fMask, '\t%6s', char(['R', num2str(nSet)]));
end
fprintf(fMask, '\n');

for nCmp=1:length(rownames)
    fprintf(fMask, '%s', char(rownames(nCmp)));
    for nSet=1:Randoms
        fprintf(fMask, '\t%6d',tData(nCmp,Mean_Ind));
    end
    fprintf(fMask, '\n');
end
fclose(fMask);

% Generate theMask file for the best Model found for randomization
Defaultfile = strcat(FilePath,ModelName,'_BestRnd_',FileName);

[FName, FPath] = uiputfile(Defaultfile, 'Mask File for Randomized Best');
newmaskfile = strcat(FPath,FName);


if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask, '%s', 'Name');
else
   display('Invalid File Name! Try again ');
   return;
end;

for nSet=1:Randoms
    fprintf(fMask, '\t%6s', char(['R', num2str(nSet)]));
end
fprintf(fMask, '\n');

for nCmp=1:length(rownames)
    fprintf(fMask, '%s', char(rownames(nCmp)));
    for nSet=1:Randoms
        fprintf(fMask, '\t%6d',tData(nCmp,Best_Ind));
    end
    fprintf(fMask, '\n');
end
fclose(fMask);


% --- Executes during object creation, after setting all properties.
function Random_No_CreateFcn(hObject, eventdata, handles)
% hObject    handle to Random_No (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function Random_No_Callback(hObject, eventdata, handles)
% hObject    handle to Random_No (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of Random_No as text
%        str2double(get(hObject,'String')) returns contents of Random_No as a double


% --- Executes on button press in ChkHitoLow.
function ChkHitoLow_Callback(hObject, eventdata, handles)
% hObject    handle to ChkHitoLow (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of ChkHitoLow


% --- Executes on button press in btnAnalysis.
function btnAnalysis_Callback(hObject, eventdata, handles)
% hObject    handle to btnAnalysis (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
Analysis;


