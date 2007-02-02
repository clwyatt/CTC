clear all;
close all;
warning off;
filename = 'ersin_results2.xls';
worklist = {'a4b2', 'd2', 'd3', 'dhfr', 'topliss'}; 
outputfile = 'classes.xls'
% 1 = step-wise 2=GPLS 3=GFA 4=PLS
% Inc =  shows which methods will be used in the ensemble decisions
Inc = [1,2,3,4];

for rr=1:length(worklist)
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
    
    [A,txt] = xlsread(filename,worksheet);

    [r,c ] = find(isnan(A));
    if strcmp(worksheet,'dhfr')
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
    
    step_bin_tr = ones(length(bioact_tr),1);
    step_bin_tst = ones(length(bioact_tst),1);
    pls_bin_tr = ones(length(bioact_tr),1);
    pls_bin_tst = ones(length(bioact_tst),1);
    gpls_bin_tr = ones(length(bioact_tr),1);
    gpls_bin_tst = ones(length(bioact_tst),1);
    gfa_bin_tr = ones(length(bioact_tr),1);
    gfa_bin_tst = ones(length(bioact_tst),1);
    
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

    results_bin_tr = [bin_tr step_bin_tr gpls_bin_tr gfa_bin_tr pls_bin_tr];
    results_bin_tst = [bin_tst step_bin_tst gpls_bin_tst gfa_bin_tst pls_bin_tst];
    
    SF = {'Actual' 'Stepwise' 'GPLS' 'GFA' 'PLS'}; 
    xlswrite(outputfile,SF,worksheet,'A1:E1');
    xlswrite(outputfile,results_bin_tr,worksheet,'A2');
    xlswrite(outputfile,results_bin_tst,worksheet,strcat('A',num2str(3+length(bin_tr))));
    
end;
    
    
    