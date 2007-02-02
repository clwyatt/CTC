%function A = ensemble(filename,worksheet);
clear all;
close all;
warning off;
filename = 'ersin_results2.xls';
worklist = {'a4b2', 'd2', 'd3', 'dhfr', 'topliss'}; 

[filename, pathname, filterindex] = uigetfile('*.xls', 'Pick Excel Class File');

 if isequal(filename,0) | isequal(pathname,0)
       disp('User pressed cancel'); return; 
 end
 

% Inc =  shows which methods will be used in the prediction based ensemble decisions

% 1 = step-wise 2=GPLS 3=GFA 4=PLS 
Inc = [1,2,3];
% Inc2 =  shows which methods will be used in the classification based ensemble decision
% combination
% 1 = step-wise 2=GPLS 3=GFA 4=PLS 5=SSOM 6=kNN
Inc2 = [1,2,3,5];
% EnsInc =  shows which methods will be used in combining the ensembles
% 1 = avg 2=regression 3=step 4=NNET 5=Vote 6=RBF 7=ClassNet 8=FFCNET

EnsInc = [1,2,4,5,6,7,8,9];
Rep = 30;

for rr=1:length(worklist)
    worksheet = char(worklist(rr));
    [A,txt] = xlsread(filename,worksheet);
    if strcmp(worksheet,'dhfr')
        offset = 1;
        Trloc = find(isnan(A(:,1)));
        TrCmpNames = A(1:Trloc(1)-1,1);
        TstCmpNames = A(Trloc(1)+1:end,1);  
    else
        offset = 0; 
        Trloc = find(strcmp(txt(:,1),''));
        TrCmpNames = txt(2:Trloc(1)-1,1);
        TstCmpNames = txt(Trloc(1)+1:end,1);
    end;    
    [r,c ] = find(isnan(A));
    bin_tr  = A(1:r(1)-1,1+ offset);
    bin_tst = A(r(1)+1:end,1+ offset);
    step_tr = A(1:r(1)-1,2+ offset);
    step_tst = A(r(1)+1:end,2+ offset);
    gpls_tr = A(1:r(1)-1,3+ offset);
    gpls_tst = A(r(1)+1:end,3+ offset);
    gfa_tr = A(1:r(1)-1,4+ offset);
    gfa_tst = A(r(1)+1:end,4+ offset);
    pls_tr = A(1:r(1)-1,5+ offset);
    pls_tst = A(r(1)+1:end,5+ offset);
    som_tr = A(1:r(1)-1,6+ offset);
    som_tst = A(r(1)+1:end,6+ offset);
    knn_tr = A(1:r(1)-1,7+ offset);
    knn_tst = A(r(1)+1:end,7+ offset);
    avg_ens_tr = A(1:r(1)-1,8+ offset);
    avg_ens_tst = A(r(1)+1:end,8+ offset);
    regress_ens_tr = A(1:r(1)-1,9+ offset);
    regress_ens_tst = A(r(1)+1:end,9+ offset);
    step_ens_tr = A(1:r(1)-1,10+ offset);
    step_ens_tst = A(r(1)+1:end,10+ offset);
    nnet_ens_tr = A(1:r(1)-1,11+ offset);
    nnet_ens_tst = A(r(1)+1:end,11+ offset);
    vote_ens_tr = A(1:r(1)-1,12+ offset);
    vote_ens_tst = A(r(1)+1:end,12+ offset);
    rbf_ens_tr = A(1:r(1)-1,13+ offset);
    rbf_ens_tst = A(r(1)+1:end,13+ offset);
    classnet_ens_tr = A(1:r(1)-1,14+ offset);
    classnet_ens_tst = A(r(1)+1:end,14+ offset);
    ffcnet_ens_tr = A(1:r(1)-1,15+ offset);
    ffcnet_ens_tst = A(r(1)+1:end,15+ offset);
    majornet_ens_tr = A(1:r(1)-1,16+ offset);
    majornet_ens_tst = A(r(1)+1:end,16+ offset);
    
    Ind_Results_tr = [step_tr gpls_tr gfa_tr pls_tr som_tr knn_tr];
    Ind_Results_tr = Ind_Results_tr(:,Inc2);
    Ind_Results_tst = [step_tst gpls_tst gfa_tst pls_tst som_tst knn_tst];
    Ind_Results_tst = Ind_Results_tst(:,Inc2);
    
    Act_tr = repmat(bin_tr,1,length(Inc2));
    Cor_Mtx = sum(Act_tr == Ind_Results_tr,2);
    Opt_Acc_tr(rr) = sum(Cor_Mtx > 0)/length(bin_tr);
    
    Act_tst = repmat(bin_tst,1,length(Inc2));
    Cor_Mtx = sum(Act_tst == Ind_Results_tst,2);
    Opt_Acc_tst(rr) = sum(Cor_Mtx > 0)/length(bin_tst);
    
    Ens_Results_tr = [avg_ens_tr regress_ens_tr step_ens_tr nnet_ens_tr vote_ens_tr ...
                      rbf_ens_tr classnet_ens_tr ffcnet_ens_tr majornet_ens_tr];
    Ens_Results_tr = Ens_Results_tr(:,EnsInc);
    Ens_Results_tst = [avg_ens_tst regress_ens_tst step_ens_tst nnet_ens_tst vote_ens_tst ...
                      rbf_ens_tst classnet_ens_tst ffcnet_ens_tst majornet_ens_tst];
    Ens_Results_tst = Ens_Results_tst(:,EnsInc);
    
    Act_tr = repmat(bin_tr,1,length(EnsInc));
    Cor_Mtx = sum(Act_tr == Ens_Results_tr,2);
    Opt_Ens_Acc_tr(rr) = sum(Cor_Mtx > 0)/length(bin_tr);
    
    Act_tst = repmat(bin_tst,1,length(EnsInc));
    Cor_Mtx = sum(Act_tst == Ens_Results_tst,2);
    Opt_Ens_Acc_tst(rr) = sum(Cor_Mtx > 0)/length(bin_tst); 
    
    if strcmp(worksheet,'topliss')
        binnum = 4;
    else
        binnum = 3;
    end;    
    if ~strcmp(worksheet,'dhfr') & ~strcmp(worksheet,'topliss') 
        GB = 1; BB = binnum; 
    else 
        GB = binnum; BB = 1; 
    end;  
    
    MR_step_tr = (step_tr == BB).*(bin_tr == GB);
    MR_step_tst = (step_tst == BB).*(bin_tst == GB);
    MR_gpls_tr = (gpls_tr == BB).*(bin_tr == GB);
    MR_gpls_tst = (gpls_tst == BB).*(bin_tst == GB);
    MR_gfa_tr = (gfa_tr == BB).*(bin_tr == GB);
    MR_gfa_tst = (gfa_tst == BB).*(bin_tst == GB);
    MR_pls_tr = (pls_tr == BB).*(bin_tr == GB);
    MR_pls_tst = (pls_tst == BB).*(bin_tst == GB);
    MR_som_tr = (som_tr == BB).*(bin_tr == GB);
    MR_som_tst = (som_tst == BB).*(bin_tst == GB);
    MR_knn_tr = (knn_tr == BB).*(bin_tr == GB);
    MR_knn_tst = (knn_tst == BB).*(bin_tst == GB);
    
    Ind_MR_tr = [MR_step_tr MR_gpls_tr MR_gfa_tr MR_pls_tr MR_som_tr MR_knn_tr];
    Ind_MR_tr = sum(Ind_MR_tr(:,Inc2),2);
    Ind_MR_tst = [MR_step_tst MR_gpls_tst MR_gfa_tst MR_pls_tst MR_som_tst MR_knn_tst];
    Ind_MR_tst = sum(Ind_MR_tst(:,Inc2),2);
    
    % Individual confusion matrices
    for ii=1:binnum 
        for jj=1:binnum
            confusion_step_tr(ii,jj) = sum((step_tr == jj).*(bin_tr == ii));
            confusion_step_tst(ii,jj) = sum((step_tst == jj).*(bin_tst == ii));
            confusion_pls_tr(ii,jj) = sum((pls_tr == jj).*(bin_tr == ii));
            confusion_pls_tst(ii,jj) = sum((pls_tst == jj).*(bin_tst == ii));
            confusion_gpls_tr(ii,jj) = sum((gpls_tr == jj).*(bin_tr == ii));
            confusion_gpls_tst(ii,jj) = sum((gpls_tst == jj).*(bin_tst == ii));
            confusion_gfa_tr(ii,jj) = sum((gfa_tr == jj).*(bin_tr == ii));
            confusion_gfa_tst(ii,jj) = sum((gfa_tst == jj).*(bin_tst == ii));
            confusion_som_tr(ii,jj) = sum((som_tr == jj).*(bin_tr == ii));
            confusion_som_tst(ii,jj) = sum((som_tst == jj).*(bin_tst == ii));
            confusion_knn_tr(ii,jj) = sum((knn_tr == jj).*(bin_tr == ii));
            confusion_knn_tst(ii,jj) = sum((knn_tst == jj).*(bin_tst == ii));
        end;
    end;    
    
    
    MajLimit = ceil(length(Inc2)/2);
    
    St_MR_tr(rr) = sum(Ind_MR_tr > MajLimit);
    St_MR_tst(rr) = sum(Ind_MR_tst > MajLimit);
    
    
end;    
   
Opt_Acc_tr = Opt_Acc_tr
Opt_Acc_tst = Opt_Acc_tst

St_MR_tr  =St_MR_tr
St_MR_tst  =St_MR_tst

Opt_Ens_Acc_tr = Opt_Ens_Acc_tr
Opt_Ens_Acc_tst = Opt_Ens_Acc_tst
    
    
    
