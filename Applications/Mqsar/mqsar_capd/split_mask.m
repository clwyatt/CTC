function split_mask(Splits)
% 
% Synopsis:
%         split_mask(Splits)
%
%
% 
% Inputs:
% -------
% Splits: Number of partitions to be included in each sub partition file
%
%
% Ersin Bayram, September 2003
% Targacept, Inc. 
% http://www.targacept.com
%---------------------------------------------------------------
 


% Now read the mask file. 

Defaultfile = '/home/matlab/SBIR/Data/*.*';
[FileName, FilePath] = uigetfile({Defaultfile, 'Mask Files {*.dat, *.txt}'; ...
    '*.*', 'All Files (*.*)';}, 'Mask File to be Partitioned');
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

%if mod(N,Splits)~=0
%   display(' Number of Partitions in mask file is not divisible by Splits!');
%   return;
%end;   

K = round(N/Splits);
Init = 0;

for i=1:Splits
    % Generate theMask files for the splitted partitions
    Init = K*(i-1)+1;
    if i~=Splits | (mod(N,Splits) ==0)
		Defaultfile = strcat(maskfile(1:end-4), '_', num2str(Init), '_', num2str(Init+K-1), maskfile(end-3:end));
    else
        Defaultfile = strcat(maskfile(1:end-4), '_', num2str(Init), '_', num2str(Init+rem(N,K)-1), maskfile(end-3:end));
    end;
    
	[FName, FPath] = uiputfile(Defaultfile,'Mask Files for the Split Partitions');
	newmaskfile = strcat(FPath,FName);
	
	
	if (FName ~= 0)
      fMask = fopen(newmaskfile, 'w');
      fprintf(fMask, '%s', 'Name');
	else
       display('Invalid File Name! Try again ');
       return;
	end;
    if (i~=Splits) | (mod(N,Splits) ==0)
		for nSet=1:K
            fprintf(fMask, '\t%6s', char(['P', num2str((i-1)*K + nSet)]));
		end
		fprintf(fMask, '\n');
	
		for nCmp=1:length(rownames)
            fprintf(fMask, '%s', char(rownames(nCmp)));
            for nSet=1:K
                fprintf(fMask, '\t%6d',tData(nCmp,(i-1)*K + nSet));
            end
            fprintf(fMask, '\n');
		end
    else
        for nSet=1:rem(N,K)
            fprintf(fMask, '\t%6s', char(['P', num2str((i-1)*K + nSet)]));
		end
		fprintf(fMask, '\n');
	
		for nCmp=1:length(rownames)
            fprintf(fMask, '%s', char(rownames(nCmp)));
            for nSet=1:rem(N,K)
                fprintf(fMask, '\t%6d',tData(nCmp,(i-1)*K + nSet));
            end
            fprintf(fMask, '\n');
		end
    end;  
	fclose(fMask);

end;    


