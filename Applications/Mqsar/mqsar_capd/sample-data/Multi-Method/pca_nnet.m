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
    [XmatNorm,meanXmat,stdXmat] = prestd(Xmat);
    [Xmattrans,transMat] = prepca(XmatNorm,0.0001);
    
    bioact_trnorm = (bioact_tr - mean(bioact_tr))/std(bioact_tr);
    InputMin = min(Xmattrans);
    InputMax = max(Xmattrans);
    net_predict = newff([InputMin InputMax],[size(Xmattrans,2) 5 1],{'tansig' 'tansig' 'purelin'});
    net_predict.trainParam.epochs = 200;
    net_predict = train(net_predict,Xmattrans,bioact_trnorm);
    net_predict_tr_norm = sim(net_predict,Xmattrans);
    net_predict_tr = poststd(net_predict_tr_norm,meanbioact_tr, stdbioact_tr);
    
    % Error on training  
    err_net_predict_tr = mean(sqrt((net_predict_tr - bioact_tr).^2));
    stderr_net_predict_tr = std(sqrt((net_predict_tr - bioact_tr).^2));
    
    HypLoc = strfind(szDataName,'_');
    szPrdName = strcat(szDataName(1:HypLoc(1)),'valid', szDataName(end-3:end)); 
    
    [PrdCmpNames PrdVarNames, PrdX, PrdY, PrdYname, bin_tst] = qsarimport(szPrdName, 'bioact', 1, 'cutoffs', Cutoff);
    
    index = zeros(length(VarNames),1);
    [r c] = size(PrdX);
    
    dataTe = zeros(r, length(VarNames));
    % define the key descriptor in the prediction set. 
    for i=1:length(VarNames)
        for j=1:length(PrdVarNames)
            if strcmp(VarNames(i), PrdVarNames(j))
                dataTe(:, i) = PrdX(:, j);
                break;
            end
        end
    end
   
    % Error on test
    dataTenorm = trastd(dataTe,meanXmat,stdXmat);
    net_predict_tstnorm = sim(net_predict,dataTenorm);
    net_predict_tst = poststd(net_predict_tstnorm,meanbioact_tr,stdbioact_tr);
    err_net_predict_tst = mean(sqrt((net_predict_tst - PrdY).^2));
    stderr_net_ensemble_tst = std(sqrt((net_predict_tst - PrdY).^2));
end;   
    