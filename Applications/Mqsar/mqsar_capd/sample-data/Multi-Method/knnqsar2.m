% kNN based classifier for qsar modeling
close all;
clear all;
warning off;

classfile = 'knn.xls'
worklist = {'a4b2', 'd2', 'd3', 'dhfr', 'topliss'};
for zz = 1:5
clear X*
clear b*
clear T*
clear K*
clear d*
clear P*
K=6; % K of the K-NN classifier    
worksheet = char(worklist(zz));
    zz = zz
    [fname,dirpath]  = uigetfile('C:\QSAR\Data\TrTeValid\*.dat','Load training file');

    HypLoc = strfind(fname,'_');
    Trdata = fname(1:HypLoc(1)-1);
    switch Trdata
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

    szDataName = strcat(dirpath,fname);
    [CmpNames, VarNames, Xmat, bioact_tr, Ynames, bin_tr] = qsarimport(szDataName, 'bioact', ...
                                                         1, 'binning', 'Non', ...
                                                         'cutoffs', Cutoff, 'ex', 'Non');                                               
    % Remove the identical columns                                                 
    [VarNames, Xmat] = qsarfilter(VarNames, Xmat,1.0, 0.0);

    % Normalize to Mahalanobis distance
    %xmean = mean(Xmat);
    %xstd = std(Xmat);
    %Xmat = (Xmat - ones(size(Xmat,1),1)*xmean)./(ones(size(Xmat,1),1)*xstd);


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

    % Normalize to Mahalanobis distance with respect to training data
    %dataTe = (dataTe - ones(size(dataTe,1),1)*xmean)./(ones(size(dataTe,1),1)*xstd);

    TrClass = knnclassify(Xmat,Xmat,bin_tr,K,'cosine','nearest');
    TrAccuracy = sum(TrClass==bin_tr)/length(bin_tr); 
    PrdClass = knnclassify(dataTe,Xmat,bin_tr,K,'cosine','nearest');
    PrdAccuracy = sum(PrdClass==bin_tst)/length(bin_tst); 
    
  
    xlswrite(classfile,{'Name' 'knn' 'Accuracy'},worksheet,'A1:C1');
    xlswrite(classfile,CmpNames,worksheet,'A2');
    xlswrite(classfile,TrClass,worksheet,'B2');
    xlswrite(classfile,TrAccuracy,worksheet,'C2');
    xlswrite(classfile,PrdAccuracy,worksheet,'C3');
    xlswrite(classfile,PrdCmpNames,worksheet,strcat('A',num2str(3+length(CmpNames))));
    xlswrite(classfile,PrdClass,worksheet,strcat('B',num2str(3+length(CmpNames))));
end;   



