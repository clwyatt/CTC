
Defaultfile = '/home/matlab/SBIR/Data/*.*';
[FileName, FilePath] = uigetfile({'*.dat','Text Files (*.dat)'; ...
    '*.*', 'All Files (*.*)';}, ['Pick the Data File']);
maskfile = strcat(FilePath,FileName);  
[rownames columnames Maskmat] = qsarimport(maskfile);

Temp =  model_stats('ga',columnames);
[Tops_GAStd, Ind_GAStd] =  sort(Temp);
Temp = model_stats('ga',columnames);
[Tops_GASub, Ind_GASub] = sort(Temp);
%Temp = model_stats('sa',columnames);
%[Tops_SA, Ind_SA] = sort(Temp);

Limit = 30;
%Limit = length(Tops_GAStd);

plot(Ind_GAStd(end-Limit+1:end),Tops_GAStd(end-Limit+1:end),'*')
hold on;
plot(Ind_GASub(end-Limit+1:end),Tops_GASub(end-Limit+1:end),'o')
%plot(Ind_SA(end-Limit+1:end),Tops_SA(end-Limit+1:end),'+')
axis([1 length(columnames) 0 100])
legend('SGA', 'EGA')
ylabel('% Used in Models')
hold off;