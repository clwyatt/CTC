function crop_mask(Initial, Last)
% 
% Synopsis:
%    crop_mask(Initial, Last)
%
%
% 
% Inputs:
% -------
% Initial: First column to be included in each sub partition file
% Last   : Last column to be included in each sub partition file
%
% Ersin Bayram, September 2003
% Targacept, Inc. 
% http://www.targacept.com
%---------------------------------------------------------------
 


% Now read the mask file. 

Defaultfile = '/home/matlab/SBIR/Data/*.*';
[FileName, FilePath] = uigetfile(Defaultfile,'Mask File to be Partitioned');
maskfile = strcat(FilePath,FileName);

if (FileName ~= 0)
  [rownames, colnames, tData] = dataimport(maskfile);
  rownames(1) =  [];
  colnames(1) =  [];
else
   display('Invalid File Name! Try again ');
   return;
end;

[M,N] = size(tData);

Defaultfile = strcat(maskfile(1:end-4), '_', num2str(Initial), '_', num2str(Last), maskfile(end-3:end));
[FName, FPath] = uiputfile(Defaultfile,'Mask Files for the Split Partitions');
newmaskfile = strcat(FPath,FName);
	
	
if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask, '%s', 'Name');
else
   display('Invalid File Name! Try again ');
   return;
end;

for nSet=Initial:Last
    fprintf(fMask, '\t%6s', char(['P', num2str(nSet)]));
end
fprintf(fMask, '\n');

for nCmp=1:length(rownames)
    fprintf(fMask, '%s', char(rownames(nCmp)));
    for nSet=Initial:Last
        fprintf(fMask, '\t%6d',tData(nCmp,nSet));
    end
    fprintf(fMask, '\n');
end
fclose(fMask);
   


