function repeat_mask(Repeats,varargin)
% 
% Synopsis:
%         random_mask(Repeats)
%
% This m-file reads the model and mask files for a specific run, and 
% finds the best and the average models using the Test correct Rates of 
% the models, and then generates the mask files for these two cases by 
% simply repeating the corresponding partiton column from the original 
% mask file. 
% 
% Inputs:
% -------
% Repeats   : Number of Repeats needed to extract statistical information
%
% Optional Inputs
% 'GA', 'ga', 'Ga' : indicates that the method is Genetic algorithm which
% is different than others. 
%
% Ersin Bayram, September 2003
% Targacept, Inc. 
% http://www.targacept.com
%---------------------------------------------------------------
 
%% Check arguments
i=1;
Method = 0;
warning off;
if nargin ==2 & ischar(varargin{1}), 
	switch varargin{1}, 
	% unambiguous values
	case {'GA', 'ga', 'Ga'},
        % GA is the feature selection method
		Method = 1;
    otherwise
        % The rest
        Method = 0;
    end
end

Defaultfile = '/home/matlab/SBIR/*.*';
[ModelName, FilePath] = uigetfile({Defaultfile, 'Model Files {*.mod}'; ...
    '*.*', 'All Files (*.*)';}, 'Pick the Model File to Load');
modelfile = strcat(FilePath,ModelName);


if (ModelName ~= 0)
	fid   = fopen(modelfile,'r');
	ModelMat = fread(fid);
    ModelMat = char(ModelMat');
else
   display('Invalid File Name! Try again ');
   return;
end;

if Method ==0
	Matchstr = 'Test Correct rate: ';    % All methods except GA
else 
   Matchstr = ' Model_Test_TruePos: ';   % GA
end;   

Match_Ind = findstr(ModelMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Now we are right at the starting point of each Numeric Test Correct Value
% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TP_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        TP_Str = strcat(TP_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   True_Pos(i) = str2num(TP_Str);
end;   

% Find the models with the mean and best test correct rates
Mean_TP = mean(True_Pos);
%Difference = Mean_TP - True_Pos
[Sorted,Ind] = sort(abs(Mean_TP - True_Pos));
[Best_TP,Best_Ind] = max(True_Pos);

% Pick the first ones if we have moreo than one
Mean_Ind = Ind(1)
Best_Ind = Best_Ind(1)

% Now read the mask file. 

Defaultfile = '/home/matlab/SBIR/Data/*.*';
[FileName, FilePath] = uigetfile({Defaultfile, 'Mask Files {*.dat, *.txt}'; ...
    '*.*', 'All Files (*.*)';}, 'Pick the Model''s Mask File');
maskfile = strcat(FilePath,FileName);

if (FileName ~= 0)
  [rownames, colnames, tData] = dataimport(maskfile);
  rownames(1) =  [];
  colnames(1) =  [];
else
   display('Invalid File Name! Try again ');
   return;
end;


% Generate theMask file for the average Model found for repetition

Defaultfile = strcat(FilePath,ModelName(1:end-4),'_AvgRep_',FileName);

[FName, FPath] = uiputfile(Defaultfile,'Mask File for the Average Rep.');
newmaskfile = strcat(FPath,FName);


if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask, '%s', 'Name');
else
   display('Invalid File Name! Try again ');
   return;
end;

for nSet=1:Repeats
    fprintf(fMask, '\t%6s', char(['R', num2str(nSet)]));
end
fprintf(fMask, '\n');

for nCmp=1:length(rownames)
    fprintf(fMask, '%s', char(rownames(nCmp)));
    for nSet=1:Repeats
        fprintf(fMask, '\t%6d',tData(nCmp,Mean_Ind));
    end
    fprintf(fMask, '\n');
end
fclose(fMask);

% Generate theMask file for the best Model found for Repetition
Defaultfile = strcat(FilePath,ModelName(1:end-4),'_BestRep_',FileName);

[FName, FPath] = uiputfile(Defaultfile, 'Mask File for the Best Rep.');
newmaskfile = strcat(FPath,FName);


if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask, '%s', 'Name');
else
   display('Invalid File Name! Try again ');
   return;
end;

for nSet=1:Repeats
    fprintf(fMask, '\t%6s', char(['R', num2str(nSet)]));
end
fprintf(fMask, '\n');

for nCmp=1:length(rownames)
    fprintf(fMask, '%s', char(rownames(nCmp)));
    for nSet=1:Repeats
        fprintf(fMask, '\t%6d',tData(nCmp,Best_Ind));
    end
    fprintf(fMask, '\n');
end
fclose(fMask);

