clear all;
close all;
warning off;
tic,
filename = 'ersin_results2.xls';
worklist = {'a4b2', 'd2', 'd3', 'dhfr', 'topliss'}; 

outputfile = 'ensemble_eb.xls'
classfile = 'ensemble_eb_class.xls'

% Inc =  shows which methods will be used in the prediction based ensemble decisions
% 1 = step-wise 2=GPLS 3=GFA 4=PLS 
Inc = [1,2,3];
% Inc2 =  shows which methods will be used in the classification based ensemble decision
% combination
% 1 = step-wise 2=GPLS 3=GFA 4=PLS 5=SSOM 6=kNN
Inc2 = [1,2,3,5,6];
% EnsInc =  shows which methods will be used in combining the ensembles
% 1 = avg 2=regression 3=step 4=NNET 5=Vote 6=RBF 7=ClassNet 8=FFCNET
% 9=Multiple NNEt with averaging

EnsInc = [1,2,4,5,6,7,8];
Rep = 20;

for rr=1:length(worklist)
    clear avg_*  step_* regr* net* rbf* vote_* *ensemble* *som*
    worksheet = char(worklist(rr));
    switch worksheet
        case 'a4b2'
           Cutoff = [200 1000]; 
        case 'd2' 
            Cutoff = [100 1000];
        case 'd3'
            Cutoff = [100 1000];
        case 'dhfr'
            Cutoff = [6.75 7.75];
        case 'topliss'
            Cutoff = [1.5 2.5 3.5];
    end

    binnum = length(Cutoff) + 1; 
      
    % Need to know whivh bins are good or bad for specificty analysis
    if ~strcmp(worksheet,'dhfr') & ~strcmp(worksheet,'topliss') 
        GB = 1; BB = binnum; 
    else 
        GB = binnum; BB = 1; 
    end; 
       
    [A,txt] = xlsread(filename,worksheet);

    [r,c ] = find(isnan(A));
    if strcmp(worksheet,'dhfr')
        bioact_tr  = A(1:r(1)-1,2);
        bioact_tst = A(r(1)+1:end,2);
        results_tr = A(1:r(1)-1,3:end);
        results_tst = A(r(1)+1:end,3:end);
        Trloc = find(isnan(A(:,1)));
        TrCmpNames = A(1:Trloc(1)-1,1);
        TstCmpNames = A(Trloc(1)+1:end,1);
    else
        bioact_tr  = A(1:r(1)-1,1);
        bioact_tst = A(r(1)+1:end,1);
        results_tr = A(1:r(1)-1,2:end);
        results_tst = A(r(1)+1:end,2:end);
        Trloc = find(strcmp(txt(:,1),''));
        TrCmpNames = txt(2:Trloc(1)-1,1);
        TstCmpNames = txt(Trloc(1)+1:end,1);
    end;

    bin_tr = ones(length(bioact_tr),1);
    bin_tst = ones(length(bioact_tst),1);

    for i=1:length(Cutoff)
        if strcmp(worksheet,'a4b2') | strcmp(worksheet,'d2') | strcmp(worksheet,'d3')
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
    
    step_bin_tr = ones(length(bioact_tr),1);
    step_bin_tst = ones(length(bioact_tst),1);
    pls_bin_tr = ones(length(bioact_tr),1);
    pls_bin_tst = ones(length(bioact_tst),1);
    gpls_bin_tr = ones(length(bioact_tr),1);
    gpls_bin_tst = ones(length(bioact_tst),1);
    gfa_bin_tr = ones(length(bioact_tr),1);
    gfa_bin_tst = ones(length(bioact_tst),1);
    
    if strcmp(worksheet,'a4b2') | strcmp(worksheet,'d2') | strcmp(worksheet,'d3')
        som_bin_tr = results_tr(:,end-2);
        som_bin_tst = results_tst(:,end-2); 
        knn_bin_tr = results_tr(:,end-1);
        knn_bin_tst = results_tst(:,end-1); 
    else
        som_bin_tr = results_tr(:,end-1);
        som_bin_tst = results_tst(:,end-1); 
        knn_bin_tr = results_tr(:,end);
        knn_bin_tst = results_tst(:,end); 
    end;  
       
    avg_ensemble_bin_tr = ones(length(bioact_tr),Rep);
    avg_ensemble_bin_tst = ones(length(bioact_tst),Rep);
    step_ensemble_bin_tr = ones(length(bioact_tr),Rep);
    step_ensemble_bin_tst = ones(length(bioact_tst),Rep);
    regress_ensemble_bin_tr = ones(length(bioact_tr),Rep);
    regress_ensemble_bin_tst = ones(length(bioact_tst),Rep);
    net_ensemble_bin_tr = ones(length(bioact_tr),Rep);
    net_ensemble_bin_tst = ones(length(bioact_tst),Rep);
    rbf_ensemble_bin_tr = ones(length(bioact_tr),Rep);
    rbf_ensemble_bin_tst = ones(length(bioact_tst),Rep);
    
    net_addens_bin_tr = ones(length(bioact_tr),Rep);
    net_addens_bin_tst = ones(length(bioact_tst),Rep);
    rbf_addens_bin_tr = ones(length(bioact_tr),Rep);
    rbf_addens_bin_tst = ones(length(bioact_tst),Rep);
    
    
    for i=1:length(Cutoff)
        if strcmp(worksheet,'a4b2') | strcmp(worksheet,'d2') | strcmp(worksheet,'d3')
          step_bin_tr(10.^(-step_tr)>=Cutoff(i)) = i+1;
          step_bin_tst(10.^(-step_tst)>=Cutoff(i)) = i+1;
          pls_bin_tr(10.^(-pls_tr)>=Cutoff(i)) = i+1;
          pls_bin_tst(10.^(-pls_tst)>=Cutoff(i)) = i+1;
          gpls_bin_tr(10.^(-gpls_tr)>=Cutoff(i)) = i+1;
          gpls_bin_tst(10.^(-gpls_tst)>=Cutoff(i)) = i+1;
          gfa_bin_tr(10.^(-gfa_tr)>=Cutoff(i)) = i+1;
          gfa_bin_tst(10.^(-gfa_tst)>=Cutoff(i)) = i+1; 
        else
          step_bin_tr(step_tr>=Cutoff(i)) = i+1;
          step_bin_tst(step_tst>=Cutoff(i)) = i+1; 
          pls_bin_tr(pls_tr>=Cutoff(i)) = i+1;
          pls_bin_tst(pls_tst>=Cutoff(i)) = i+1; 
          gpls_bin_tr(gpls_tr>=Cutoff(i)) = i+1;
          gpls_bin_tst(gpls_tst>=Cutoff(i)) = i+1; 
          gfa_bin_tr(gfa_tr>=Cutoff(i)) = i+1;
          gfa_bin_tst(gfa_tst>=Cutoff(i)) = i+1; 
        end;  
    end;

    results_bin_tr = [step_bin_tr gpls_bin_tr gfa_bin_tr pls_bin_tr som_bin_tr knn_bin_tr];
    results_bin_tst = [step_bin_tst gpls_bin_tst gfa_bin_tst pls_bin_tst som_bin_tst knn_bin_tst];
    results_bin_tr = results_bin_tr(:,Inc2);
    results_bin_tst = results_bin_tst(:,Inc2);
    
    Goods_Ind_tr = find(bin_tr == GB);
    add_act_tr = [bioact_tr; bioact_tr(Goods_Ind_tr); bioact_tr(Goods_Ind_tr); bioact_tr(Goods_Ind_tr)...
                  ; bioact_tr(Goods_Ind_tr); bioact_tr(Goods_Ind_tr); bioact_tr(Goods_Ind_tr)...
                  ; bioact_tr(Goods_Ind_tr); bioact_tr(Goods_Ind_tr); bioact_tr(Goods_Ind_tr)];
    add_bin_tr = [bin_tr; bin_tr(Goods_Ind_tr); bin_tr(Goods_Ind_tr); bin_tr(Goods_Ind_tr)...
                  ; bin_tr(Goods_Ind_tr); bin_tr(Goods_Ind_tr); bin_tr(Goods_Ind_tr)...
                  ; bin_tr(Goods_Ind_tr); bin_tr(Goods_Ind_tr); bin_tr(Goods_Ind_tr)];
    add_results_bin_tr = [results_bin_tr; results_bin_tr(Goods_Ind_tr,:); results_bin_tr(Goods_Ind_tr,:); results_bin_tr(Goods_Ind_tr,:)...
                          ; results_bin_tr(Goods_Ind_tr,:); results_bin_tr(Goods_Ind_tr,:); results_bin_tr(Goods_Ind_tr,:)...
                          ; results_bin_tr(Goods_Ind_tr,:); results_bin_tr(Goods_Ind_tr,:); results_bin_tr(Goods_Ind_tr,:)];
    add_results_tr = [results_tr(:,Inc); results_tr(Goods_Ind_tr,Inc); results_tr(Goods_Ind_tr,Inc); results_tr(Goods_Ind_tr,Inc)....
                      ; results_tr(Goods_Ind_tr,Inc); results_tr(Goods_Ind_tr,Inc); results_tr(Goods_Ind_tr,Inc)...
                      ; results_tr(Goods_Ind_tr,Inc); results_tr(Goods_Ind_tr,Inc); results_tr(Goods_Ind_tr,Inc)]; 
      
    % Individual correct classification rates
    class_step_tr = sum(step_bin_tr == bin_tr)/length(bin_tr);
    class_step_tst = sum(step_bin_tst == bin_tst)/length(bin_tst);
    class_pls_tr = sum(pls_bin_tr == bin_tr)/length(bin_tr);
    class_pls_tst = sum(pls_bin_tst == bin_tst)/length(bin_tst);
    class_gpls_tr = sum(gpls_bin_tr == bin_tr)/length(bin_tr);
    class_gpls_tst = sum(gpls_bin_tst == bin_tst)/length(bin_tst);
    class_gfa_tr = sum(gfa_bin_tr == bin_tr)/length(bin_tr);
    class_gfa_tst = sum(gfa_bin_tst == bin_tst)/length(bin_tst);   
    class_som_tr = sum(som_bin_tr == bin_tr)/length(bin_tr);
    class_som_tst = sum(som_bin_tst == bin_tst)/length(bin_tst); 
    class_knn_tr = sum(knn_bin_tr == bin_tr)/length(bin_tr);
    class_knn_tst = sum(knn_bin_tst == bin_tst)/length(bin_tst);
    
    % Individual confusion matrices
    for ii=1:binnum 
        for jj=1:binnum
            confusion_step_tr(ii,jj) = sum((step_bin_tr == jj).*(bin_tr == ii));
            confusion_step_tst(ii,jj) = sum((step_bin_tst == jj).*(bin_tst == ii));
            confusion_pls_tr(ii,jj) = sum((pls_bin_tr == jj).*(bin_tr == ii));
            confusion_pls_tst(ii,jj) = sum((pls_bin_tst == jj).*(bin_tst == ii));
            confusion_gpls_tr(ii,jj) = sum((gpls_bin_tr == jj).*(bin_tr == ii));
            confusion_gpls_tst(ii,jj) = sum((gpls_bin_tst == jj).*(bin_tst == ii));
            confusion_gfa_tr(ii,jj) = sum((gfa_bin_tr == jj).*(bin_tr == ii));
            confusion_gfa_tst(ii,jj) = sum((gfa_bin_tst == jj).*(bin_tst == ii));
            confusion_som_tr(ii,jj) = sum((som_bin_tr == jj).*(bin_tr == ii));
            confusion_som_tst(ii,jj) = sum((som_bin_tst == jj).*(bin_tst == ii));
            confusion_knn_tr(ii,jj) = sum((knn_bin_tr == jj).*(bin_tr == ii));
            confusion_knn_tst(ii,jj) = sum((knn_bin_tst == jj).*(bin_tst == ii));
        end;
    end;    
    

    % Perform the ensemble methods Rep times
    for xx = 1:Rep
        
        %=====================================
        % Average based ensemble method
        avg_ensemble_tr(:,xx) = mean(results_tr(:,Inc),2);
        avg_ensemble_tst(:,xx) = mean(results_tst(:,Inc),2);
        % Error on training
        err_avg_ensemble_tr(xx) = mean(sqrt((avg_ensemble_tr(:,xx) - bioact_tr).^2));
        stderr_avg_ensemble_tr(xx) = std(sqrt((avg_ensemble_tr(:,xx) - bioact_tr).^2));
        % Error on test
        err_avg_ensemble_tst(:,xx) = mean(sqrt((avg_ensemble_tst(:,xx) - bioact_tst).^2));
        stderr_avg_ensemble_tst(xx) = std(sqrt((avg_ensemble_tst(:,xx) - bioact_tst).^2));
        %=====================================

        %=====================================
        % Stepwise regression based ensemble method
        step_ensemble_coeff(:,xx) = stepwisefit(results_tr(:,Inc),bioact_tr);
        step_ensemble_tr(:,xx) = results_tr(:,Inc)*step_ensemble_coeff(:,xx);
        % Error on training
        err_step_ensemble_tr(xx) = mean(sqrt((step_ensemble_tr(:,xx) - bioact_tr).^2));
        stderr_step_ensemble_tr(xx) = std(sqrt((step_ensemble_tr(:,xx) - bioact_tr).^2));

        % Error on test
        step_ensemble_tst(:,xx) = results_tst(:,Inc)*step_ensemble_coeff(:,xx);
        err_step_ensemble_tst(xx) = mean(sqrt((step_ensemble_tst(:,xx) - bioact_tst).^2));
        stderr_step_ensemble_tst(xx) = std(sqrt((step_ensemble_tst(:,xx) - bioact_tst).^2));

        %=====================================
        %regression based ensemble method
        %[regress_ensemble_coeff,regress_ensemble_stats] = robustfit(results_tr(:,Inc),bioact_tr);
        regress_ensemble_coeff(:,xx) = regress(bioact_tr, results_tr(:,Inc));
        regress_ensemble_tr(:,xx) = results_tr(:,Inc) *regress_ensemble_coeff(:,xx);
        % Error on training
        err_regress_ensemble_tr(xx) = mean(sqrt((regress_ensemble_tr(:,xx) - bioact_tr).^2));
        stderr_regress_ensemble_tr(xx) = std(sqrt((regress_ensemble_tr(:,xx) - bioact_tr).^2));

        % Error on test
        regress_ensemble_tst(:,xx) = results_tst(:,Inc)*regress_ensemble_coeff(:,xx);
        err_regress_ensemble_tst(xx) = mean(sqrt((regress_ensemble_tst(:,xx) - bioact_tst).^2));
        stderr_regress_ensemble_tst(xx) = std(sqrt((regress_ensemble_tst(:,xx) - bioact_tst).^2));

        %=====================================
        %NNET based ensemble method
        % Feed forward backpropagation network is used here
        InputMin = min(results_tr(:,Inc))';
        InputMax = max(results_tr(:,Inc))';
        net = newff([InputMin InputMax],[length(Inc) 4 1],{'tansig' 'tansig' 'purelin'}, 'trainlm','learngdm');
        net.trainParam.epochs = 100;
        net.trainParam.show = nan;
        net = train(net,add_results_tr',add_act_tr');
        net_ensemble_tr(:,xx) = sim(net,results_tr(:,Inc)')';
        % Error on training
        err_net_ensemble_tr(xx) = mean(sqrt((net_ensemble_tr(:,xx) - bioact_tr).^2));
        stderr_net_ensemble_tr(xx) = std(sqrt((net_ensemble_tr(:,xx) - bioact_tr).^2));
        % Error on test
        net_ensemble_tst(:,xx) = sim(net,results_tst(:,Inc)')';
        err_net_ensemble_tst(xx) = mean(sqrt((net_ensemble_tst(:,xx) - bioact_tst).^2));
        stderr_net_ensemble_tst(xx) = std(sqrt((net_ensemble_tst(:,xx) - bioact_tst).^2));

        %=====================================
        % RBF  based NNET  ensemble method
        rbf = newrb(add_results_tr',add_act_tr',0,4,60,40);
        rbf_ensemble_tr(:,xx) = sim(rbf,results_tr(:,Inc)')';
         % Error on training
        err_rbf_ensemble_tr(xx) = mean(sqrt((rbf_ensemble_tr(:,xx) - bioact_tr).^2));
        stderr_rbf_ensemble_tr(xx) = std(sqrt((rbf_ensemble_tr(:,xx) - bioact_tr).^2));
        % Error on test
        rbf_ensemble_tst(:,xx) = sim(rbf,results_tst(:,Inc)')';
        err_rbf_ensemble_tst(xx) = mean(sqrt((rbf_ensemble_tst(:,xx) - bioact_tst).^2));
        stderr_rbf_ensemble_tst(xx) = std(sqrt((rbf_ensemble_tst(:,xx) - bioact_tst).^2));

        %=====================================
        % Classification  based NNET  ensemble method
        % Feed forward backpropagation network is used here
        InputMin = ones(length(Inc2), 1);
        InputMax = binnum*ones(length(Inc2), 1);
        net2 = newff([InputMin InputMax],[length(Inc2) 5 1],{'tansig' 'tansig' 'purelin'}, 'trainlm','learngdm');
        net2.trainParam.epochs = 80;
        net2.trainParam.show = nan;
        net2 = train(net2,add_results_bin_tr',add_bin_tr');
        tempres = round(sim(net2,results_bin_tr')');
        tempres(tempres > binnum) = binnum;
        tempres(tempres < 1) = 1;
        net2_ensemble_bin_tr(:,xx) = tempres;
        tempres = round(sim(net2,results_bin_tst')');
        tempres(tempres > binnum) = binnum;
        tempres(tempres < 1) = 1;
        net2_ensemble_bin_tst(:,xx) = tempres;
        
        for i=1:length(Cutoff)
            if strcmp(worksheet,'a4b2') | strcmp(worksheet,'d2') | strcmp(worksheet,'d3')
              avg_ensemble_bin_tr(10.^(-avg_ensemble_tr(:,xx))>=Cutoff(i),xx) = i+1;
              avg_ensemble_bin_tst(10.^(-avg_ensemble_tst(:,xx))>=Cutoff(i),xx) = i+1;
              step_ensemble_bin_tr(10.^(-step_ensemble_tr(:,xx))>=Cutoff(i),xx) = i+1;
              step_ensemble_bin_tst(10.^(-step_ensemble_tst(:,xx))>=Cutoff(i),xx) = i+1;
              regress_ensemble_bin_tr(10.^(-regress_ensemble_tr(:,xx))>=Cutoff(i),xx) = i+1;
              regress_ensemble_bin_tst(10.^(-regress_ensemble_tst(:,xx))>=Cutoff(i),xx) = i+1;
              net_ensemble_bin_tr(10.^(-net_ensemble_tr(:,xx))>=Cutoff(i),xx) = i+1;
              net_ensemble_bin_tst(10.^(-net_ensemble_tst(:,xx))>=Cutoff(i),xx) = i+1; 
              rbf_ensemble_bin_tr(10.^(-rbf_ensemble_tr(:,xx))>=Cutoff(i),xx) = i+1;
              rbf_ensemble_bin_tst(10.^(-rbf_ensemble_tst(:,xx))>=Cutoff(i),xx) = i+1;  
            else
              avg_ensemble_bin_tr(avg_ensemble_tr(:,xx)>=Cutoff(i),xx) = i+1;
              avg_ensemble_bin_tst(avg_ensemble_tst(:,xx)>=Cutoff(i),xx) = i+1; 
              step_ensemble_bin_tr(step_ensemble_tr(:,xx)>=Cutoff(i),xx) = i+1;
              step_ensemble_bin_tst(step_ensemble_tst(:,xx)>=Cutoff(i),xx) = i+1;
              regress_ensemble_bin_tr(regress_ensemble_tr(:,xx)>=Cutoff(i),xx) = i+1;
              regress_ensemble_bin_tst(regress_ensemble_tst(:,xx)>=Cutoff(i),xx) = i+1;
              net_ensemble_bin_tr(net_ensemble_tr(:,xx)>=Cutoff(i),xx) = i+1;
              net_ensemble_bin_tst(net_ensemble_tst(:,xx)>=Cutoff(i),xx) = i+1;
              rbf_ensemble_bin_tr(rbf_ensemble_tr(:,xx)>=Cutoff(i),xx) = i+1;
              rbf_ensemble_bin_tst(rbf_ensemble_tst(:,xx)>=Cutoff(i),xx) = i+1;
            end;  
        end;
        
        % ensemble correct classification rates
        class_avg_ensemble_tr(xx) = sum(avg_ensemble_bin_tr(:,xx) == bin_tr)/length(bin_tr);
        class_avg_ensemble_tst(xx) = sum(avg_ensemble_bin_tst(:,xx) == bin_tst)/length(bin_tst);     
        class_regress_ensemble_tr(xx) = sum(regress_ensemble_bin_tr(:,xx) == bin_tr)/length(bin_tr);
        class_regress_ensemble_tst(xx) = sum(regress_ensemble_bin_tst(:,xx) == bin_tst)/length(bin_tst);     
        class_step_ensemble_tr(xx) = sum(step_ensemble_bin_tr(:,xx) == bin_tr)/length(bin_tr);
        class_step_ensemble_tst(xx) = sum(step_ensemble_bin_tst(:,xx) == bin_tst)/length(bin_tst);   
        class_net_ensemble_tr(xx) = sum(net_ensemble_bin_tr(:,xx) == bin_tr)/length(bin_tr);
        class_net_ensemble_tst(xx) = sum(net_ensemble_bin_tst(:,xx) == bin_tst)/length(bin_tst);
        class_rbf_ensemble_tr(xx) = sum(rbf_ensemble_bin_tr(:,xx) == bin_tr)/length(bin_tr);
        class_rbf_ensemble_tst(xx) = sum(rbf_ensemble_bin_tst(:,xx) == bin_tst)/length(bin_tst);
        class_net2_ensemble_tr(xx) = sum(net2_ensemble_bin_tr(:,xx) == bin_tr)/length(bin_tr);
        class_net2_ensemble_tst(xx) = sum(net2_ensemble_bin_tst(:,xx) == bin_tst)/length(bin_tst);

        votes_tr = zeros(length(bioact_tr),binnum);
        votes_tst = zeros(length(bioact_tst),binnum);

        for i=1:length(bioact_tr)
            for j = 1: length(Inc2)                 
                votes_tr(i,results_bin_tr(i,j)) = votes_tr(i,results_bin_tr(i,j)) + 1;
            end;
        end;    

        [TrV, SortInd] = sort(votes_tr,2);  
        vote_ensemble_bin_tr(:,xx) = SortInd(:,end);

        for i=1:length(bioact_tst)
            for j = 1: length(Inc2)                  
                votes_tst(i,results_bin_tst(i,j)) = votes_tst(i,results_bin_tst(i,j)) + 1;
            end;
        end;    

        [TstV, SortInd] = sort(votes_tst,2);  
        vote_ensemble_bin_tst(:,xx) = SortInd(:,end);
        
        class_vote_ensemble_tr(xx) = sum(vote_ensemble_bin_tr(:,xx) == bin_tr)/length(bin_tr);
        class_vote_ensemble_tst(xx) = sum(vote_ensemble_bin_tst(:,xx) == bin_tst)/length(bin_tst);
          
    end;
    
    Merr_avg_ensemble_tr = mean(err_avg_ensemble_tr);
    Mstderr_avg_ensemble_tr = mean(stderr_avg_ensemble_tr); 
    Merr_regress_ensemble_tr = mean(err_regress_ensemble_tr);
    Mstderr_regress_ensemble_tr = mean(stderr_regress_ensemble_tr); 
    Merr_regress_ensemble_tr = mean(err_regress_ensemble_tr);
    Mstderr_regress_ensemble_tr = mean(stderr_regress_ensemble_tr); 
    Merr_step_ensemble_tr = mean(err_step_ensemble_tr);
    Mstderr_step_ensemble_tr = mean(stderr_regress_ensemble_tr); 
    Merr_regress_ensemble_tr = mean(err_regress_ensemble_tr);
    Mstderr_regress_ensemble_tr = mean(stderr_regress_ensemble_tr); 
    Merr_net_ensemble_tr = mean(err_net_ensemble_tr);
    Mstderr_net_ensemble_tr = mean(stderr_net_ensemble_tr); 
    Merr_rbf_ensemble_tr = mean(err_rbf_ensemble_tr);
    Mstderr_rbf_ensemble_tr = mean(stderr_rbf_ensemble_tr); 
    
    Merr_avg_ensemble_tst = mean(err_avg_ensemble_tst);
    Mstderr_avg_ensemble_tst = mean(stderr_avg_ensemble_tst); 
    Merr_regress_ensemble_tst = mean(err_regress_ensemble_tst);
    Mstderr_regress_ensemble_tst = mean(stderr_regress_ensemble_tst); 
    Merr_regress_ensemble_tst = mean(err_regress_ensemble_tst);
    Mstderr_regress_ensemble_tst = mean(stderr_regress_ensemble_tst); 
    Merr_step_ensemble_tst = mean(err_step_ensemble_tst);
    Mstderr_step_ensemble_tst = mean(stderr_regress_ensemble_tst); 
    Merr_regress_ensemble_tst = mean(err_regress_ensemble_tst);
    Mstderr_regress_ensemble_tst = mean(stderr_regress_ensemble_tst); 
    Merr_net_ensemble_tst = mean(err_net_ensemble_tst);
    Mstderr_net_ensemble_tst = mean(stderr_net_ensemble_tst); 
    Merr_rbf_ensemble_tst = mean(err_rbf_ensemble_tst);
    Mstderr_rbf_ensemble_tst = mean(stderr_rbf_ensemble_tst); 
    
    Mavg_ensemble_tr = mean(avg_ensemble_tr,2);
    Mavg_ensemble_tst = mean(avg_ensemble_tst,2);
    Mregress_ensemble_tr = mean(regress_ensemble_tr,2);
    Mregress_ensemble_tst = mean(regress_ensemble_tst,2);
    Mstep_ensemble_tr = mean(step_ensemble_tr,2);
    Mstep_ensemble_tst = mean(step_ensemble_tst,2);
    Mnet_ensemble_tr = mean(net_ensemble_tr,2);
    Mnet_ensemble_tst = mean(net_ensemble_tst,2);
    Mrbf_ensemble_tr = mean(rbf_ensemble_tr,2);
    Mrbf_ensemble_tst = mean(rbf_ensemble_tst,2);
    
    Mavg_ensemble_bin_tr = ones(length(bioact_tr),1);
    Mavg_ensemble_bin_tst = ones(length(bioact_tst),1);
    Mstep_ensemble_bin_tr = ones(length(bioact_tr),1);
    Mstep_ensemble_bin_tst = ones(length(bioact_tst),1);
    Mregress_ensemble_bin_tr = ones(length(bioact_tr),1);
    Mregress_ensemble_bin_tst = ones(length(bioact_tst),1);
    Mnet_ensemble_bin_tr = ones(length(bioact_tr),1);
    Mnet_ensemble_bin_tst = ones(length(bioact_tst),1);
    Mrbf_ensemble_bin_tr = ones(length(bioact_tr),1);
    Mrbf_ensemble_bin_tst = ones(length(bioact_tst),1);
    
    for i=1:length(Cutoff)
        if strcmp(worksheet,'a4b2') | strcmp(worksheet,'d2') | strcmp(worksheet,'d3')
          Mavg_ensemble_bin_tr(10.^(-Mavg_ensemble_tr)>=Cutoff(i)) = i+1;
          Mavg_ensemble_bin_tst(10.^(-Mavg_ensemble_tst)>=Cutoff(i)) = i+1;
          Mstep_ensemble_bin_tr(10.^(-Mstep_ensemble_tr)>=Cutoff(i)) = i+1;
          Mstep_ensemble_bin_tst(10.^(-Mstep_ensemble_tst)>=Cutoff(i)) = i+1;
          Mregress_ensemble_bin_tr(10.^(-Mregress_ensemble_tr)>=Cutoff(i)) = i+1;
          Mregress_ensemble_bin_tst(10.^(-Mregress_ensemble_tst)>=Cutoff(i)) = i+1;
          Mnet_ensemble_bin_tr(10.^(-Mnet_ensemble_tr)>=Cutoff(i)) = i+1;
          Mnet_ensemble_bin_tst(10.^(-Mnet_ensemble_tst)>=Cutoff(i)) = i+1; 
          Mrbf_ensemble_bin_tr(10.^(-Mrbf_ensemble_tr)>=Cutoff(i)) = i+1;
          Mrbf_ensemble_bin_tst(10.^(-Mrbf_ensemble_tst)>=Cutoff(i)) = i+1;  
        else
          Mavg_ensemble_bin_tr(Mavg_ensemble_tr>=Cutoff(i)) = i+1;
          Mavg_ensemble_bin_tst(Mavg_ensemble_tst>=Cutoff(i)) = i+1; 
          Mstep_ensemble_bin_tr(Mstep_ensemble_tr>=Cutoff(i)) = i+1;
          Mstep_ensemble_bin_tstM(step_ensemble_tst>=Cutoff(i)) = i+1;
          Mregress_ensemble_bin_tr(Mregress_ensemble_tr>=Cutoff(i)) = i+1;
          Mregress_ensemble_bin_tst(Mregress_ensemble_tst>=Cutoff(i)) = i+1;
          Mnet_ensemble_bin_tr(Mnet_ensemble_tr>=Cutoff(i)) = i+1;
          Mnet_ensemble_bin_tst(Mnet_ensemble_tst>=Cutoff(i)) = i+1;
          Mrbf_ensemble_bin_tr(Mrbf_ensemble_tr>=Cutoff(i)) = i+1;
          Mrbf_ensemble_bin_tst(Mrbf_ensemble_tst>=Cutoff(i)) = i+1;
        end;  
    end;
    
    % Mean ensemble correct classification rates
    Mclass_avg_ensemble_tr = sum(Mavg_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_avg_ensemble_tst = sum(Mavg_ensemble_bin_tst == bin_tst)/length(bin_tst);     
    Mclass_regress_ensemble_tr = sum(Mregress_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_regress_ensemble_tst = sum(Mregress_ensemble_bin_tst == bin_tst)/length(bin_tst);     
    Mclass_step_ensemble_tr = sum(Mstep_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_step_ensemble_tst = sum(Mstep_ensemble_bin_tst == bin_tst)/length(bin_tst);   
    Mclass_net_ensemble_tr = sum(Mnet_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_net_ensemble_tst = sum(Mnet_ensemble_bin_tst == bin_tst)/length(bin_tst);
    Mclass_rbf_ensemble_tr = sum(Mrbf_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_rbf_ensemble_tst = sum(Mrbf_ensemble_bin_tst == bin_tst)/length(bin_tst);
    
    % Combine the votes 
    Mvotes_tr = zeros(length(bioact_tr),binnum);
    Mvotes_tst = zeros(length(bioact_tst),binnum);
    % Combine the class based nnet results
    Mnet2_votes_tr = zeros(length(bioact_tr),binnum);
    Mnet2_votes_tst = zeros(length(bioact_tst),binnum);
    % Convert the results of the NNET based prediction to classification
    % and then make a majority decision. 
    Mnet1_votes_tr = zeros(length(bioact_tr),binnum);
    Mnet1_votes_tst = zeros(length(bioact_tst),binnum);
    
    for i=1:length(bioact_tr)
        for j = 1: Rep                 
            Mvotes_tr(i,vote_ensemble_bin_tr(i,j)) = Mvotes_tr(i,vote_ensemble_bin_tr(i,j)) + 1;
            Mnet2_votes_tr(i,net2_ensemble_bin_tr(i,j)) = Mnet2_votes_tr(i,net2_ensemble_bin_tr(i,j)) + 1;
            Mnet1_votes_tr(i,net_ensemble_bin_tr(i,j)) = Mnet1_votes_tr(i,net_ensemble_bin_tr(i,j)) + 1;
        end;
    end;  
    
    [TrV, SortInd] = sort(Mvotes_tr,2);  
    Mvote_ensemble_bin_tr = SortInd(:,end);
    [TrV, SortInd] = sort(Mnet2_votes_tr,2);  
    Mnet2_ensemble_bin_tr = SortInd(:,end);  
    [TrV, SortInd] = sort(Mnet1_votes_tr,2);  
    Mnet1_ensemble_bin_tr = SortInd(:,end);
    
    for i=1:length(bioact_tst)
        for j = 1: Rep                  
            Mvotes_tst(i,vote_ensemble_bin_tst(i,j)) = Mvotes_tst(i,vote_ensemble_bin_tst(i,j)) + 1;
            Mnet2_votes_tst(i,net2_ensemble_bin_tst(i,j)) = Mnet2_votes_tst(i,net2_ensemble_bin_tst(i,j)) + 1;
            Mnet1_votes_tst(i,net_ensemble_bin_tst(i,j)) = Mnet1_votes_tst(i,net_ensemble_bin_tst(i,j)) + 1;
        end;
    end;    
   
    [TstV, SortInd] = sort(Mvotes_tst,2);  
    Mvote_ensemble_bin_tst = SortInd(:,end);
    [TstV, SortInd] = sort(Mnet2_votes_tst,2);  
    Mnet2_ensemble_bin_tst = SortInd(:,end);
    [TstV, SortInd] = sort(Mnet1_votes_tst,2);  
    Mnet1_ensemble_bin_tst = SortInd(:,end);
    
    MajLimit = ceil(length(Inc2)/2);
   % Every ensemble method but stepwise is here:  Mstep_ensemble_bin_tst
    MEns_Class_tr = [Mavg_ensemble_bin_tr Mregress_ensemble_bin_tr Mstep_ensemble_bin_tr ...
                     Mnet_ensemble_bin_tr Mvote_ensemble_bin_tr Mrbf_ensemble_bin_tr ...
                     Mnet1_ensemble_bin_tr Mnet2_ensemble_bin_tr];
                 
    MEns_Class_tst = [Mavg_ensemble_bin_tst Mregress_ensemble_bin_tst Mstep_ensemble_bin_tst ...
                     Mnet_ensemble_bin_tst Mvote_ensemble_bin_tst Mrbf_ensemble_bin_tst ...
                     Mnet1_ensemble_bin_tst Mnet2_ensemble_bin_tst];
    
    MEns_Class_tr = MEns_Class_tr(:,EnsInc);
    MEns_Class_tst = MEns_Class_tst(:,EnsInc);
                     
    MEns_Class_votes_tr = zeros(length(bioact_tr),binnum);
    MEns_Class_votes_tst = zeros(length(bioact_tst),binnum);
    
    for i=1:length(bioact_tst)
        for j = 1: size(MEns_Class_tr,2)                  
            MEns_Class_votes_tr(i,MEns_Class_tr(i,j)) = MEns_Class_votes_tr(i,MEns_Class_tr(i,j)) + 1; 
        end;
    end;   
    [TstV, SortInd] = sort(MEns_Class_votes_tr,2);  
    Mmajor_ensemble_bin_tr = SortInd(:,end);
    
    for i=1:length(bioact_tst)
        for j = 1: size(MEns_Class_tst,2)                  
            MEns_Class_votes_tst(i,MEns_Class_tst(i,j)) = MEns_Class_votes_tst(i,MEns_Class_tst(i,j)) + 1;
        end;
    end;  
    
    [TstV, SortInd] = sort(MEns_Class_votes_tst,2);  
    Mmajor_ensemble_bin_tst = SortInd(:,end);
    
    [Tri,Trj] = find(votes_tr > MajLimit);
    [Tsti,Tstj] = find(votes_tst > MajLimit);
    
    %check whether vectoral indexing is doable
    Mmajor_ensemble_bin_tr(Tri) = Trj;
    Mmajor_ensemble_bin_tst(Tsti) = Tstj;
    
    Mclass_vote_ensemble_tr = sum(Mvote_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_vote_ensemble_tst = sum(Mvote_ensemble_bin_tst == bin_tst)/length(bin_tst);
    Mclass_net2_ensemble_tr = sum(Mnet2_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_net2_ensemble_tst = sum(Mnet2_ensemble_bin_tst == bin_tst)/length(bin_tst);   
    Mclass_net1_ensemble_tr = sum(Mnet1_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_net1_ensemble_tst = sum(Mnet1_ensemble_bin_tst == bin_tst)/length(bin_tst);
    Mclass_major_ensemble_tr = sum(Mmajor_ensemble_bin_tr == bin_tr)/length(bin_tr);
    Mclass_major_ensemble_tst = sum(Mmajor_ensemble_bin_tst == bin_tst)/length(bin_tst);
    
    
    for ii=1:binnum 
        for jj=1:binnum
            Mconfusion_avg_ensemble_tr(ii,jj) = sum((Mavg_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_avg_ensemble_tst(ii,jj) = sum((Mavg_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_step_ensemble_tr(ii,jj) = sum((Mstep_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_step_ensemble_tst(ii,jj) = sum((Mstep_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_regress_ensemble_tr(ii,jj) = sum((Mregress_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_regress_ensemble_tst(ii,jj) = sum((Mregress_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_net_ensemble_tr(ii,jj) = sum((Mnet_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_net_ensemble_tst(ii,jj) = sum((Mnet_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_rbf_ensemble_tr(ii,jj) = sum((Mrbf_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_rbf_ensemble_tst(ii,jj) = sum((Mrbf_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_vote_ensemble_tr(ii,jj) = sum((Mvote_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_vote_ensemble_tst(ii,jj) = sum((Mvote_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_net2_ensemble_tr(ii,jj) = sum((Mnet2_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_net2_ensemble_tst(ii,jj) = sum((Mnet2_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_net1_ensemble_tr(ii,jj) = sum((Mnet1_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_net1_ensemble_tst(ii,jj) = sum((Mnet1_ensemble_bin_tst == jj).*(bin_tst == ii));
            Mconfusion_major_ensemble_tr(ii,jj) = sum((Mmajor_ensemble_bin_tr == jj).*(bin_tr == ii));
            Mconfusion_major_ensemble_tst(ii,jj) = sum((Mmajor_ensemble_bin_tst == jj).*(bin_tst == ii));
        end;
    end;   
      
    MR_step_tr = confusion_step_tr(GB,BB);
    MR_step_tst = confusion_step_tst(GB,BB);
    MR_pls_tr = confusion_pls_tr(GB,BB);
    MR_pls_tst = confusion_pls_tst(GB,BB);
    MR_gpls_tr = confusion_gpls_tr(GB,BB);
    MR_gpls_tst = confusion_gpls_tst(GB,BB);
    MR_gfa_tr = confusion_gfa_tr(GB,BB);
    MR_gfa_tst = confusion_gfa_tst(GB,BB);
    MR_som_tr = confusion_som_tr(GB,BB);
    MR_som_tst = confusion_som_tst(GB,BB);
    MR_knn_tr = confusion_knn_tr(GB,BB);
    MR_knn_tst = confusion_knn_tst(GB,BB);
    MR_avg_ensemble_tr = Mconfusion_avg_ensemble_tr(GB,BB);
    MR_avg_ensemble_tst = Mconfusion_avg_ensemble_tst(GB,BB);
    MR_step_ensemble_tr = Mconfusion_step_ensemble_tr(GB,BB);
    MR_step_ensemble_tst = Mconfusion_step_ensemble_tst(GB,BB);
    MR_regress_ensemble_tr = Mconfusion_regress_ensemble_tr(GB,BB);
    MR_regress_ensemble_tst = Mconfusion_regress_ensemble_tst(GB,BB);
    MR_net_ensemble_tr = Mconfusion_net_ensemble_tr(GB,BB);
    MR_net_ensemble_tst = Mconfusion_net_ensemble_tst(GB,BB);
    MR_rbf_ensemble_tr = Mconfusion_rbf_ensemble_tr(GB,BB);
    MR_rbf_ensemble_tst = Mconfusion_rbf_ensemble_tst(GB,BB);
    MR_vote_ensemble_tr = Mconfusion_vote_ensemble_tr(GB,BB);
    MR_vote_ensemble_tst = Mconfusion_vote_ensemble_tst(GB,BB);
    MR_net2_ensemble_tr = Mconfusion_net2_ensemble_tr(GB,BB);
    MR_net2_ensemble_tst = Mconfusion_net2_ensemble_tst(GB,BB);
    MR_net1_ensemble_tr = Mconfusion_net1_ensemble_tr(GB,BB);
    MR_net1_ensemble_tst = Mconfusion_net1_ensemble_tst(GB,BB);
    MR_major_ensemble_tr = Mconfusion_major_ensemble_tr(GB,BB);
    MR_major_ensemble_tst = Mconfusion_major_ensemble_tst(GB,BB);
    
    MR_Tr = [MR_step_tr; MR_pls_tr; MR_gpls_tr; MR_gfa_tr; MR_som_tr; MR_knn_tr; ...
             MR_avg_ensemble_tr; MR_regress_ensemble_tr; MR_step_ensemble_tr; ...
             MR_net_ensemble_tr; MR_vote_ensemble_tr; MR_rbf_ensemble_tr; ...
             MR_net2_ensemble_tr; MR_net1_ensemble_tr;MR_major_ensemble_tr]; 
    MR_Tst = [MR_step_tst; MR_pls_tst; MR_gpls_tst; MR_gfa_tst; MR_som_tst; MR_knn_tst; ...
             MR_avg_ensemble_tst; MR_regress_ensemble_tst; MR_step_ensemble_tst; ...
             MR_net_ensemble_tst; MR_vote_ensemble_tst; MR_rbf_ensemble_tst; ...
             MR_net2_ensemble_tst; MR_net1_ensemble_tst;MR_major_ensemble_tst]; 
 
    FHR_step_tr = confusion_step_tr(BB,GB);
    FHR_step_tst = confusion_step_tst(BB,GB);
    FHR_pls_tr = confusion_pls_tr(BB,GB);
    FHR_pls_tst = confusion_pls_tst(BB,GB);
    FHR_gpls_tr = confusion_gpls_tr(BB,GB);
    FHR_gpls_tst = confusion_gpls_tst(BB,GB);
    FHR_gfa_tr = confusion_gfa_tr(BB,GB);
    FHR_gfa_tst = confusion_gfa_tst(BB,GB);
    FHR_som_tr = confusion_som_tr(BB,GB);
    FHR_som_tst = confusion_som_tst(BB,GB);
    FHR_knn_tr = confusion_knn_tr(BB,GB);
    FHR_knn_tst = confusion_knn_tst(BB,GB);
    FHR_avg_ensemble_tr = Mconfusion_avg_ensemble_tr(BB,GB);
    FHR_avg_ensemble_tst = Mconfusion_avg_ensemble_tst(BB,GB);
    FHR_step_ensemble_tr = Mconfusion_step_ensemble_tr(BB,GB);
    FHR_step_ensemble_tst = Mconfusion_step_ensemble_tst(BB,GB);
    FHR_regress_ensemble_tr = Mconfusion_regress_ensemble_tr(BB,GB);
    FHR_regress_ensemble_tst = Mconfusion_regress_ensemble_tst(BB,GB);
    FHR_net_ensemble_tr = Mconfusion_net_ensemble_tr(BB,GB);
    FHR_net_ensemble_tst = Mconfusion_net_ensemble_tst(BB,GB);
    FHR_rbf_ensemble_tr = Mconfusion_rbf_ensemble_tr(BB,GB);
    FHR_rbf_ensemble_tst = Mconfusion_rbf_ensemble_tst(BB,GB);
    FHR_vote_ensemble_tr = Mconfusion_vote_ensemble_tr(BB,GB);
    FHR_vote_ensemble_tst = Mconfusion_vote_ensemble_tst(BB,GB);
    FHR_net2_ensemble_tr = Mconfusion_net2_ensemble_tr(BB,GB);
    FHR_net2_ensemble_tst = Mconfusion_net2_ensemble_tst(BB,GB);
    FHR_net1_ensemble_tr = Mconfusion_net1_ensemble_tr(BB,GB);
    FHR_net1_ensemble_tst = Mconfusion_net1_ensemble_tst(BB,GB);
    FHR_major_ensemble_tr = Mconfusion_major_ensemble_tr(BB,GB);
    FHR_major_ensemble_tst = Mconfusion_major_ensemble_tst(BB,GB);

    FHR_Tr = [FHR_step_tr; FHR_pls_tr; FHR_gpls_tr; FHR_gfa_tr; FHR_som_tr; FHR_knn_tr; ...
             FHR_avg_ensemble_tr; FHR_regress_ensemble_tr; FHR_step_ensemble_tr; ...
             FHR_net_ensemble_tr; FHR_vote_ensemble_tr; FHR_rbf_ensemble_tr; ...
             FHR_net2_ensemble_tr; FHR_net1_ensemble_tr;FHR_major_ensemble_tr]; 
    FHR_Tst = [FHR_step_tst; FHR_pls_tst; FHR_gpls_tst; FHR_gfa_tst; FHR_som_tst; FHR_knn_tst; ...
             FHR_avg_ensemble_tst; FHR_regress_ensemble_tst; FHR_step_ensemble_tst; ...
             FHR_net_ensemble_tst; FHR_vote_ensemble_tst; FHR_rbf_ensemble_tst; ...
             FHR_net2_ensemble_tst; FHR_net1_ensemble_tst;FHR_major_ensemble_tst]; 
    
    % Write miss rate and false hit rate to spreadsheet
    xlswrite(outputfile,[MR_Tr MR_Tst FHR_Tr FHR_Tst],worksheet,'H2:K16');
             
    % Create a matrix of results to generate an excel spreadsheet
    F = [err_step_tr stderr_step_tr err_step_tst stderr_step_tst class_step_tr class_step_tst; ...
        err_pls_tr stderr_pls_tr err_pls_tst stderr_pls_tst class_pls_tr class_pls_tst; ...
        err_gpls_tr stderr_gpls_tr err_gpls_tst stderr_gpls_tst class_gpls_tr class_gpls_tst; ...
        err_gfa_tr stderr_gfa_tr  err_gfa_tst stderr_gfa_tst class_gfa_tr class_gfa_tst; ...
        nan nan nan nan class_som_tr class_som_tst;  ...
        nan nan nan nan class_knn_tr class_knn_tst; ...
        Merr_avg_ensemble_tr Mstderr_avg_ensemble_tr Merr_avg_ensemble_tst, ...
        Mstderr_avg_ensemble_tst Mclass_avg_ensemble_tr Mclass_avg_ensemble_tst; ... 
        Merr_regress_ensemble_tr Mstderr_regress_ensemble_tr Merr_regress_ensemble_tst, ...
        Mstderr_regress_ensemble_tst Mclass_regress_ensemble_tr Mclass_regress_ensemble_tst; ...
        Merr_step_ensemble_tr Mstderr_step_ensemble_tr Merr_step_ensemble_tst, ...
        Mstderr_step_ensemble_tst Mclass_step_ensemble_tr Mclass_step_ensemble_tst; ...
        Merr_net_ensemble_tr Mstderr_net_ensemble_tr Merr_net_ensemble_tst, ...
        Mstderr_net_ensemble_tst Mclass_net_ensemble_tr Mclass_net_ensemble_tst; ...
        nan nan nan nan Mclass_vote_ensemble_tr Mclass_vote_ensemble_tst; ...
        Merr_rbf_ensemble_tr Mstderr_rbf_ensemble_tr Merr_rbf_ensemble_tst, ...
        Mstderr_rbf_ensemble_tst Mclass_rbf_ensemble_tr Mclass_rbf_ensemble_tst; ...
        nan nan nan nan Mclass_net2_ensemble_tr Mclass_net2_ensemble_tst;...
        nan nan nan nan Mclass_net1_ensemble_tr Mclass_net1_ensemble_tst; ...
        nan nan nan nan Mclass_major_ensemble_tr Mclass_major_ensemble_tst];
        
    xlswrite(outputfile,F,worksheet,'B2:G16');

    SF = {'Tr Mean RSE' 'Tr Std RSE' 'Tst Mean RSE' 'Tst Std RSE', ...
          'Tr Accuracy' 'Tst Accuracy' 'Tr MR' 'Tst MR', ...
          'Tr FHR' 'Tst FHR'};
      
    SF2 = {'Stepwise' ; ...
           'PLS' ; ...
           'GPLS' ; ...
           'GFA' ; ...
           'SOM'; 'kNN'; ...
           'Avg. Ensemble' ; ...
           'Regression Ensemble' ; ...
           'Stepwise Ensemble' ; ...
           'NNET Ensemble'; ...
           'Vote Ensemble'; ...
           'RBF Ensemble'; ...
           'ClassNet Ensemble'; ...
           'FFCNet Ensemble';...
           'MajorNet Ensemble'};
    
    if ~strcmp(worksheet,'topliss')
        SF3 = {'O1' 'O2' 'O3'}; 
        SF4 = {};
        
        xlswrite(outputfile,{'Step-Tr'; 'A1'; 'A2'; 'A3'; '';...
            'PLS-Tr'; 'A1'; 'A2'; 'A3'; ''; 'GPLS-Tr'; 'A1'; 'A2'; 'A3';...
            ''; 'GFA-Tr'; 'A1'; 'A2'; 'A3'; ''; ;'sSOM-Tr'; 'A1'; 'A2'; 'A3'; ''; ...
            'kNN-Tr'; 'A1'; 'A2'; 'A3'; ''; 'AvgEns-Tr';...
            'A1'; 'A2'; 'A3'; ''; 'RegEns-Tr'; 'A1'; 'A2'; 'A3'; '';'StepEns-Tr';...
            'A1'; 'A2'; 'A3'; '';'NnetEns-Tr'; 'A1'; 'A2'; 'A3'; '';'VoteEns-Tr';...
            'A1'; 'A2'; 'A3'; '';'RBFEns-Tr'; 'A1'; 'A2'; 'A3'; '';'ClassNetEns-Tr'; ...
            'A1'; 'A2'; 'A3'; ''; 'FFCNetEns-Tr';'A1'; 'A2'; 'A3'; ''; ...
            'MajorEns-Tr';'A1'; 'A2'; 'A3'},worksheet,'A23:A96');
        
        xlswrite(outputfile,SF3,worksheet,'B23:D23');
        xlswrite(outputfile,SF3,worksheet,'B28:D28');
        xlswrite(outputfile,SF3,worksheet,'B33:D33');
        xlswrite(outputfile,SF3,worksheet,'B38:D38');
        xlswrite(outputfile,SF3,worksheet,'B43:D43');
        xlswrite(outputfile,SF3,worksheet,'B48:D48');
        xlswrite(outputfile,SF3,worksheet,'B53:D53');
        xlswrite(outputfile,SF3,worksheet,'B58:D58');
        xlswrite(outputfile,SF3,worksheet,'B63:D63');
        xlswrite(outputfile,SF3,worksheet,'B68:D68');
        xlswrite(outputfile,SF3,worksheet,'B73:D73');
        xlswrite(outputfile,SF3,worksheet,'B78:D78');
        xlswrite(outputfile,SF3,worksheet,'B83:D83');
        xlswrite(outputfile,SF3,worksheet,'B88:D88');
        xlswrite(outputfile,SF3,worksheet,'B93:D93');
        
        xlswrite(outputfile,confusion_step_tr,worksheet,'B24:D26');
        xlswrite(outputfile,confusion_pls_tr,worksheet,'B29:D31');
        xlswrite(outputfile,confusion_gpls_tr,worksheet,'B34:D36');
        xlswrite(outputfile,confusion_gfa_tr,worksheet,'B39:D41');
        xlswrite(outputfile,confusion_som_tr,worksheet,'B44:D46');
        xlswrite(outputfile,confusion_knn_tr,worksheet,'B49:D51');
        xlswrite(outputfile,Mconfusion_avg_ensemble_tr,worksheet,'B54:D56');
        xlswrite(outputfile,Mconfusion_regress_ensemble_tr,worksheet,'B59:D61');
        xlswrite(outputfile,Mconfusion_step_ensemble_tr,worksheet,'B64:D66');
        xlswrite(outputfile,Mconfusion_net_ensemble_tr,worksheet,'B69:D71');
        xlswrite(outputfile,Mconfusion_vote_ensemble_tr,worksheet,'B74:D76');
        xlswrite(outputfile,Mconfusion_rbf_ensemble_tr,worksheet,'B79:D81');
        xlswrite(outputfile,Mconfusion_net2_ensemble_tr,worksheet,'B84:D86');
        xlswrite(outputfile,Mconfusion_net1_ensemble_tr,worksheet,'B89:D91');
        xlswrite(outputfile,Mconfusion_major_ensemble_tr,worksheet,'B94:D96');
        
        xlswrite(outputfile,{'Step-Tst'; 'A1'; 'A2'; 'A3'; '';...
            'PLS-Tst'; 'A1'; 'A2'; 'A3'; ''; 'GPLS-Tst'; 'A1'; 'A2'; 'A3';...
            ''; 'GFA-Tst'; 'A1'; 'A2'; 'A3'; '';'sSOM-Tst'; 'A1'; 'A2'; 'A3'; ''; ...
            'kNN-Tst'; 'A1'; 'A2'; 'A3'; ''; 'AvgEns-Tst';...
            'A1'; 'A2'; 'A3'; ''; 'RegEns-Tst'; 'A1'; 'A2'; 'A3'; '';'StepEns-Tst';...
            'A1'; 'A2'; 'A3'; '';'NnetEns-Tst'; 'A1'; 'A2'; 'A3'; '';'VoteEns-Tst';...
            'A1'; 'A2'; 'A3';'';'RBFEns-Tst'; 'A1'; 'A2'; 'A3';'';'ClassNetEns-Tst';...
            'A1'; 'A2'; 'A3';''; 'FFCNetEns-Tst';'A1'; 'A2'; 'A3';'';...
            'MajorEns-Tst';'A1'; 'A2'; 'A3'},worksheet,'F23:F96');
       
        xlswrite(outputfile,SF3,worksheet,'G23:I23');
        xlswrite(outputfile,SF3,worksheet,'G28:I28');
        xlswrite(outputfile,SF3,worksheet,'G33:I33');
        xlswrite(outputfile,SF3,worksheet,'G38:I38');
        xlswrite(outputfile,SF3,worksheet,'G43:I43');
        xlswrite(outputfile,SF3,worksheet,'G48:I48');
        xlswrite(outputfile,SF3,worksheet,'G53:I53');
        xlswrite(outputfile,SF3,worksheet,'G58:I58');
        xlswrite(outputfile,SF3,worksheet,'G63:I63');    
        xlswrite(outputfile,SF3,worksheet,'G68:I68');
        xlswrite(outputfile,SF3,worksheet,'G73:I73');
        xlswrite(outputfile,SF3,worksheet,'G78:I78');
        xlswrite(outputfile,SF3,worksheet,'G83:I83');
        xlswrite(outputfile,SF3,worksheet,'G88:I88');
        xlswrite(outputfile,SF3,worksheet,'G93:I93');
        
        
        xlswrite(outputfile,confusion_step_tst,worksheet,'G24:I26');
        xlswrite(outputfile,confusion_pls_tst,worksheet,'G29:I31');
        xlswrite(outputfile,confusion_gpls_tst,worksheet,'G34:I36');
        xlswrite(outputfile,confusion_gfa_tst,worksheet,'G39:I41');
        xlswrite(outputfile,confusion_som_tst,worksheet,'G44:I46');
        xlswrite(outputfile,confusion_knn_tst,worksheet,'G49:I51');
        xlswrite(outputfile,Mconfusion_avg_ensemble_tst,worksheet,'G54:I56');
        xlswrite(outputfile,Mconfusion_regress_ensemble_tst,worksheet,'G59:I61');
        xlswrite(outputfile,Mconfusion_step_ensemble_tst,worksheet,'G64:I66');
        xlswrite(outputfile,Mconfusion_net_ensemble_tst,worksheet,'G69:I71');
        xlswrite(outputfile,Mconfusion_vote_ensemble_tst,worksheet,'G74:I76');
        xlswrite(outputfile,Mconfusion_rbf_ensemble_tst,worksheet,'G79:I81');
        xlswrite(outputfile,Mconfusion_net2_ensemble_tst,worksheet,'G84:I86');
        xlswrite(outputfile,Mconfusion_net1_ensemble_tst,worksheet,'G89:I91');
        xlswrite(outputfile,Mconfusion_major_ensemble_tst,worksheet,'G94:I96');
    else
        SF3 = {'O1' 'O2' 'O3' 'O4'};
        SF4 = {'A1'; 'A2'; 'A3'; 'A4'};
        
        xlswrite(outputfile,{'Step-Tr'; 'A1'; 'A2'; 'A3'; 'A4'; '';...
            'PLS-Tr'; 'A1'; 'A2'; 'A3'; 'A4'; ''; 'GPLS-Tr'; 'A1'; 'A2'; 'A3';'A4';...
            ''; 'GFA-Tr'; 'A1'; 'A2'; 'A3';'A4'; ''; 'sSOM-Tr'; 'A1'; 'A2'; 'A3';'A4'; '';...
            'kNN-Tr'; 'A1'; 'A2'; 'A3';'A4'; '';'AvgEns-Tr';...
            'A1'; 'A2'; 'A3'; 'A4';''; 'RegEns-Tr'; 'A1'; 'A2'; 'A3'; 'A4';'';'StepEns-Tr';...
            'A1'; 'A2'; 'A3'; 'A4';'';'NnetEns-Tr'; 'A1'; 'A2'; 'A3';'A4'; '';'VoteEns-Tr';...
            'A1'; 'A2'; 'A3';'A4';'';'RBFEns-Tr'; 'A1'; 'A2'; 'A3';'A4';'';'ClassNetEns-Tr';...
            'A1'; 'A2'; 'A3';'A4';''; 'FFCNetEns-Tr';'A1'; 'A2'; 'A3';'A4';'';...
            'MajorEns-Tr';'A1'; 'A2'; 'A3';'A4'},worksheet,'A23:A111');
        
        xlswrite(outputfile,SF3,worksheet,'B23:E23');
        xlswrite(outputfile,SF3,worksheet,'B29:E29');
        xlswrite(outputfile,SF3,worksheet,'B35:E35');
        xlswrite(outputfile,SF3,worksheet,'B41:E41');
        xlswrite(outputfile,SF3,worksheet,'B47:E47');
        xlswrite(outputfile,SF3,worksheet,'B53:E53');
        xlswrite(outputfile,SF3,worksheet,'B59:E59');
        xlswrite(outputfile,SF3,worksheet,'B65:E65');
        xlswrite(outputfile,SF3,worksheet,'B71:E71');
        xlswrite(outputfile,SF3,worksheet,'B77:E77');
        xlswrite(outputfile,SF3,worksheet,'B83:E83');
        xlswrite(outputfile,SF3,worksheet,'B89:E89');
        xlswrite(outputfile,SF3,worksheet,'B95:E95');
        xlswrite(outputfile,SF3,worksheet,'B101:E101');
        xlswrite(outputfile,SF3,worksheet,'B107:E107');
        
        xlswrite(outputfile,confusion_step_tr,worksheet,'B24:E27');
        xlswrite(outputfile,confusion_pls_tr,worksheet,'B30:E33');
        xlswrite(outputfile,confusion_gpls_tr,worksheet,'B36:E39');
        xlswrite(outputfile,confusion_gfa_tr,worksheet,'B42:E45');
        xlswrite(outputfile,confusion_som_tr,worksheet,'B48:E51');
        xlswrite(outputfile,confusion_knn_tr,worksheet,'B54:E57');
        xlswrite(outputfile,Mconfusion_avg_ensemble_tr,worksheet,'B60:E63');
        xlswrite(outputfile,Mconfusion_regress_ensemble_tr,worksheet,'B66:E69');
        xlswrite(outputfile,Mconfusion_step_ensemble_tr,worksheet,'B72:E75');
        xlswrite(outputfile,Mconfusion_net_ensemble_tr,worksheet,'B78:E81');
        xlswrite(outputfile,Mconfusion_vote_ensemble_tr,worksheet,'B84:E87');
        xlswrite(outputfile,Mconfusion_rbf_ensemble_tr,worksheet,'B90:E93');
        xlswrite(outputfile,Mconfusion_net2_ensemble_tr,worksheet,'B96:E99');
        xlswrite(outputfile,Mconfusion_net1_ensemble_tr,worksheet,'B102:E105');
        xlswrite(outputfile,Mconfusion_major_ensemble_tr,worksheet,'B108:E111');
        
        xlswrite(outputfile,{'Step-Tst'; 'A1'; 'A2'; 'A3'; 'A4'; '';...
            'PLS-Tst'; 'A1'; 'A2'; 'A3'; 'A4'; ''; 'GPLS-Tst'; 'A1'; 'A2'; 'A3';'A4';...
            ''; 'GFA-Tst'; 'A1'; 'A2'; 'A3';'A4'; ''; ; 'sSOM-Tst'; 'A1'; 'A2'; 'A3';'A4'; '';...
            'kNN-Tst'; 'A1'; 'A2'; 'A3';'A4'; '';'AvgEns-Tst';...
            'A1'; 'A2'; 'A3'; 'A4';''; 'RegEns-Tst'; 'A1'; 'A2'; 'A3'; 'A4';'';'StepEns-Tst';...
            'A1'; 'A2'; 'A3'; 'A4';'';'NnetEns-Tst'; 'A1'; 'A2'; 'A3';'A4'; '';'VoteEns-Tst';...
            'A1'; 'A2'; 'A3';'A4';'';'RBFEns-Tst'; 'A1'; 'A2'; 'A3';'A4';'';'ClassNetEns-Tst';...
            'A1'; 'A2'; 'A3';'A4';''; 'FFCNetEns-Tst';'A1'; 'A2'; 'A3';'A4';'';...
            'MajorNetEns-Tst';'A1'; 'A2'; 'A3';'A4'},worksheet,'G23:G111');
       
        xlswrite(outputfile,SF3,worksheet,'H23:K23');
        xlswrite(outputfile,SF3,worksheet,'H29:K29');
        xlswrite(outputfile,SF3,worksheet,'H35:K35');
        xlswrite(outputfile,SF3,worksheet,'H41:K41');
        xlswrite(outputfile,SF3,worksheet,'H47:K47');
        xlswrite(outputfile,SF3,worksheet,'H53:K53');
        xlswrite(outputfile,SF3,worksheet,'H59:K59');
        xlswrite(outputfile,SF3,worksheet,'H65:K65');
        xlswrite(outputfile,SF3,worksheet,'H71:K71');    
        xlswrite(outputfile,SF3,worksheet,'H77:K77');
        xlswrite(outputfile,SF3,worksheet,'H83:K83');
        xlswrite(outputfile,SF3,worksheet,'H89:K89'); 
        xlswrite(outputfile,SF3,worksheet,'H95:K95');
        xlswrite(outputfile,SF3,worksheet,'H101:K101');
        xlswrite(outputfile,SF3,worksheet,'H107:K107');
         
        xlswrite(outputfile,confusion_step_tst,worksheet,'H24:K27');
        xlswrite(outputfile,confusion_pls_tst,worksheet,'H30:K33');
        xlswrite(outputfile,confusion_gpls_tst,worksheet,'H36:K39');
        xlswrite(outputfile,confusion_gfa_tst,worksheet,'H42:K45');
        xlswrite(outputfile,confusion_som_tst,worksheet,'H48:K51');
        xlswrite(outputfile,confusion_knn_tst,worksheet,'H54:K57');
        xlswrite(outputfile,Mconfusion_avg_ensemble_tst,worksheet,'H60:K63');
        xlswrite(outputfile,Mconfusion_regress_ensemble_tst,worksheet,'H66:K69');
        xlswrite(outputfile,Mconfusion_step_ensemble_tst,worksheet,'H72:K75');
        xlswrite(outputfile,Mconfusion_net_ensemble_tst,worksheet,'H78:K81');
        xlswrite(outputfile,Mconfusion_vote_ensemble_tst,worksheet,'H84:K87');
        xlswrite(outputfile,Mconfusion_rbf_ensemble_tst,worksheet,'H90:K93');
        xlswrite(outputfile,Mconfusion_net2_ensemble_tst,worksheet,'H96:K99');
        xlswrite(outputfile,Mconfusion_net1_ensemble_tst,worksheet,'H102:K105');
        xlswrite(outputfile,Mconfusion_major_ensemble_tst,worksheet,'H108:K111');
       
    end;
    
    xlswrite(outputfile,SF,worksheet,'B1:K1');

    xlswrite(outputfile,SF2,worksheet,'A2:A16');
    
    SF = {'Name' 'Actual' 'Stepwise' 'GPLS' 'GFA' 'PLS' 'SSOM' 'kNN' 'Avg. Ensemble' ...
           'Regression Ensemble' 'Stepwise Ensemble' 'NNET Ensemble' ...
           'Vote Ensemble' 'RBF Ensemble' 'ClassNet Ensemble' ...
           'FFCNet Ensemble' 'MajorNet Ensemble'};
    xlswrite(classfile,SF,worksheet,'A1:Q1');
    
    xlswrite(classfile,TrCmpNames,worksheet,'A2');
    xlswrite(classfile,TstCmpNames,worksheet,strcat('A',num2str(3+length(bin_tr))));
    
    results_class_tr = [bin_tr step_bin_tr gpls_bin_tr gfa_bin_tr pls_bin_tr som_bin_tr knn_bin_tr ...
                     Mavg_ensemble_bin_tr Mregress_ensemble_bin_tr  Mstep_ensemble_bin_tr ...
                     Mnet_ensemble_bin_tr Mvote_ensemble_bin_tr Mrbf_ensemble_bin_tr ...
                     Mnet2_ensemble_bin_tr Mnet1_ensemble_bin_tr Mmajor_ensemble_bin_tr];
   
    
    results_class_tst = [bin_tst step_bin_tst gpls_bin_tst gfa_bin_tst pls_bin_tst som_bin_tst knn_bin_tst ...
                     Mavg_ensemble_bin_tst Mregress_ensemble_bin_tst  Mstep_ensemble_bin_tst ...
                     Mnet_ensemble_bin_tst Mvote_ensemble_bin_tst Mrbf_ensemble_bin_tst ...
                     Mnet2_ensemble_bin_tst Mnet1_ensemble_bin_tst Mmajor_ensemble_bin_tst];
    
    xlswrite(classfile,results_class_tr,worksheet,'B2');
    xlswrite(classfile,results_class_tst,worksheet,strcat('B',num2str(3+length(bin_tr))));

end;

toc
