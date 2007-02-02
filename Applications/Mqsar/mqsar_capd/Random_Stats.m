function Random_Stats(alpha)
% This m-file reads the model file for a specific run i.e 3 bin SASOM, and 
% asks user to load the randomized runs for the best model and the median model. 
% Once the randomized run model files are loaded the script performs
% z-score test and the Wilcox RankSum test on the "TEST PERCENT CORRECT
% RATES" to see whether there is any statistically significant difference
% between the actual and randomized runs. File dumps the results of the
% analysis to a file whose name and location is selected by the user.  
% 
% Inputs:
% -------
% alpha   : Significance level for the Wilcox rankSum test
%           Default value is 1 - RandomRuns/(Random Runs + 1)
%           i.e for 0.05 confidence we should have 19 Random Runs.    
%
%
% Ersin Bayram, September 2003
% Targacept, Inc. 
% http://www.targacept.com
%---------------------------------------------------------------

 
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

Matchstr = 'Test Correct rate: ';
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

% Pick the first ones if we have more than one
Mean_Ind = Ind(1);
Best_Ind = Best_Ind(1);

% Now read the avg. model's Randomization runs' model file. 

Rfile_Ind = findstr(FilePath,'/Actual');
if ~isempty(Rfile_Ind)
   Defaultfile = strcat(FilePath(1:Rfile_Ind),'Random/Model/*.mod');    
else
   Defaultfile = '/home/matlab/SBIR/Data/*.*';
end;
   
[FileName, FilePath] = uigetfile({Defaultfile, 'Model Files (*.mod)'; ...
    '*.*', 'All Files (*.*)';}, 'Pick the Model File for the Avg. Model''s Randomized Run');
AvgRmodelfile = strcat(FilePath,FileName);

if (FileName ~= 0)
	fid   = fopen(AvgRmodelfile,'r');
	AvgRMat = fread(fid);
    AvgRMat = char(AvgRMat');
else
   display('Invalid File Name! Try again ');
   return;
end;

%Matchstr = 'Test Correct rate: ';

% Extract the Test Correct Rates from the the model file

Match_Ind = findstr(AvgRMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Now we are right at the starting point of each Numeric Test Correct Value
% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TP_Str = '';
   while not(strcmp(AvgRMat(Start_Ind(i)+j),' '))
        TP_Str = strcat(TP_Str,AvgRMat(Start_Ind(i)+j));
        j = j+1;
   end;
   AvgR_TruePos(i) = str2num(TP_Str);
end;   

% Find the models with the mean and best test correct rates
AvgR_MeanTP = mean(AvgR_TruePos);
AvgR_StdTP = std(AvgR_TruePos);

AvgR_ZS = (Mean_TP - AvgR_MeanTP)/AvgR_StdTP;
AvgR_Conf = sum((Mean_TP - AvgR_TruePos) >0)/(length(AvgR_TruePos)+1)*100;



if nargin < 1
   alpha = 1- length(AvgR_TruePos)/(length(AvgR_TruePos)+1);    
end;  

[AvgR_p,AvgR_h] = ranksum(True_Pos(Mean_Ind),AvgR_TruePos, alpha);


% Now read the best model's Randomization runs' model file.

[FileName, FilePath] = uigetfile({Defaultfile, 'Model Files (*.mod)'; ...
    '*.*', 'All Files (*.*)';}, 'Pick the Model File for the Best Model''s Randomized Run ');
BestRmodelfile = strcat(FilePath,FileName);


if (FileName ~= 0)
	fid   = fopen(BestRmodelfile,'r');
	BestRMat = fread(fid);
    BestRMat = char(BestRMat');
else
   display('Invalid File Name! Try again ');
   return;
end;

% Extract the Test Correct Rates from the the model file

Match_Ind = findstr(BestRMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Now we are right at the starting point of each Numeric Test Correct Value
% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TP_Str = '';
   while not(strcmp(BestRMat(Start_Ind(i)+j),' '))
        TP_Str = strcat(TP_Str,BestRMat(Start_Ind(i)+j));
        j = j+1;
   end;
   BestR_TruePos(i) = str2num(TP_Str);
end;   

% Find the models with the mean and best test correct rates
BestR_MeanTP = mean(BestR_TruePos);
BestR_StdTP = std(BestR_TruePos);

BestR_ZS = (Mean_TP - BestR_MeanTP)/BestR_StdTP;
BestR_Conf = sum((Mean_TP - BestR_TruePos) >0)/(length(BestR_TruePos)+1)*100;


if nargin < 1
   alpha = 1- length(BestR_TruePos)/(length(BestR_TruePos)+1);    
end;  


[BestR_p,BestR_h,BestR_stats] = ranksum(True_Pos(Best_Ind),BestR_TruePos, alpha);


% Generate theMask file for the average Model found for randomization

Defaultfile = strcat(FilePath,ModelName(1:end-4),'_RStats.txt');

[FName, FPath] = uiputfile(Defaultfile,'Pick the Txt File to Write Stat Results');
newmaskfile = strcat(FPath,FName);


if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', 'Average Model Randomized Runs');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Number of Randomized Runs: ' num2str(length(AvgR_TruePos))]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Average Model Index: ' num2str(Mean_Ind)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Actual Test Correct: ' num2str(True_Pos(Mean_Ind))]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Random Test Corrects: ' num2str(AvgR_TruePos)]);
  fprintf(fMask,'\n');fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Z-Score: ' num2str(AvgR_ZS)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Z-Score Confidence Level: ' num2str(AvgR_Conf)]);
  fprintf(fMask,'\n'); fprintf(fMask,'\n'); 
  fprintf(fMask, '%s', ['RankSum_H ' num2str(AvgR_h)]);
  fprintf(fMask,'\n'); 
  fprintf(fMask, '%s', ['RankSum_p: ' num2str(AvgR_p)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['RankSum Significance: ' num2str(alpha)]);
  fprintf(fMask,'\n');fprintf(fMask,'\n');
  fprintf(fMask,'-------------------------------------------------\n');
  fprintf(fMask,'\n');fprintf(fMask,'\n');
  
  fprintf(fMask, '%s', 'Best Model Randomized Runs');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Number of Randomized Runs: ' num2str(length(BestR_TruePos))]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Best Model Index: ' num2str(Best_Ind)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Actual Test Correct: ' num2str(True_Pos(Best_Ind))]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Random Test Corrects: ' num2str(BestR_TruePos)]);
  fprintf(fMask,'\n');fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Z-Score: ' num2str(BestR_ZS)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Z-Score Confidence Level: ' num2str(BestR_Conf)]);
  fprintf(fMask,'\n'); fprintf(fMask,'\n'); 
  fprintf(fMask, '%s', ['RankSum_H ' num2str(BestR_h)]);
  fprintf(fMask,'\n'); 
  fprintf(fMask, '%s', ['RankSum_p: ' num2str(BestR_p)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['RankSum Significance: ' num2str(alpha)]);
  fprintf(fMask,'\n');fprintf(fMask,'\n');
  
else
   display('Invalid File Name! Try again ');
   return;
end;

fclose(fMask);
