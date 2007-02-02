%function A = ensemble(filename,worksheet);
clear all;
close all;
warning off;
filename = 'linear_results.xls';
worklist = {'A4B2', 'D2', 'D3', 'DHFR', 'Topliss'}; 
% 1 = step-wise 2=GPLS 3=GFA 4=PLS
% Inc =  shows which methods will be used in the ensemble decisions
Inc = [1,2,3,4];

for rr=1:length(worklist)
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

    bin_tr = ones(length(bioact_tr),1);
    bin_tst = ones(length(bioact_tst),1);

    for i=1:length(Cutoff)
        if strcmp(worksheet,'A4B2') | strcmp(worksheet,'D2') | strcmp(worksheet,'D3')
          bin_tr(10.^(-bioact_tr)>=Cutoff(i)) = i+1;
          bin_tst(10.^(-bioact_tst)>=Cutoff(i)) = i+1;
        else
          bin_tr(bioact_tr>=Cutoff(i)) = i+1;
          bin_tst(bioact_tst>=Cutoff(i)) = i+1; 
        end;  
    end;

    %=====================================
    % Individual Runs 
    % training
    step_tr = results_tr(:,1);
    gpls_tr = results_tr(:,2);
    gfa_tr = results_tr(:,3);
    pls_tr = results_tr(:,4);
    % test
    step_tst = results_tst(:,1);
    gpls_tst = results_tst(:,2);
    gfa_tst = results_tst(:,3);
    pls_tst = results_tst(:,4);
    %=====================================

    %=====================================
    % Individual Runs error
    % Error on training
    err_step_tr = mean(sqrt((step_tr - bioact_tr).^2));
    err_gpls_tr = mean(sqrt((gpls_tr - bioact_tr).^2));
    err_gfa_tr = mean(sqrt((gfa_tr - bioact_tr).^2));
    err_pls_tr = mean(sqrt((pls_tr - bioact_tr).^2));

    stderr_step_tr = std(sqrt((step_tr - bioact_tr).^2));
    stderr_gpls_tr = std(sqrt((gpls_tr - bioact_tr).^2));
    stderr_gfa_tr = std(sqrt((gfa_tr - bioact_tr).^2));
    stderr_pls_tr = std(sqrt((pls_tr - bioact_tr).^2));

    % Error on test
    err_step_tst = mean(sqrt((step_tst - bioact_tst).^2));
    err_gpls_tst = mean(sqrt((gpls_tst - bioact_tst).^2));
    err_gfa_tst = mean(sqrt((gfa_tst - bioact_tst).^2));
    err_pls_tst = mean(sqrt((pls_tst - bioact_tst).^2));

    stderr_step_tst = std(sqrt((step_tst - bioact_tst).^2));
    stderr_gpls_tst = std(sqrt((gpls_tst - bioact_tst).^2));
    stderr_gfa_tst = std(sqrt((gfa_tst - bioact_tst).^2));
    stderr_pls_tst = std(sqrt((pls_tst - bioact_tst).^2));
    %=====================================

    %=====================================
    % Average based ensemble method
    avg_ensemble_tr = mean(results_tr(:,Inc),2);
    avg_ensemble_tst = mean(results_tst(:,Inc),2);
    % Error on training
    err_avg_ensemble_tr = mean(sqrt((avg_ensemble_tr - bioact_tr).^2));
    stderr_avg_ensemble_tr = std(sqrt((avg_ensemble_tr - bioact_tr).^2));
    % Error on test
    err_avg_ensemble_tst = mean(sqrt((avg_ensemble_tst - bioact_tst).^2));
    stderr_avg_ensemble_tst = std(sqrt((avg_ensemble_tst - bioact_tst).^2));
    %=====================================


    %=====================================
    % Stepwise regression based ensemble method
    step_ensemble_coeff = stepwisefit(results_tr(:,Inc),bioact_tr);
    step_ensemble_tr = results_tr(:,Inc)*step_ensemble_coeff;
    % Error on training
    err_step_ensemble_tr = mean(sqrt((step_ensemble_tr - bioact_tr).^2));
    stderr_step_ensemble_tr = std(sqrt((step_ensemble_tr - bioact_tr).^2));

    % Error on test
    step_ensemble_tst = results_tst(:,Inc)*step_ensemble_coeff;
    err_step_ensemble_tst = mean(sqrt((step_ensemble_tst - bioact_tst).^2));
    stderr_step_ensemble_tst = std(sqrt((step_ensemble_tst - bioact_tst).^2));

    %=====================================
    %regression based ensemble method
    %[regress_ensemble_coeff,regress_ensemble_stats] = robustfit(results_tr(:,Inc),bioact_tr);
    regress_ensemble_coeff = regress(bioact_tr, results_tr(:,Inc));
    regress_ensemble_tr = results_tr(:,Inc) *regress_ensemble_coeff;
    % Error on training
    err_regress_ensemble_tr = mean(sqrt((regress_ensemble_tr - bioact_tr).^2));
    stderr_regress_ensemble_tr = std(sqrt((regress_ensemble_tr - bioact_tr).^2));

    % Error on test
    regress_ensemble_tst = results_tst(:,Inc)*regress_ensemble_coeff;
    err_regress_ensemble_tst = mean(sqrt((regress_ensemble_tst - bioact_tst).^2));
    stderr_regress_ensemble_tst = std(sqrt((regress_ensemble_tst - bioact_tst).^2));

    %=====================================
    %NNET based ensemble method
    % Feed forward backpropagation network is used here

    InputMin = min(results_tr(:,Inc))';
    InputMax = max(results_tr(:,Inc))';
    net = newff([InputMin InputMax],[length(Inc) 4 1],{'tansig' 'tansig' 'purelin'}, 'trainlm','learngdm');
    net.trainParam.epochs = 80;
    net.trainParam.show = nan;
    net = train(net,results_tr(:,Inc)',bioact_tr');
    net_ensemble_tr = sim(net,results_tr(:,Inc)')';
    
    % Error on training
    err_net_ensemble_tr = mean(sqrt((net_ensemble_tr - bioact_tr).^2));
    stderr_net_ensemble_tr = std(sqrt((net_ensemble_tr - bioact_tr).^2));

    % Error on test
    net_ensemble_tst = sim(net,results_tst(:,Inc)')';
    err_net_ensemble_tst = mean(sqrt((net_ensemble_tst - bioact_tst).^2));
    stderr_net_ensemble_tst = std(sqrt((net_ensemble_tst - bioact_tst).^2));
    
    %rbf network
    rbf = newrb(results_tr(:,Inc)',bioact_tr',0, 2.0,50);
    rbf_ensemble_tr = sim(net,results_tr(:,Inc)')';
    
     % Error on training
    err_rbf_ensemble_tr = mean(sqrt((rbf_ensemble_tr - bioact_tr).^2));
    stderr_rbf_ensemble_tr = std(sqrt((rbf_ensemble_tr - bioact_tr).^2));

    % Error on test
    rbf_ensemble_tst = sim(rbf,results_tst(:,Inc)')';
    err_rbf_ensemble_tst = mean(sqrt((rbf_ensemble_tst - bioact_tst).^2));
    stderr_rbf_ensemble_tst = std(sqrt((rbf_ensemble_tst - bioact_tst).^2));
    
    
    step_bin_tr = ones(length(bioact_tr),1);
    step_bin_tst = ones(length(bioact_tst),1);
    pls_bin_tr = ones(length(bioact_tr),1);
    pls_bin_tst = ones(length(bioact_tst),1);
    gpls_bin_tr = ones(length(bioact_tr),1);
    gpls_bin_tst = ones(length(bioact_tst),1);
    gfa_bin_tr = ones(length(bioact_tr),1);
    gfa_bin_tst = ones(length(bioact_tst),1);
    avg_ensemble_bin_tr = ones(length(bioact_tr),1);
    avg_ensemble_bin_tst = ones(length(bioact_tst),1);
    step_ensemble_bin_tr = ones(length(bioact_tr),1);
    step_ensemble_bin_tst = ones(length(bioact_tst),1);
    regress_ensemble_bin_tr = ones(length(bioact_tr),1);
    regress_ensemble_bin_tst = ones(length(bioact_tst),1);
    net_ensemble_bin_tr = ones(length(bioact_tr),1);
    net_ensemble_bin_tst = ones(length(bioact_tst),1);
    rbf_ensemble_bin_tr = ones(length(bioact_tr),1);
    rbf_ensemble_bin_tst = ones(length(bioact_tst),1);
    
    
    for i=1:length(Cutoff)
        if strcmp(worksheet,'A4B2') | strcmp(worksheet,'D2') | strcmp(worksheet,'D3')
          step_bin_tr(10.^(-step_tr)>=Cutoff(i)) = i+1;
          step_bin_tst(10.^(-step_tst)>=Cutoff(i)) = i+1;
          pls_bin_tr(10.^(-pls_tr)>=Cutoff(i)) = i+1;
          pls_bin_tst(10.^(-pls_tst)>=Cutoff(i)) = i+1;
          gpls_bin_tr(10.^(-gpls_tr)>=Cutoff(i)) = i+1;
          gpls_bin_tst(10.^(-gpls_tst)>=Cutoff(i)) = i+1;
          gfa_bin_tr(10.^(-gfa_tr)>=Cutoff(i)) = i+1;
          gfa_bin_tst(10.^(-gfa_tst)>=Cutoff(i)) = i+1;
          avg_ensemble_bin_tr(10.^(-avg_ensemble_tr)>=Cutoff(i)) = i+1;
          avg_ensemble_bin_tst(10.^(-avg_ensemble_tst)>=Cutoff(i)) = i+1;
          step_ensemble_bin_tr(10.^(-step_ensemble_tr)>=Cutoff(i)) = i+1;
          step_ensemble_bin_tst(10.^(-step_ensemble_tst)>=Cutoff(i)) = i+1;
          regress_ensemble_bin_tr(10.^(-regress_ensemble_tr)>=Cutoff(i)) = i+1;
          regress_ensemble_bin_tst(10.^(-regress_ensemble_tst)>=Cutoff(i)) = i+1;
          net_ensemble_bin_tr(10.^(-net_ensemble_tr)>=Cutoff(i)) = i+1;
          net_ensemble_bin_tst(10.^(-net_ensemble_tst)>=Cutoff(i)) = i+1; 
          rbf_ensemble_bin_tr(10.^(-rbf_ensemble_tr)>=Cutoff(i)) = i+1;
          rbf_ensemble_bin_tst(10.^(-rbf_ensemble_tst)>=Cutoff(i)) = i+1;  
        else
          step_bin_tr(step_tr>=Cutoff(i)) = i+1;
          step_bin_tst(step_tst>=Cutoff(i)) = i+1; 
          pls_bin_tr(pls_tr>=Cutoff(i)) = i+1;
          pls_bin_tst(pls_tst>=Cutoff(i)) = i+1; 
          gpls_bin_tr(gpls_tr>=Cutoff(i)) = i+1;
          gpls_bin_tst(gpls_tst>=Cutoff(i)) = i+1; 
          gfa_bin_tr(gfa_tr>=Cutoff(i)) = i+1;
          gfa_bin_tst(gfa_tst>=Cutoff(i)) = i+1; 
          avg_ensemble_bin_tr(avg_ensemble_tr>=Cutoff(i)) = i+1;
          avg_ensemble_bin_tst(avg_ensemble_tst>=Cutoff(i)) = i+1; 
          step_ensemble_bin_tr(step_ensemble_tr>=Cutoff(i)) = i+1;
          step_ensemble_bin_tst(step_ensemble_tst>=Cutoff(i)) = i+1;
          regress_ensemble_bin_tr(regress_ensemble_tr>=Cutoff(i)) = i+1;
          regress_ensemble_bin_tst(regress_ensemble_tst>=Cutoff(i)) = i+1;
          net_ensemble_bin_tr(net_ensemble_tr>=Cutoff(i)) = i+1;
          net_ensemble_bin_tst(net_ensemble_tst>=Cutoff(i)) = i+1;
          rbf_ensemble_bin_tr(rbf_ensemble_tr>=Cutoff(i)) = i+1;
          rbf_ensemble_bin_tst(rbf_ensemble_tst>=Cutoff(i)) = i+1;
        end;  
    end;
    
    results_bin_tr = [step_bin_tr gpls_bin_tr gfa_bin_tr pls_bin_tr];
    results_bin_tst = [step_bin_tst gpls_bin_tst gfa_bin_tst pls_bin_tst];
    
    InputMin = ones(length(Inc), 1);
    InputMax = (length(Cutoff)+1)*ones(length(Inc), 1);
    net2 = newff([InputMin InputMax],[length(Inc) 5 1],{'tansig' 'tansig' 'purelin'}, 'trainlm','learngdm');
    net2.trainParam.epochs = 60;
    net2.trainParam.show = nan;
    net2 = train(net2,results_bin_tr(:,Inc)',bin_tr');
    net2_ensemble_bin_tr = round(sim(net2,results_bin_tr(:,Inc)')');
    net2_ensemble_bin_tst = round(sim(net2,results_bin_tst(:,Inc)')');
    
    
    % Individual correct classification rates
    class_step_tr = sum(step_bin_tr == bin_tr)/length(bin_tr);
    class_step_tst = sum(step_bin_tst == bin_tst)/length(bin_tst);
    class_pls_tr = sum(pls_bin_tr == bin_tr)/length(bin_tr);
    class_pls_tst = sum(pls_bin_tst == bin_tst)/length(bin_tst);
    class_gpls_tr = sum(gpls_bin_tr == bin_tr)/length(bin_tr);
    class_gpls_tst = sum(gpls_bin_tst == bin_tst)/length(bin_tst);
    class_gfa_tr = sum(gfa_bin_tr == bin_tr)/length(bin_tr);
    class_gfa_tst = sum(gfa_bin_tst == bin_tst)/length(bin_tst);
    
    % ensemble correct classification rates
    class_avg_ensemble_tr = sum(avg_ensemble_bin_tr == bin_tr)/length(bin_tr);
    class_avg_ensemble_tst = sum(avg_ensemble_bin_tst == bin_tst)/length(bin_tst);     
    class_regress_ensemble_tr = sum(regress_ensemble_bin_tr == bin_tr)/length(bin_tr);
    class_regress_ensemble_tst = sum(regress_ensemble_bin_tst == bin_tst)/length(bin_tst);     
    class_step_ensemble_tr = sum(step_ensemble_bin_tr == bin_tr)/length(bin_tr);
    class_step_ensemble_tst = sum(step_ensemble_bin_tst == bin_tst)/length(bin_tst);   
    class_net_ensemble_tr = sum(net_ensemble_bin_tr == bin_tr)/length(bin_tr);
    class_net_ensemble_tst = sum(net_ensemble_bin_tst == bin_tst)/length(bin_tst);
    class_rbf_ensemble_tr = sum(rbf_ensemble_bin_tr == bin_tr)/length(bin_tr);
    class_rbf_ensemble_tst = sum(rbf_ensemble_bin_tst == bin_tst)/length(bin_tst);
    class_net2_ensemble_tr = sum(net2_ensemble_bin_tr == bin_tr)/length(bin_tr);
    class_net2_ensemble_tst = sum(net2_ensemble_bin_tst == bin_tst)/length(bin_tst);
    
    
    binnum = length(Cutoff) + 1;
    class_tr = [step_bin_tr gpls_bin_tr gfa_bin_tr pls_bin_tr];
    class_tst = [step_bin_tst gpls_bin_tst gfa_bin_tst pls_bin_tst];

    class_tr = class_tr(:,Inc);
    class_tst = class_tst(:,Inc);
    votes_tr = zeros(length(bioact_tr),binnum);
    votes_tst = zeros(length(bioact_tst),binnum);

    for i=1:length(bioact_tr)
        for j = 1: length(Inc)                 
            votes_tr(i,class_tr(i,j)) = votes_tr(i,class_tr(i,j)) + 1;
        end;
    end;    
    [TrV, SortInd] = sort(votes_tr,2);  
    vote_ensemble_bin_tr = SortInd(:,end);
    
    for i=1:length(bioact_tst)
        for j = 1: length(Inc)                  
            votes_tst(i,class_tst(i,j)) = votes_tst(i,class_tst(i,j)) + 1;
        end;
    end;    

    [TstV, SortInd] = sort(votes_tst,2);  
    vote_ensemble_bin_tst = SortInd(:,end);
    class_vote_ensemble_tr = sum(vote_ensemble_bin_tr == bin_tr)/length(bin_tr);
    class_vote_ensemble_tst = sum(vote_ensemble_bin_tst == bin_tst)/length(bin_tst);
    
    % Individual methods' confusion matrices
    for ii=1:length(Cutoff)+1; 
        for jj=1:length(Cutoff)+1;
            confusion_step_tr(ii,jj) = sum((step_bin_tr == jj).*(bin_tr == ii));
            confusion_step_tst(ii,jj) = sum((step_bin_tst == jj).*(bin_tst == ii));
            confusion_pls_tr(ii,jj) = sum((pls_bin_tr == jj).*(bin_tr == ii));
            confusion_pls_tst(ii,jj) = sum((pls_bin_tst == jj).*(bin_tst == ii));
            confusion_gpls_tr(ii,jj) = sum((gpls_bin_tr == jj).*(bin_tr == ii));
            confusion_gpls_tst(ii,jj) = sum((gpls_bin_tst == jj).*(bin_tst == ii));
            confusion_gfa_tr(ii,jj) = sum((gfa_bin_tr == jj).*(bin_tr == ii));
            confusion_gfa_tst(ii,jj) = sum((gfa_bin_tst == jj).*(bin_tst == ii));
            confusion_avg_ensemble_tr(ii,jj) = sum((avg_ensemble_bin_tr == jj).*(bin_tr == ii));
            confusion_avg_ensemble_tst(ii,jj) = sum((avg_ensemble_bin_tst == jj).*(bin_tst == ii));
            confusion_step_ensemble_tr(ii,jj) = sum((step_ensemble_bin_tr == jj).*(bin_tr == ii));
            confusion_step_ensemble_tst(ii,jj) = sum((step_ensemble_bin_tst == jj).*(bin_tst == ii));
            confusion_regress_ensemble_tr(ii,jj) = sum((regress_ensemble_bin_tr == jj).*(bin_tr == ii));
            confusion_regress_ensemble_tst(ii,jj) = sum((regress_ensemble_bin_tst == jj).*(bin_tst == ii));
            confusion_net_ensemble_tr(ii,jj) = sum((net_ensemble_bin_tr == jj).*(bin_tr == ii));
            confusion_net_ensemble_tst(ii,jj) = sum((net_ensemble_bin_tst == jj).*(bin_tst == ii));
            confusion_rbf_ensemble_tr(ii,jj) = sum((rbf_ensemble_bin_tr == jj).*(bin_tr == ii));
            confusion_rbf_ensemble_tst(ii,jj) = sum((rbf_ensemble_bin_tst == jj).*(bin_tst == ii));
            confusion_vote_ensemble_tr(ii,jj) = sum((vote_ensemble_bin_tr == jj).*(bin_tr == ii));
            confusion_vote_ensemble_tst(ii,jj) = sum((vote_ensemble_bin_tst == jj).*(bin_tst == ii));
            confusion_net2_ensemble_tr(ii,jj) = sum((net2_ensemble_bin_tr == jj).*(bin_tr == ii));
            confusion_net2_ensemble_tst(ii,jj) = sum((net2_ensemble_bin_tst == jj).*(bin_tst == ii));
        end;
    end;    
    
    
    % Create a matrix of results to generate an excel spreadsheet
    F = [err_step_tr stderr_step_tr err_step_tst stderr_step_tst class_step_tr class_step_tst; ...
        err_pls_tr stderr_pls_tr err_pls_tst stderr_pls_tst class_pls_tr class_pls_tst; ...
        err_gpls_tr stderr_gpls_tr err_gpls_tst stderr_gpls_tst class_gpls_tr class_gpls_tst; ...
        err_gfa_tr stderr_gfa_tr  err_gfa_tst stderr_gfa_tst class_gfa_tr class_gfa_tst; ...
        err_avg_ensemble_tr stderr_avg_ensemble_tr err_avg_ensemble_tst, ...
        stderr_avg_ensemble_tst class_avg_ensemble_tr class_avg_ensemble_tst; ... 
        err_regress_ensemble_tr stderr_regress_ensemble_tr err_regress_ensemble_tst, ...
        stderr_regress_ensemble_tst class_regress_ensemble_tr class_regress_ensemble_tst; ...
        err_step_ensemble_tr stderr_step_ensemble_tr err_step_ensemble_tst, ...
        stderr_step_ensemble_tst class_step_ensemble_tr class_step_ensemble_tst; ...
        err_net_ensemble_tr stderr_net_ensemble_tr err_net_ensemble_tst, ...
        stderr_net_ensemble_tst class_net_ensemble_tr class_net_ensemble_tst; ...
        nan nan nan nan class_vote_ensemble_tr class_vote_ensemble_tst; ...
        err_rbf_ensemble_tr stderr_rbf_ensemble_tr err_rbf_ensemble_tst, ...
        stderr_rbf_ensemble_tst class_rbf_ensemble_tr class_rbf_ensemble_tst; ...
        nan nan nan nan class_net2_ensemble_tr class_net2_ensemble_tst;];

    xlswrite('results.xls',F,worksheet,'B2:G12');

    SF = {'Tr Mean RSE' 'Tr Std RSE' 'Tst Mean RSE' 'Tst Std RSE', ...
          'Tr Accuracy' 'Tst Accuracy'};
    SF2 = {'Stepwise' ; ...
           'PLS' ; ...
           'GPLS' ; ...
           'GFA' ; ...
           'Avg. Ensemble' ; ...
           'Regression Ensemble' ; ...
           'Stepwise Ensemble' ; ...
           'NNET Ensemble'; ...
           'Vote Ensemble'; ...
           'RBF Ensemble'; ...
           'ClassNet Ensemble'};
    
    if ~strcmp(worksheet,'Topliss')
        SF3 = {'O1' 'O2' 'O3'}; 
        SF4 = {};
        
        xlswrite('results.xls',{'Step-Tr'; 'A1'; 'A2'; 'A3'; '';...
            'PLS-Tr'; 'A1'; 'A2'; 'A3'; ''; 'GPLS-Tr'; 'A1'; 'A2'; 'A3';...
            ''; 'GFA-Tr'; 'A1'; 'A2'; 'A3'; ''; 'AvgEns-Tr';...
            'A1'; 'A2'; 'A3'; ''; 'RegEns-Tr'; 'A1'; 'A2'; 'A3'; '';'StepEns-Tr';...
            'A1'; 'A2'; 'A3'; '';'NnetEns-Tr'; 'A1'; 'A2'; 'A3'; '';'VoteEns-Tr';...
            'A1'; 'A2'; 'A3'; '';'RBFEns-Tr'; 'A1'; 'A2'; 'A3'; '';'ClassNetEns-Tr'; 'A1'; 'A2'; 'A3'},worksheet,'A23:A76');
        
        xlswrite('results.xls',SF3,worksheet,'B23:D23');
        xlswrite('results.xls',SF3,worksheet,'B28:D28');
        xlswrite('results.xls',SF3,worksheet,'B33:D33');
        xlswrite('results.xls',SF3,worksheet,'B38:D38');
        xlswrite('results.xls',SF3,worksheet,'B43:D43');
        xlswrite('results.xls',SF3,worksheet,'B48:D48');
        xlswrite('results.xls',SF3,worksheet,'B53:D53');
        xlswrite('results.xls',SF3,worksheet,'B58:D58');
        xlswrite('results.xls',SF3,worksheet,'B63:D63');
        xlswrite('results.xls',SF3,worksheet,'B68:D68');
        xlswrite('results.xls',SF3,worksheet,'B73:D73');
        
        xlswrite('results.xls',confusion_step_tr,worksheet,'B24:D26');
        xlswrite('results.xls',confusion_pls_tr,worksheet,'B29:D31');
        xlswrite('results.xls',confusion_gpls_tr,worksheet,'B34:D36');
        xlswrite('results.xls',confusion_gfa_tr,worksheet,'B39:D41');
        xlswrite('results.xls',confusion_avg_ensemble_tr,worksheet,'B44:D46');
        xlswrite('results.xls',confusion_regress_ensemble_tr,worksheet,'B49:D51');
        xlswrite('results.xls',confusion_step_ensemble_tr,worksheet,'B54:D56');
        xlswrite('results.xls',confusion_net_ensemble_tr,worksheet,'B59:D61');
        xlswrite('results.xls',confusion_vote_ensemble_tr,worksheet,'B64:D66');
        xlswrite('results.xls',confusion_rbf_ensemble_tr,worksheet,'B69:D71');
        xlswrite('results.xls',confusion_net2_ensemble_tr,worksheet,'B74:D76');
        
        xlswrite('results.xls',{'Step-Tst'; 'A1'; 'A2'; 'A3'; '';...
            'PLS-Tst'; 'A1'; 'A2'; 'A3'; ''; 'GPLS-Tst'; 'A1'; 'A2'; 'A3';...
            ''; 'GFA-Tst'; 'A1'; 'A2'; 'A3'; ''; 'AvgEns-Tst';...
            'A1'; 'A2'; 'A3'; ''; 'RegEns-Tst'; 'A1'; 'A2'; 'A3'; '';'StepEns-Tst';...
            'A1'; 'A2'; 'A3'; '';'NnetEns-Tst'; 'A1'; 'A2'; 'A3'; '';'VoteEns-Tst';...
            'A1'; 'A2'; 'A3';'';'RBFEns-Tst'; 'A1'; 'A2'; 'A3';'';'ClassNetEns-Tst'; 'A1'; 'A2'; 'A3';},worksheet,'F23:F76');
       
        xlswrite('results.xls',SF3,worksheet,'G23:I23');
        xlswrite('results.xls',SF3,worksheet,'G28:I28');
        xlswrite('results.xls',SF3,worksheet,'G33:I33');
        xlswrite('results.xls',SF3,worksheet,'G38:I38');
        xlswrite('results.xls',SF3,worksheet,'G43:I43');
        xlswrite('results.xls',SF3,worksheet,'G48:I48');
        xlswrite('results.xls',SF3,worksheet,'G53:I53');
        xlswrite('results.xls',SF3,worksheet,'G58:I58');
        xlswrite('results.xls',SF3,worksheet,'G63:I63');    
        xlswrite('results.xls',SF3,worksheet,'G68:I68');
        xlswrite('results.xls',SF3,worksheet,'G73:I73');
        
        xlswrite('results.xls',confusion_step_tst,worksheet,'G24:I26');
        xlswrite('results.xls',confusion_pls_tst,worksheet,'G29:I31');
        xlswrite('results.xls',confusion_gpls_tst,worksheet,'G34:I36');
        xlswrite('results.xls',confusion_gfa_tst,worksheet,'G39:I41');
        xlswrite('results.xls',confusion_avg_ensemble_tst,worksheet,'G44:I46');
        xlswrite('results.xls',confusion_regress_ensemble_tst,worksheet,'G49:I51');
        xlswrite('results.xls',confusion_step_ensemble_tst,worksheet,'G54:I56');
        xlswrite('results.xls',confusion_net_ensemble_tst,worksheet,'G59:I61');
        xlswrite('results.xls',confusion_vote_ensemble_tst,worksheet,'G64:I66');
        xlswrite('results.xls',confusion_rbf_ensemble_tst,worksheet,'G69:I71');
        xlswrite('results.xls',confusion_net2_ensemble_tst,worksheet,'G74:I76');
    else
        SF3 = {'O1' 'O2' 'O3' 'O4'};
        SF4 = {'A1'; 'A2'; 'A3'; 'A4'};
        
        xlswrite('results.xls',{'Step-Tr'; 'A1'; 'A2'; 'A3'; 'A4'; '';...
            'PLS-Tr'; 'A1'; 'A2'; 'A3'; 'A4'; ''; 'GPLS-Tr'; 'A1'; 'A2'; 'A3';'A4';...
            ''; 'GFA-Tr'; 'A1'; 'A2'; 'A3';'A4'; ''; 'AvgEns-Tr';...
            'A1'; 'A2'; 'A3'; 'A4';''; 'RegEns-Tr'; 'A1'; 'A2'; 'A3'; 'A4';'';'StepEns-Tr';...
            'A1'; 'A2'; 'A3'; 'A4';'';'NnetEns-Tr'; 'A1'; 'A2'; 'A3';'A4'; '';'VoteEns-Tr';...
            'A1'; 'A2'; 'A3';'A4';'';'RBFEns-Tr'; 'A1'; 'A2'; 'A3';'A4';'';'ClassNetEns-Tr'; 'A1'; 'A2'; 'A3';'A4'},worksheet,'A23:A88');
        
        xlswrite('results.xls',SF3,worksheet,'B23:E23');
        xlswrite('results.xls',SF3,worksheet,'B29:E29');
        xlswrite('results.xls',SF3,worksheet,'B35:E35');
        xlswrite('results.xls',SF3,worksheet,'B41:E41');
        xlswrite('results.xls',SF3,worksheet,'B47:E47');
        xlswrite('results.xls',SF3,worksheet,'B53:E53');
        xlswrite('results.xls',SF3,worksheet,'B59:E59');
        xlswrite('results.xls',SF3,worksheet,'B65:E65');
        xlswrite('results.xls',SF3,worksheet,'B71:E71');
        xlswrite('results.xls',SF3,worksheet,'B77:E77');
        xlswrite('results.xls',SF3,worksheet,'B83:E83');
        
        xlswrite('results.xls',confusion_step_tr,worksheet,'B24:E27');
        xlswrite('results.xls',confusion_pls_tr,worksheet,'B30:E33');
        xlswrite('results.xls',confusion_gpls_tr,worksheet,'B36:E39');
        xlswrite('results.xls',confusion_gfa_tr,worksheet,'B42:E45');
        xlswrite('results.xls',confusion_avg_ensemble_tr,worksheet,'B48:E51');
        xlswrite('results.xls',confusion_regress_ensemble_tr,worksheet,'B44:E57');
        xlswrite('results.xls',confusion_step_ensemble_tr,worksheet,'B60:E63');
        xlswrite('results.xls',confusion_net_ensemble_tr,worksheet,'B66:E69');
        xlswrite('results.xls',confusion_vote_ensemble_tr,worksheet,'B72:E75');
        
        xlswrite('results.xls',{'Step-Tst'; 'A1'; 'A2'; 'A3'; 'A4'; '';...
            'PLS-Tst'; 'A1'; 'A2'; 'A3'; 'A4'; ''; 'GPLS-Tst'; 'A1'; 'A2'; 'A3';'A4';...
            ''; 'GFA-Tst'; 'A1'; 'A2'; 'A3';'A4'; ''; 'AvgEns-Tst';...
            'A1'; 'A2'; 'A3'; 'A4';''; 'RegEns-Tst'; 'A1'; 'A2'; 'A3'; 'A4';'';'StepEns-Tst';...
            'A1'; 'A2'; 'A3'; 'A4';'';'NnetEns-Tst'; 'A1'; 'A2'; 'A3';'A4'; '';'VoteEns-Tst';...
            'A1'; 'A2'; 'A3';'A4';'RBFEns-Tst'; 'A1'; 'A2'; 'A3';'A4';'';'ClassNetEns-Tst'; 'A1'; 'A2'; 'A3';'A4'},worksheet,'G23:G88');
       
        xlswrite('results.xls',SF3,worksheet,'H23:K23');
        xlswrite('results.xls',SF3,worksheet,'H29:K29');
        xlswrite('results.xls',SF3,worksheet,'H35:K35');
        xlswrite('results.xls',SF3,worksheet,'H41:K41');
        xlswrite('results.xls',SF3,worksheet,'H47:K47');
        xlswrite('results.xls',SF3,worksheet,'H53:K53');
        xlswrite('results.xls',SF3,worksheet,'H59:K59');
        xlswrite('results.xls',SF3,worksheet,'H65:K65');
        xlswrite('results.xls',SF3,worksheet,'H71:K71');    
        xlswrite('results.xls',SF3,worksheet,'H77:K77');
        xlswrite('results.xls',SF3,worksheet,'H83:K83'); 
        
        xlswrite('results.xls',confusion_step_tst,worksheet,'H24:K27');
        xlswrite('results.xls',confusion_pls_tst,worksheet,'H30:K33');
        xlswrite('results.xls',confusion_gpls_tst,worksheet,'H36:K39');
        xlswrite('results.xls',confusion_gfa_tst,worksheet,'H42:K45');
        xlswrite('results.xls',confusion_avg_ensemble_tst,worksheet,'H48:K51');
        xlswrite('results.xls',confusion_regress_ensemble_tst,worksheet,'H54:K57');
        xlswrite('results.xls',confusion_step_ensemble_tst,worksheet,'H60:K63');
        xlswrite('results.xls',confusion_net_ensemble_tst,worksheet,'H66:K69');
        xlswrite('results.xls',confusion_vote_ensemble_tst,worksheet,'H72:K75');
        xlswrite('results.xls',confusion_rbf_ensemble_tst,worksheet,'H78:K81');
        xlswrite('results.xls',confusion_vote_ensemble_tst,worksheet,'H84:K87');
        
    end;
    
    xlswrite('results.xls',SF,worksheet,'B1:G1');

    xlswrite('results.xls',SF2,worksheet,'A2:A12');

end;

