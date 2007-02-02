clear all;
warning off;
filename = 'linear_results.xls';
worklist = {'A4B2', 'D2', 'D3', 'DHFR', 'Topliss'}; 

% 1 = step-wise 2=GPLS 3=GFA 4=PLS
% Inc =  shows which methods will be used in the ensemble decisions
Inc = [1,2,3];

for rr=1:1 %length(worklist)
    worksheet = char(worklist(rr));
    switch worksheet
        case 'A4B2'
           Cutoff = [200 1000]; 
        case 'D2' 
            Cutoff = [100 1000];
        case 'D3'
            Cutoff = [100 1000];
        case 'DHFR'
            Cutoff = [6.75 7.75];
        case 'Topliss'
            Cutoff = [1.5 2.5 3.5];
    end

    [A,txt] = xlsread(filename,worksheet);

    [r,c ] = find(isnan(A));
    if strcmp(worksheet,'DHFR')
        bioact_tr  = A(1:r(1)-1,2);
        bioact_tst = A(r(1)+1:end,2);
        results_tr = A(1:r(1)-1,3:end);
        results_tst = A(r(1)+1:end,3:end);
    else
        bioact_tr  = A(1:r(1)-1,1);
        bioact_tst = A(r(1)+1:end,1);
        results_tr = A(1:r(1)-1,2:end);
        results_tst = A(r(1)+1:end,2:end);
    end;
    
    for i=1:length(Cutoff)
        if strcmp(worksheet,'A4B2') | strcmp(worksheet,'D2') | strcmp(worksheet,'D3')
          bin_tr(10.^(-bioact_tr)>Cutoff(i)) = i+1;
          bin_tst(10.^(-bioact_tst)>Cutoff(i)) = i+1;
        else
          bin_tr(bioact_tr>Cutoff(i)) = i+1;
          bin_tst(bioact_tst>Cutoff(i)) = i+1; 
        end;  
    end;
    
   
    %=====================================
    %NNET based ensemble method
    % Feed forward backpropagation network is used here

    % Read file according the specify options
    [fname,dirpath]  = uigetfile('C:\QSAR\Data\TrTeValid\*.dat','Load training file');
    szDataName = strcat(dirpath,fname);
    [CmpNames, VarNames, Xmat, bioact_tr, Ynames, bin_tr] = qsarimport(szDataName, 'bioact', ...
                                                         1, 'binning', 'Non', ...
                                                         'cutoffs', Cutoff, 'ex', 'Non');      
     % Remove the identical columns                                                 
    [VarNames, Xmat] = qsarfilter(VarNames, Xmat,1.0, 0.0);
    [fname,dirpath]  = uigetfile('C:\QSAR\Data\TrTeValid\*.txt','Load mask file');
    szPartition = strcat(dirpath,fname);
    [rownames columnames Maskmat] = qsarimport(szPartition);
     % Training set information
    index = find(Maskmat(:, 1));
    tmpX = Xmat;
    tmpX(index, :) = [];
    tmpY = bioact_tr;
    tmpY(index, :) = [];
    tmpBins = bin_tr;
    tmpBins(index) = [];
    tmpNames = CmpNames;
    tmpNames(index) = [];

    % test set information
    tmpXTest = Xmat(index, :);
    tmpBinsTest = bin_tr(index);
    tmpNamesTest = CmpNames(index);
    
    locs = randperm(size(Xmat,2));
    genome = zeros(size(Xmat,2),1);
    genome(locs(1:20)) = 1;
    %genome = zeros(size(Xmat,2),1);
    %genome = round(rand(size(Xmat,2),1));
    % Initialize the method dependent variables. 
    doneflag = 0;
    Tcurrent = 100;
    bestgenome = ones(size(genome,1));
    bestfitness = 0;
    tcount = 0;
    currgenome = zeros(size(genome));
    tempgenome = genome;
    currfitness = 0;   
    Droprate = 0.9;
    Cycle = 20;
    Tend = 10^(-2);
    Mutate = 2;
    
    while not(doneflag)         
        % Modeling Step
        % Use the selected descriptors to perform classification
        
        tmpX = Xmat(:,find(tempgenome));     
        InputMin = min(tmpX)';
        InputMax = max(tmpX)';
        net_predict = newff([InputMin InputMax],[size(tmpX,2) 5 1],{'tansig' 'tansig' 'purelin'});
        net_predict.trainParam.epochs = 100;
        net_predict = train(net_predict,tmpX',bioact_tr');
        net_predict_tr = sim(net_predict,tmpX')';
        % Error on training
        tempfitness = mean(sqrt((net_predict_tr - bioact_tr).^2));
        stderr_net_predict_tr = std(sqrt((net_predict_tr - bioact_tr).^2));
        
        [currgenome, currfitness, bestgenome, bestfitness, tempgenome, Tcurrent,tcount] = sa_feature(currgenome, currfitness, bestgenome, bestfitness, tempfitness, tempgenome, ...
                                 Tcurrent, Droprate, Cycle, tcount, Mutate); 
        if Tcurrent <= Tend
              doneflag = 1;
        end; 
        clear net_predict;
    end; % while not(doneflag)  
    % Best descriptors are found. Create the model for final statistics
    tmpX = Xmat(:,find(bestgenome));     
    InputMin = min(tmpX)';
    InputMax = max(tmpX)';
    net_predict = newff([InputMin InputMax],[size(tmpX,2) 10 1],{'tansig' 'tansig' 'purelin'});
    net_predict.trainParam.epochs = 300;
    net_predict = train(net_predict,tmpX',bioact_tr');
    net_predict_tr = sim(net,tmpX')';
    % Error on training
    err_net_predict_tr = mean(sqrt((net_predict_tr - bioact_tr).^2));
    stderr_net_predict_tr = std(sqrt((net_predict_tr - bioact_tr).^2));
    
    HypLoc = strfind(szDataName,'_');
    szPrdName = strcat(szDataName(1:HypLoc(1)),'valid', szDataName(end-3:end)); 
    
    [PrdCmpNames PrdVarNames, PrdX, PrdY, PrdYname, bin_tst] = qsarimport(szPrdName, 'bioact', 1, 'cutoffs', Cutoff);
    
    index = zeros(length(VarNames),1);
    [r c] = size(PrdX);
    
    keyvarnames = VarNames(find(bestgenome));
    
    dataTe = zeros(r, length(keyvarnames));
    % define the key descriptor in the prediction set. 
    for i=1:length(keyvarnames)
        for j=1:length(PrdVarNames)
            if strcmp(keyvarnames(i), PrdVarNames(j))
                dataTe(:, i) = PrdX(:, j);
                break;
            end
        end
    end
   
    % Error on test
    
    net_predict_tst = sim(net_predict,dataTe')';
    err_net_predict_tst = mean(sqrt((net_predict_tst - PrdY).^2));
    stderr_net_ensemble_tst = std(sqrt((net_predict_tst - PrdY).^2));
end;   
    