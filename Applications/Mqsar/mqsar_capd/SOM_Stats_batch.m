function SOM_Stats_batch(alpha, data, varargin)
% This m-file reads the model file for a specific run i.e 3 bin SASOM, and 
% asks user to load the repeat runs for the best model and the median model. 
% Once the repeat run model files are loaded the script performs
% z-score test and the Wilcox RankSum test on the "TEST PERCENT CORRECT
% RATES" to see whether there is any statistically significant difference
% between the actual and repeated runs. File dumps the results of the
% analysis to a file whose name and location is selected by the user. 
% 
% First models reported in best model and avg. model runs are the actual 
% ones. For example repeat runs build 19 models and the first reported ones
% are from the actual runs, so we will have 20 best model or avg. model
% runs in total 
%
% alpha: Statistical significance level for Wilcox Ranksum test 
%
% Optional Inputs
% 'GA', 'ga', 'Ga' : indicates that the method is Genetic algorithm which
% is different than others. 
%
% Ersin Bayram, October 2003
% Targacept, Inc. 
% http://www.targacept.com
%---------------------------------------------------------------

%% Check arguments
i=1;
Method = 0;
warning off;
if nargin ==3 & ischar(varargin{1}), 
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

Defaultfile = '/home/matlab/SBIR/*.mod';
[ModelName, FilePath] = uigetfile(Defaultfile, 'Load the Actual Model File');
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
Std_TP = std(True_Pos);
%Difference = Mean_TP - True_Pos
[Sorted,Ind] = sort(abs(Mean_TP - True_Pos));
[Best_TP,Best_Ind] = max(True_Pos);
[worst_TP,Worst_Ind] = min(True_Pos);

% Pick the first ones if we have more than one
Mean_Ind = Ind(1);
Best_Ind = Best_Ind(1);

Defaultfile = [FilePath '*.mod'];
[BestName, FPath] = uigetfile(Defaultfile, 'Load the Best Repeat Model');
bestfile = strcat(FPath,BestName);
	
if (BestName ~= 0)
	fid   = fopen(bestfile,'r');
	BestMat = fread(fid);
    BestMat = char(BestMat');
else
   display('Invalid File Name! Try again ');
   return;
end;	

%Matchstr = 'Test Correct rate: ';
Match_Ind = findstr(BestMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Now we are right at the starting point of each Numeric Test Correct Value
% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TP_Str = '';
   while not(strcmp(BestMat(Start_Ind(i)+j),' '))
        TP_Str = strcat(TP_Str,BestMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Best_True_Pos(i) = str2num(TP_Str);
end;   

Best_True_Pos = [Best_TP Best_True_Pos];


Best_zscore = zscore(Best_True_Pos);
Best_zscore = Best_zscore(1);

[Best_Ranksum_P, Best_Ranksum_H] = ranksum(Best_True_Pos(1), Best_True_Pos(2:end));

if Best_Ranksum_H == 1
   RS_Best = ['"Medians are equal" can be rejected at ' num2str(100*alpha) '% significance level'];
else
   RS_Best = ['"Medians are equal" can NOT be rejected at ' num2str(100*alpha) '% significance level'];
end;

% Find the models with the mean and best test correct rates
Mean_Best_TP = mean(Best_True_Pos);
Std_Best_TP = std(Best_True_Pos);

[Best_BestTP,Best_BestInd] = max(Best_True_Pos);
[worst_BestTP,Worst_BestInd] = min(Best_True_Pos);
%------ Average Model --------------

Defaultfile = [FilePath '*.mod'];
[AvgName, FPath] = uigetfile(Defaultfile,'Load the Average Repeat Model');
Avgfile = strcat(FPath,AvgName);

if (AvgName ~= 0)
	fid   = fopen(Avgfile,'r');
	AvgMat = fread(fid);
    AvgMat = char(AvgMat');
else
   display('Invalid File Name! Try again ');
   return;
end;

%Matchstr = 'Test Correct rate: ';
Match_Ind = findstr(AvgMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Now we are right at the starting point of each Numeric Test Correct Value
% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TP_Str = '';
   while not(strcmp(AvgMat(Start_Ind(i)+j),' '))
        TP_Str = strcat(TP_Str,AvgMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Avg_True_Pos(i) = str2num(TP_Str);
end;   

Avg_True_Pos = [Mean_TP Avg_True_Pos]
Avg_zscore = zscore(Avg_True_Pos);
Avg_zscore = Avg_zscore(1);

[Avg_Ranksum_P, Avg_Ranksum_H] = ranksum(Avg_True_Pos(1), Avg_True_Pos(2:end));
if Avg_Ranksum_H == 1
   RS_Avg = ['"Medians are equal" can be rejected at ' num2str(100*alpha) '% significance level'];
else
   RS_Avg = ['"Medians are equal" can NOT be rejected at ' num2str(100*alpha) '% significance level'];
end;

% Find the models with the mean and best test correct rates
Mean_Avg_TP = mean(Avg_True_Pos);
Std_Avg_TP = std(Avg_True_Pos);

[Best_AvgTP,Best_AvgInd] = max(Avg_True_Pos);
[worst_AvgTP,Worst_AvgInd] = min(Avg_True_Pos);

% Perform[Avg_Ranksum_P, Avg_Ranksum_H] = ranksum(Avg_True_Pos(1), Avg_True_Pos(2:end));

[Tot_Ranksum_P, Tot_Ranksum_H] = ranksum(Avg_True_Pos, Best_True_Pos);
if Tot_Ranksum_H == 1
   RS_Tot = ['"Medians are equal" can be rejected at ' num2str(100*alpha) '% significance level'];
else
   RS_Tot = ['"Medians are equal" can NOT be rejected at ' num2str(100*alpha) '% significance level'];
end; 

%---------------------------------------------
% Generate the Mask file for the average Model found for randomization	
Defaultfile = strcat(FilePath,ModelName(1:end-4),'_Stats.txt');	
[FName, FPath] = uiputfile(Defaultfile,'Pick the Txt File to Write Stat Results');
newmaskfile = strcat(FPath,FName);
	
if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask,['Data Name: ' data]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Model Name: ' ModelName]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Average Model Index: ' num2str(Mean_Ind)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Best Model Index: ' num2str(Best_Ind)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Worst Model Index: ' num2str(Worst_Ind)]);
  fprintf(fMask,'\n');fprintf(fMask,'\n');
  fprintf(fMask, '%s', 'Model Run Test Corrects:');
  fprintf(fMask,'\n');
   
  S = rem(length(True_Pos),10);
  R = floor(length(True_Pos)/10);
  
  for i=1:R
    for j=1:10
        if j==1
            fprintf(fMask, '%s',num2str(True_Pos(10*(i-1)+j)));
        else
            fprintf(fMask, '\t%s',num2str(True_Pos(10*(i-1)+j)));
        end;
    end
    fprintf(fMask, '\n');
  end
  
  for j=1:S
        if j==1
            fprintf(fMask, '%s',num2str(True_Pos(10*R+j)));
        else
            fprintf(fMask, '\t%s',num2str(True_Pos(10*R+j)));
        end;
  end
  fprintf(fMask,'\n');
  fprintf(fMask,'-------------------------------------------------\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', 'Best Model Run Test Corrects:');
  fprintf(fMask,'\n');
   
  S = rem(length(Best_True_Pos),10);
  R = floor(length(Best_True_Pos)/10);
  
  for i=1:R
    for j=1:10
        if j==1
            fprintf(fMask, '%s',num2str(Best_True_Pos(10*(i-1)+j)));
        else
            fprintf(fMask, '\t%s',num2str(Best_True_Pos(10*(i-1)+j)));
        end;
    end
    fprintf(fMask, '\n');
  end
  
  for j=1:S
        if j==1
            fprintf(fMask, '%s',num2str(Best_True_Pos(10*R+j)));
        else
            fprintf(fMask, '\t%s',num2str(Best_True_Pos(10*R+j)));
        end;
  end
  fprintf(fMask, '\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Best Model Repeats'' Mean: ' num2str(Mean_Best_TP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Best Model Repeats'' Std: ' num2str(Std_Best_TP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Best Result: ' num2str(Best_BestTP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Worst Result: ' num2str(worst_BestTP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Best Model Z-Score : ' num2str(Best_zscore )]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s\n', RS_Best);
  fprintf(fMask, '%s', ['Best Model Ranksum Probability : ' num2str(Best_Ranksum_P)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'-------------------------------------------------\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', 'Average Model Run Test Corrects:');
  fprintf(fMask,'\n');
  
  S = rem(length(Avg_True_Pos),10);
  R = floor(length(Avg_True_Pos)/10);
  
  for i=1:R
    for j=1:10
        if j==1
            fprintf(fMask, '%s',num2str(Avg_True_Pos(10*(i-1)+j)));
        else
            fprintf(fMask, '\t%s',num2str(Avg_True_Pos(10*(i-1)+j)));
        end;
    end
    fprintf(fMask, '\n');
  end
  
  for j=1:S
        if j==1
            fprintf(fMask, '%s',num2str(Avg_True_Pos(10*R+j)));
        else
            fprintf(fMask, '\t%s',num2str(Avg_True_Pos(10*R+j)));
        end;
  end
  fprintf(fMask, '\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Avg Model Repeats'' Mean: ' num2str(Mean_Avg_TP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Avg Model Repeats'' Std: ' num2str(Std_Avg_TP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Best Result: ' num2str(Best_AvgTP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Worst Result: ' num2str(worst_AvgTP)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Average Model Z-Score'' : ' num2str(Avg_zscore )]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s\n', RS_Avg);
  fprintf(fMask, '%s', ['Avg. Model Ranksum Probability : ' num2str(Avg_Ranksum_P)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'-------------------------------------------------\n');
  fprintf(fMask,'\n');fprintf(fMask,'\n');
  fprintf(fMask,'%s\n','Average and Best Models'' Runs Ranksum Test');    
  fprintf(fMask, '%s\n', RS_Tot);
  fprintf(fMask, '%s', ['Best & Avg. Models'' Ranksum Probability : ' num2str(Tot_Ranksum_P)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask,'%s\n','Actual Runs Ranksum Test');    
  for k=1:length(True_Pos)
      Temp_TP = True_Pos;
      Temp_TP(k) = [];
      [Ranksum_P, Ranksum_H] = ranksum(True_Pos(k), Temp_TP);
		if Ranksum_H == 1
           RS_Avg = ['"Medians are equal" can be rejected at ' num2str(100*alpha) '% significance level'];
		else
           RS_Avg = ['"Medians are equal" can NOT be rejected at ' num2str(100*alpha) '% significance level'];
		end;
        fprintf(fMask, '%s\n', RS_Avg);
 end;
    
else
   display('Invalid File Name! Try again ');
   return;
end;

fclose(fMask);
