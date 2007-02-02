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
xmean = mean(Xmat);
xstd = std(Xmat);
Xmat = (Xmat - ones(size(Xmat,1),1)*xmean)./(ones(size(Xmat,1),1)*xstd);

DistMat  = distfun2(Xmat,Xmat,'cosine');
[FF,GG] = meshgrid(bin_tr);
ClassMat = abs(FF-GG);

[SR,SC] = find(ClassMat==0);
SRn = SR(SR>SC);
SCn = SC(SR>SC);

for i=1:length(SRn)
    SameDist(i) = DistMat(SRn(i),SCn(i));
end;    

[DR,DC] = find(ClassMat~=0);
DRn = DR(DR>DC);
DCn = DC(DR>DC);
for i=1:length(DRn)
    DiffDist(i) = DistMat(DRn(i),DCn(i));
end;  

figure(1)
subplot(211)
hist(SameDist,50)
title('Same Class Histogram');
subplot(212)
hist(DiffDist,50)
title('Different Class Histogram');

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



