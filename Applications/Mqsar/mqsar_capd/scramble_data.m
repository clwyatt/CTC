[FileName, FilePath] = uigetfile({'*.txt','Text Files (*.txt)'; ...
    '*.*', 'All Files (*.*)';}, ['Pick the Data File']);
datafile = strcat(FilePath,FileName);  

[CmpNames, VarNames, Xmat, Ymat, Ynames] = qsarimport(datafile, 'bioact',1);

Xmatrand = zeros(size(Xmat));

for i=1:length(VarNames)
    Xmatrand(:,i) = Xmat(randperm(length(Ymat)),i);
end;    

TM = [Ymat Xmatrand];
rfile = strcat(FilePath,FileName(1:end-4),'_rand.txt')
save(rfile,'TM', '-ascii') 