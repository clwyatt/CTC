function SOM_Stats_Pred_all(data, varargin)
%
% Generates stats on models
%
% Optional Inputs
% 'GA', 'ga', 'Ga' : indicates that the method is Genetic algorithm which
% is different than others. 
%
% Ersin Bayram, February 2004
%
%---------------------------------------------------------------

%% Check arguments
i=1;
Method = 0;

warning off;
i=1;
while i<=size(varargin,2), 
	if ischar(varargin{i}), 
		switch varargin{i}, 
		% unambiguous values
		case {'GA', 'ga', 'Ga'},
            % GA is the feature selection method
			Method = 1;             
        end
	end
    i=i+1;
end;    

Defaultfile = '/home/matlab/SBIR/*.mod';
[ModelName, FilePath] = uigetfile(Defaultfile, 'Load the Actual Model File');
modelfile = strcat(FilePath,ModelName);

if (ModelName ~= 0)
	fid   = fopen(modelfile,'r');
	ModelMat = fread(fid);
    ModelMat = char(ModelMat');
    fclose(fid);
else
   display('Invalid File Name! Try again ');
   return;
end;


if Method ==0
   Matchstr =  'Test Correct rate: ';    % All methods except GA
   Matchstr1 = 'Correct rate: ';
   Matchstr2 = 'varriable number: ';    % Yunde has a typo so double "r"
   Matchstr3 = 'Test false positives: ';
   Matchstr4 = 'Test false negatives: ';   
   Matchstr5 = 'false positives: ';
   Matchstr6 = 'false negatives: ';
   Matchstr7 = 'quantization error: ';
   Matchstr8 = 'topographic error: ';
else 
   Matchstr =  'Test Correct rate: ';    % GA
   Matchstr1 = 'Correct rate: ';
   Matchstr2 = 'variable number: ';     
   Matchstr3 = 'Test false positives: ';
   Matchstr4 = 'Test false negatives: ';   
   Matchstr5 = 'false positives: ';
   Matchstr6 = 'false negatives: ';
   Matchstr7 = 'quantization error: ';
   Matchstr8 = 'topographic error: ';
   Matchstr9 = 'Model_Test_TruePos: ';   % GA
   Matchstr10 = 'Model_Test_False_Pos: ';
   Matchstr11 = 'Model_Test_False_Neg: ';
   Matchstr12 = 'Model_TruePos: ';
   Matchstr13 = 'Model_False_Pos: ';   
   Matchstr14 = 'Model_False_Neg: '; 
end;    

%--------  Test Correct ------------
Match_Ind = findstr(ModelMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Test_Corr_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Test_Corr_Str = strcat(Test_Corr_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Test_Correct(i) = str2num(Test_Corr_Str);
end;   

% Find  the mean and best test correct rates
Mean_Test_Correct = mean(Test_Correct);
Std_Test_Correct = std(Test_Correct);
Max_Test_Correct = max(Test_Correct);
Min_Test_Correct = min(Test_Correct);


% Find the avg. and best models.
%Difference = Mean_TP - True_Pos
[Sorted,Ind] = sort(abs(Mean_Test_Correct - Test_Correct));
[Best_TP,Best_Ind] = max(Test_Correct);
% Pick the first ones if we have more than one
Avg_Model_Ind = Ind(1);
Best_Model_Ind = Best_Ind(1);

%--------  Training Correct ------------
Match_Ind = findstr(ModelMat,Matchstr1);
Start_Ind = Match_Ind +length(Matchstr1);

% Char by char extract them all
 for i=2:2:length(Match_Ind)
    j=0;
    Tr_Corr_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Tr_Corr_Str = strcat(Tr_Corr_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Tr_Correct(i/2) = str2num(Tr_Corr_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Tr_Correct = mean(Tr_Correct);
Std_Tr_Correct = std(Tr_Correct);
Max_Tr_Correct = max(Tr_Correct);
Min_Tr_Correct = min(Tr_Correct);

%--------  Variable Number  ------------
Match_Ind = findstr(ModelMat,Matchstr2);
Start_Ind = Match_Ind +length(Matchstr2);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Var_Num_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Var_Num_Str = strcat(Var_Num_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Var_Num(i) = str2num(Var_Num_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Var_Num = mean(Var_Num);
Std_Var_Num = std(Var_Num);
Max_Var_Num = max(Var_Num);
Min_Var_Num = min(Var_Num);

%--------  Test false positives: ------------
Match_Ind = findstr(ModelMat,Matchstr3);
Start_Ind = Match_Ind +length(Matchstr3);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Test_FalsePos_Str = '';
    length_TFP = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' ')) 
        Test_FalsePos_Str = strcat(Test_FalsePos_Str, ModelMat(Start_Ind(i)+j));
        if length_TFP == length(Test_FalsePos_Str)
            break;
        else
            length_TFP = length(Test_FalsePos_Str);
        end;    
        j = j+1;
   end;
   Test_FalsePos(i) = str2num(Test_FalsePos_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Test_FalsePos = mean(Test_FalsePos);
Std_Test_FalsePos = std(Test_FalsePos);
Max_Test_FalsePos = max(Test_FalsePos);
Min_Test_FalsePos = min(Test_FalsePos);

%--------  Test Negatives: ------------
Match_Ind = findstr(ModelMat,Matchstr4);
Start_Ind = Match_Ind +length(Matchstr4);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Test_FalseNeg_Str = '';
    length_TFN = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Test_FalseNeg_Str = strcat(Test_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
        if length_TFN== length(Test_FalseNeg_Str)
            break;
        else
            length_TFN = length(Test_FalseNeg_Str);
        end;    
        j = j+1;
   end;
   Test_FalseNeg(i) = str2num(Test_FalseNeg_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Test_FalseNeg = mean(Test_FalseNeg);
Std_Test_FalseNeg = std(Test_FalseNeg);
Max_Test_FalseNeg = max(Test_FalseNeg);
Min_Test_FalseNeg = min(Test_FalseNeg);


%--------  Tr false positives: ------------
Match_Ind = findstr(ModelMat,Matchstr5);
Start_Ind = Match_Ind +length(Matchstr5);

% Char by char extract them all
 for i=2:2:length(Match_Ind)
    j=0;
    Tr_FalsePos_Str = '';
    length_TrFP = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Tr_FalsePos_Str = strcat(Tr_FalsePos_Str, ModelMat(Start_Ind(i)+j));
        if length_TrFP == length(Tr_FalsePos_Str)
            break;
        else
            length_TrFP = length(Tr_FalsePos_Str);
        end;    
        j = j+1;
   end;
   Tr_FalsePos(i/2) = str2num(Tr_FalsePos_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Tr_FalsePos = mean(Tr_FalsePos);
Std_Tr_FalsePos = std(Tr_FalsePos);
Max_Tr_FalsePos = max(Tr_FalsePos);
Min_Tr_FalsePos = min(Tr_FalsePos);

%--------  Tr False Negatives: ------------
Match_Ind = findstr(ModelMat,Matchstr6);
Start_Ind = Match_Ind +length(Matchstr6);

% Char by char extract them all
 for i=2:2:length(Match_Ind)
    j=0;
    Tr_FalseNeg_Str = '';
    length_TrFN = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Tr_FalseNeg_Str = strcat(Tr_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
        if length_TrFN == length(Tr_FalseNeg_Str)
            break;
        else
            length_TrFN = length(Tr_FalseNeg_Str);
        end;    
        j = j+1;
   end;
   Tr_FalseNeg(i/2) = str2num(Tr_FalseNeg_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Tr_FalseNeg = mean(Tr_FalseNeg);
Std_Tr_FalseNeg = std(Tr_FalseNeg);
Max_Tr_FalseNeg = max(Tr_FalseNeg);
Min_Tr_FalseNeg = min(Tr_FalseNeg);

%--------  Quantization Error: ------------
Match_Ind = findstr(ModelMat,Matchstr7);
Start_Ind = Match_Ind +length(Matchstr7);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    QuantError_Str = '';
    length_QErr = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        QuantError_Str = strcat(QuantError_Str, ModelMat(Start_Ind(i)+j));
        if length_QErr == length(QuantError_Str)
            break;
        else
            length_QErr = length(QuantError_Str);
        end;    
        j = j+1;
   end;
   QuantError(i) = str2num(QuantError_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_QuantError = mean(QuantError);
Std_QuantError = std(QuantError);
Max_QuantError = max(QuantError);
Min_QuantError = min(QuantError);

%--------  Topographic Error ------------
Match_Ind = findstr(ModelMat,Matchstr8);
Start_Ind = Match_Ind +length(Matchstr8);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TopogError_Str = '';
    length_TopogErr = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        TopogError_Str = strcat(TopogError_Str, ModelMat(Start_Ind(i)+j));
        if length_TopogErr == length(TopogError_Str)
            break;
        else
            length_TopogErr = length(TopogError_Str);
        end;    
        j = j+1;
   end;
   TopogError(i) = str2num(TopogError_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_TopogError = mean(TopogError);
Std_TopogError = std(TopogError);
Max_TopogError = max(TopogError);
Min_TopogError = min(TopogError);

if Method % GA
    
    %--------  Model_Test_TruePos ------------
	Match_Ind = findstr(ModelMat,Matchstr9);
	Start_Ind = Match_Ind +length(Matchstr9);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_Test_TruePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_Test_TruePos_Str = strcat(Model_Test_TruePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_Test_TruePos(i) = str2num(Model_Test_TruePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_Test_TruePos = mean(Model_Test_TruePos);
	Std_Model_Test_TruePos = std(Model_Test_TruePos);
	Max_Model_Test_TruePos = max(Model_Test_TruePos);
	Min_Model_Test_TruePos = min(Model_Test_TruePos);
    
    % Find the avg. and best models.
    %Difference = Mean_TP - True_Pos
    [Sorted,Ind] = sort(abs(Mean_Model_Test_TruePos - Model_Test_TruePos));
    [Best_TP,Best_Ind] = max(Model_Test_TruePos);
    % Pick the first ones if we have more than one
    Avg_Model_Ind = Ind(1);
    Best_Model_Ind = Best_Ind(1);
    
    %--------  Model_Test_False_Pos ------------
	Match_Ind = findstr(ModelMat,Matchstr10);
	Start_Ind = Match_Ind +length(Matchstr10);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_Test_FalsePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_Test_FalsePos_Str = strcat(Model_Test_FalsePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_Test_FalsePos(i) = str2num(Model_Test_FalsePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_Test_FalsePos = mean(Model_Test_FalsePos);
	Std_Model_Test_FalsePos = std(Model_Test_FalsePos);
	Max_Model_Test_FalsePos = max(Model_Test_FalsePos);
	Min_Model_Test_FalsePos = min(Model_Test_FalsePos);
    
    %--------  Model_Test_False_Neg ------------
	Match_Ind = findstr(ModelMat,Matchstr11);
	Start_Ind = Match_Ind +length(Matchstr11);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_Test_FalseNeg_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_Test_FalseNeg_Str = strcat(Model_Test_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_Test_FalseNeg(i) = str2num(Model_Test_FalseNeg_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_Test_FalseNeg = mean(Model_Test_FalseNeg);
	Std_Model_Test_FalseNeg = std(Model_Test_FalseNeg);
	Max_Model_Test_FalseNeg = max(Model_Test_FalseNeg);
	Min_Model_Test_FalseNeg = min(Model_Test_FalseNeg);
    
    %--------  Model_TruePos ------------
	Match_Ind = findstr(ModelMat,Matchstr12);
	Start_Ind = Match_Ind +length(Matchstr12);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_TruePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_TruePos_Str = strcat(Model_TruePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_TruePos(i) = str2num(Model_TruePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_TruePos = mean(Model_TruePos);
	Std_Model_TruePos = std(Model_TruePos);
	Max_Model_TruePos = max(Model_TruePos);
	Min_Model_TruePos = min(Model_TruePos);
    
    %--------  Model_FalsePos ------------
	Match_Ind = findstr(ModelMat,Matchstr13);
	Start_Ind = Match_Ind +length(Matchstr13);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_FalsePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_FalsePos_Str = strcat(Model_FalsePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_FalsePos(i) = str2num(Model_FalsePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_FalsePos = mean(Model_FalsePos);
	Std_Model_FalsePos = std(Model_FalsePos);
	Max_Model_FalsePos = max(Model_FalsePos);
	Min_Model_FalsePos = min(Model_FalsePos);
    
    %--------  Model_FalseNeg ------------
	Match_Ind = findstr(ModelMat,Matchstr14);
	Start_Ind = Match_Ind +length(Matchstr14);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_FalseNeg_Str = '';
        length_Model_TrFN = 0;
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_FalseNeg_Str = strcat(Model_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
            if length_Model_TrFN == length(Model_FalseNeg_Str)
                break;
            else
                length_Model_TrFN = length(Model_FalseNeg_Str);
            end;    
            j = j+1;
       end;
       Model_FalseNeg(i) = str2num(Model_FalseNeg_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_FalseNeg = mean(Model_FalseNeg);
	Std_Model_FalseNeg = std(Model_FalseNeg);
	Max_Model_FalseNeg = max(Model_FalseNeg);
	Min_Model_FalseNeg = min(Model_FalseNeg);
    
end;


% Now Read the Prediction File
Defaultfile = '/home/matlab/SBIR/*.prd';
[PredictName, FilePath] = uigetfile(Defaultfile, 'Load the Prediction File for the Model');
Predictfile = strcat(FilePath,PredictName);

if (PredictName ~= 0)
	fid   = fopen(Predictfile,'r');
	PredictMat = fread(fid);
    PredictMat = char(PredictMat');
    fclose(fid);
else
   display('Invalid File Name! Try again ');
   return;
end;

  if Method ==0 % Need to modify these variable values based on Yunde's labels
   Matchstr =  'Prediction Correct rate: ';    % All methods except GA
   Matchstr1 = 'Prediction False Positives: ';
   Matchstr2 = 'Prediction False Negatives: ';    
  else 
   Matchstr =  'Prediction Correct rate: ';    %  GA
   Matchstr1 = 'Prediction False Positives: ';
   Matchstr2 = 'Prediction False Negatives: ';          
   Matchstr3 = 'Model Prediction Correct rate: ';
   Matchstr4 = 'Model Prediction False_pos: ';   
   Matchstr5 = 'Model Prediction False_neg: ';
  end;    
  
  %--------  Prediction Correct rate ------------
  Match_Ind = findstr(PredictMat,Matchstr);
  Start_Ind = Match_Ind +length(Matchstr);
	
  % Char by char extract them all
  for i=1:length(Match_Ind)
      j=0;
      Predict_Corr_Str = '';
      length_PCorr = 0;
      while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
            Predict_Corr_Str = strcat(Predict_Corr_Str, PredictMat(Start_Ind(i)+j));
            if length_PCorr == length(Predict_Corr_Str)
                break;
            else
                length_PCorr = length(Predict_Corr_Str);
            end;    
            j = j+1;
       end;
       Predict_Corr(i) = str2num(Predict_Corr_Str);
	end;   
	
   % Find  the mean, std, best, and worst 
  Mean_Predict_Corr = mean(Predict_Corr);
  Std_Predict_Corr = std(Predict_Corr); 
  Max_Predict_Corr = max(Predict_Corr);
  Min_Predict_Corr = min(Predict_Corr);
  
  %--------  Prediction False Positive ------------
  Match_Ind = findstr(PredictMat,Matchstr1);
  Start_Ind = Match_Ind +length(Matchstr1);
	
  % Char by char extract them all
  for i=1:length(Match_Ind)
      j=0;
      Predict_FalsePos_Str = '';
      length_FalsePos = 0;
      while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
            Predict_FalsePos_Str = strcat(Predict_FalsePos_Str, PredictMat(Start_Ind(i)+j));
            if length_FalsePos == length(Predict_FalsePos_Str)
                break;
            else
                length_FalsePos = length(Predict_FalsePos_Str);
            end;    
            j = j+1;
       end;
       Predict_FalsePos(i) = str2num(Predict_FalsePos_Str);
	end;   
	
   % Find  the mean, std, best, and worst 
  Mean_Predict_FalsePos = mean(Predict_FalsePos);
  Std_Predict_FalsePos = std(Predict_FalsePos); 
  Max_Predict_FalsePos = max(Predict_FalsePos);
  Min_Predict_FalsePos = min(Predict_FalsePos);
 
    %--------  Prediction False Negative ------------
  Match_Ind = findstr(PredictMat,Matchstr2);
  Start_Ind = Match_Ind +length(Matchstr2);
	
  % Char by char extract them all
  for i=1:length(Match_Ind)
      j=0;
      Predict_FalseNeg_Str = '';
      length_FalseNeg = 0;
      while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
            Predict_FalseNeg_Str = strcat(Predict_FalseNeg_Str, PredictMat(Start_Ind(i)+j));
            if length_FalseNeg == length(Predict_FalseNeg_Str)
                break;
            else
                length_FalseNeg = length(Predict_FalseNeg_Str);
            end;    
            j = j+1;
       end;
       Predict_FalseNeg(i) = str2num(Predict_FalseNeg_Str);
	end;   
	
   % Find  the mean, std, best, and worst 
  Mean_Predict_FalseNeg = mean(Predict_FalseNeg);
  Std_Predict_FalseNeg = std(Predict_FalseNeg); 
  Max_Predict_FalseNeg = max(Predict_FalseNeg);
  Min_Predict_FalseNeg = min(Predict_FalseNeg);
    
  %--------  Model Prediction Correct rate ------------
  if Method
  
      Match_Ind = findstr(PredictMat,Matchstr3);
      Start_Ind = Match_Ind +length(Matchstr3);
		
      % Char by char extract them all
      for i=1:length(Match_Ind)
          j=0;
          Model_Predict_Corr_Str = '';
          length_Model_PCorr = 0;
          while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
                Model_Predict_Corr_Str = strcat(Model_Predict_Corr_Str, PredictMat(Start_Ind(i)+j));
                if length_Model_PCorr == length(Model_Predict_Corr_Str)
                    break;
                else
                    length_Model_PCorr = length(Model_Predict_Corr_Str);
                end;    
                j = j+1;
           end;
           Model_Predict_Corr(i) = str2num(Model_Predict_Corr_Str);
		end;   
		
       % Find  the mean, std, best, and worst 
      Mean_Model_Predict_Corr = mean(Model_Predict_Corr);
      Std_Model_Predict_Corr = std(Model_Predict_Corr); 
      Max_Model_Predict_Corr = max(Model_Predict_Corr);
      Min_Model_Predict_Corr = min(Model_Predict_Corr);
      
      %--------  Model_Prediction False Positive ------------
      Match_Ind = findstr(PredictMat,Matchstr4);
      Start_Ind = Match_Ind +length(Matchstr4);
		
      % Char by char extract them all
      for i=1:length(Match_Ind)
          j=0;
          Model_Predict_FalsePos_Str = '';
          length_Model_FalsePos = 0;
          while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
                Model_Predict_FalsePos_Str = strcat(Model_Predict_FalsePos_Str, PredictMat(Start_Ind(i)+j));
                if length_Model_FalsePos == length(Model_Predict_FalsePos_Str)
                    break;
                else
                    length_Model_FalsePos = length(Model_Predict_FalsePos_Str);
                end;    
                j = j+1;
           end;
           Model_Predict_FalsePos(i) = str2num(Model_Predict_FalsePos_Str);
		end;   
		
       % Find  the mean, std, best, and worst 
      Mean_Model_Predict_FalsePos = mean(Model_Predict_FalsePos);
      Std_Model_Predict_FalsePos = std(Model_Predict_FalsePos); 
      Max_Model_Predict_FalsePos = max(Model_Predict_FalsePos);
      Min_Model_Predict_FalsePos = min(Model_Predict_FalsePos);
     
        %--------  Model_Prediction False Negative ------------
      Match_Ind = findstr(PredictMat,Matchstr5);
      Start_Ind = Match_Ind +length(Matchstr5);
		
      % Char by char extract them all
      for i=1:length(Match_Ind)
          j=0;
          Model_Predict_FalseNeg_Str = '';
          length_Model_FalseNeg = 0;
          while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
                Model_Predict_FalseNeg_Str = strcat(Model_Predict_FalseNeg_Str, PredictMat(Start_Ind(i)+j));
                if length_Model_FalseNeg == length(Model_Predict_FalseNeg_Str)
                    break;
                else
                    length_Model_FalseNeg = length(Model_Predict_FalseNeg_Str);
                end;    
                j = j+1;
           end;
           Model_Predict_FalseNeg(i) = str2num(Model_Predict_FalseNeg_Str);
		end;   
		
       % Find  the mean, std, best, and worst 
      Mean_Model_Predict_FalseNeg = mean(Model_Predict_FalseNeg);
      Std_Model_Predict_FalseNeg = std(Model_Predict_FalseNeg); 
      Max_Model_Predict_FalseNeg = max(Model_Predict_FalseNeg);
      Min_Model_Predict_FalseNeg = min(Model_Predict_FalseNeg);
      
  end;

%---------------------------------------------
% Generate the Mask file for the average Model found for randomization	
Defaultfile = strcat(FilePath,ModelName(1:end-4),'_Stats_Long.txt');	
[FName, FPath] = uiputfile(Defaultfile,'Txt File to Write Stat Results');
newmaskfile = strcat(FPath,FName);
	
if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask,['Data Name: ' data]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Model File Name: ' ModelName]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Prediction File Name: ' PredictName]);
  fprintf(fMask,'\n\n');
  fprintf(fMask, '%s\n', 'TRAINING STATS');
  fprintf(fMask, '%s', '+++++++++++++++++++++++++++++++++++++++++++++++++++++');
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  
  if Method % GA
      fprintf(fMask, '%s\n', 'INDIVIDUAL Model Stats');
  end;
  
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Number of Features: ' num2str(Mean_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Number of Features: ' num2str(Std_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Number of Features: ' num2str(Max_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Number of Features: ' num2str(Min_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Quantization Error: ' num2str(Mean_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Quantization Error: ' num2str(Std_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Quantization Error: ' num2str(Max_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Quantization Error: ' num2str(Min_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Topographical Error: ' num2str(Mean_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Topographical Error: ' num2str(Std_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Topographical Error: ' num2str(Max_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Topographical Error: ' num2str(Min_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
 
  if Method % GA
      fprintf(fMask,'\n');
      fprintf(fMask, '%s\n', 'COLLECTIVE Model Stats');
      fprintf(fMask, '%s', '-----------------------------------------------------');
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n'); 
      fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Model_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Model_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Model_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Model_FalseNeg)]);
      fprintf(fMask,'\n');
  end;
  
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', '====================================================');
  fprintf(fMask,'\n\n');
  fprintf(fMask, '%s\n', 'TEST STATS');
  fprintf(fMask, '%s', '+++++++++++++++++++++++++++++++++++++++++++++++++++++');
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask, '%s\n', 'INDIVIDUAL Model Stats');
  end;
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask,'\n');
      fprintf(fMask, '%s\n', 'COLLECTIVE Model Stats');
      fprintf(fMask, '%s', '-----------------------------------------------------');
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n'); 
      fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
  end;  
  
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', '====================================================');
  fprintf(fMask,'\n\n');
  fprintf(fMask, '%s\n', 'PREDICTION STATS');
  fprintf(fMask, '%s', '+++++++++++++++++++++++++++++++++++++++++++++++++++++');
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask, '%s\n', 'INDIVIDUAL Model Stats');
  end;
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask,'\n');
      fprintf(fMask, '%s\n', 'COLLECTIVE Model Stats');
      fprintf(fMask, '%s', '-----------------------------------------------------');
      fprintf(fMask,'\n');    
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      
  end;  
  
  fclose(fMask);
end;

%%%%%%%%%%%%%%%% AVG MODEL FILE
%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Defaultfile = '/home/matlab/SBIR/*.mod';
[ModelName, FilePath] = uigetfile(Defaultfile, 'Load the Avg. Model File');
modelfile = strcat(FilePath,ModelName);

if (ModelName ~= 0)
	fid   = fopen(modelfile,'r');
	ModelMat = fread(fid);
    ModelMat = char(ModelMat');
    fclose(fid);
else
   display('Invalid File Name! Try again ');
   return;
end;


if Method ==0
   Matchstr =  'Test Correct rate: ';    % All methods except GA
   Matchstr1 = 'Correct rate: ';
   Matchstr2 = 'varriable number: ';    % Yunde has a typo so double "r"
   Matchstr3 = 'Test false positives: ';
   Matchstr4 = 'Test false negatives: ';   
   Matchstr5 = 'false positives: ';
   Matchstr6 = 'false negatives: ';
   Matchstr7 = 'quantization error: ';
   Matchstr8 = 'topographic error: ';
else 
   Matchstr =  'Test Correct rate: ';    % GA
   Matchstr1 = 'Correct rate: ';
   Matchstr2 = 'variable number: ';     
   Matchstr3 = 'Test false positives: ';
   Matchstr4 = 'Test false negatives: ';   
   Matchstr5 = 'false positives: ';
   Matchstr6 = 'false negatives: ';
   Matchstr7 = 'quantization error: ';
   Matchstr8 = 'topographic error: ';
   Matchstr9 = 'Model_Test_TruePos: ';   % GA
   Matchstr10 = 'Model_Test_False_Pos: ';
   Matchstr11 = 'Model_Test_False_Neg: ';
   Matchstr12 = 'Model_TruePos: ';
   Matchstr13 = 'Model_False_Pos: ';   
   Matchstr14 = 'Model_False_Neg: '; 
end;    

%--------  Test Correct ------------
Match_Ind = findstr(ModelMat,Matchstr);
Start_Ind = Match_Ind +length(Matchstr);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Test_Corr_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Test_Corr_Str = strcat(Test_Corr_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Avg_Test_Correct(i) = str2num(Test_Corr_Str);
end;   

% Find  the mean and best test correct rates
Mean_Avg_Test_Correct = mean(Avg_Test_Correct);
Std_Avg_Test_Correct = std(Avg_Test_Correct);
Max_Avg_Test_Correct = max(Avg_Test_Correct);
Min_Avg_Test_Correct = min(Avg_Test_Correct);

%--------  Training Correct ------------
Match_Ind = findstr(ModelMat,Matchstr1);
Start_Ind = Match_Ind +length(Matchstr1);

% Char by char extract them all
 for i=2:2:length(Match_Ind)
    j=0;
    Tr_Corr_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Tr_Corr_Str = strcat(Tr_Corr_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Tr_Correct(i/2) = str2num(Tr_Corr_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Tr_Correct = mean(Tr_Correct);
Std_Tr_Correct = std(Tr_Correct);
Max_Tr_Correct = max(Tr_Correct);
Min_Tr_Correct = min(Tr_Correct);

%--------  Variable Number  ------------
Match_Ind = findstr(ModelMat,Matchstr2);
Start_Ind = Match_Ind +length(Matchstr2);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Var_Num_Str = '';
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Var_Num_Str = strcat(Var_Num_Str, ModelMat(Start_Ind(i)+j));
        j = j+1;
   end;
   Var_Num(i) = str2num(Var_Num_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Var_Num = mean(Var_Num);
Std_Var_Num = std(Var_Num);
Max_Var_Num = max(Var_Num);
Min_Var_Num = min(Var_Num);

%--------  Test false positives: ------------
Match_Ind = findstr(ModelMat,Matchstr3);
Start_Ind = Match_Ind +length(Matchstr3);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Test_FalsePos_Str = '';
    length_TFP = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' ')) 
        Test_FalsePos_Str = strcat(Test_FalsePos_Str, ModelMat(Start_Ind(i)+j));
        if length_TFP == length(Test_FalsePos_Str)
            break;
        else
            length_TFP = length(Test_FalsePos_Str);
        end;    
        j = j+1;
   end;
   Test_FalsePos(i) = str2num(Test_FalsePos_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Test_FalsePos = mean(Test_FalsePos);
Std_Test_FalsePos = std(Test_FalsePos);
Max_Test_FalsePos = max(Test_FalsePos);
Min_Test_FalsePos = min(Test_FalsePos);

%--------  Test Negatives: ------------
Match_Ind = findstr(ModelMat,Matchstr4);
Start_Ind = Match_Ind +length(Matchstr4);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    Test_FalseNeg_Str = '';
    length_TFN = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Test_FalseNeg_Str = strcat(Test_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
        if length_TFN== length(Test_FalseNeg_Str)
            break;
        else
            length_TFN = length(Test_FalseNeg_Str);
        end;    
        j = j+1;
   end;
   Test_FalseNeg(i) = str2num(Test_FalseNeg_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Test_FalseNeg = mean(Test_FalseNeg);
Std_Test_FalseNeg = std(Test_FalseNeg);
Max_Test_FalseNeg = max(Test_FalseNeg);
Min_Test_FalseNeg = min(Test_FalseNeg);


%--------  Tr false positives: ------------
Match_Ind = findstr(ModelMat,Matchstr5);
Start_Ind = Match_Ind +length(Matchstr5);

% Char by char extract them all
 for i=2:2:length(Match_Ind)
    j=0;
    Tr_FalsePos_Str = '';
    length_TrFP = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Tr_FalsePos_Str = strcat(Tr_FalsePos_Str, ModelMat(Start_Ind(i)+j));
        if length_TrFP == length(Tr_FalsePos_Str)
            break;
        else
            length_TrFP = length(Tr_FalsePos_Str);
        end;    
        j = j+1;
   end;
   Tr_FalsePos(i/2) = str2num(Tr_FalsePos_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Tr_FalsePos = mean(Tr_FalsePos);
Std_Tr_FalsePos = std(Tr_FalsePos);
Max_Tr_FalsePos = max(Tr_FalsePos);
Min_Tr_FalsePos = min(Tr_FalsePos);

%--------  Tr False Negatives: ------------
Match_Ind = findstr(ModelMat,Matchstr6);
Start_Ind = Match_Ind +length(Matchstr6);

% Char by char extract them all
 for i=2:2:length(Match_Ind)
    j=0;
    Tr_FalseNeg_Str = '';
    length_TrFN = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        Tr_FalseNeg_Str = strcat(Tr_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
        if length_TrFN == length(Tr_FalseNeg_Str)
            break;
        else
            length_TrFN = length(Tr_FalseNeg_Str);
        end;    
        j = j+1;
   end;
   Tr_FalseNeg(i/2) = str2num(Tr_FalseNeg_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_Tr_FalseNeg = mean(Tr_FalseNeg);
Std_Tr_FalseNeg = std(Tr_FalseNeg);
Max_Tr_FalseNeg = max(Tr_FalseNeg);
Min_Tr_FalseNeg = min(Tr_FalseNeg);

%--------  Quantization Error: ------------
Match_Ind = findstr(ModelMat,Matchstr7);
Start_Ind = Match_Ind +length(Matchstr7);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    QuantError_Str = '';
    length_QErr = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        QuantError_Str = strcat(QuantError_Str, ModelMat(Start_Ind(i)+j));
        if length_QErr == length(QuantError_Str)
            break;
        else
            length_QErr = length(QuantError_Str);
        end;    
        j = j+1;
   end;
   QuantError(i) = str2num(QuantError_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_QuantError = mean(QuantError);
Std_QuantError = std(QuantError);
Max_QuantError = max(QuantError);
Min_QuantError = min(QuantError);

%--------  Topographic Error ------------
Match_Ind = findstr(ModelMat,Matchstr8);
Start_Ind = Match_Ind +length(Matchstr8);

% Char by char extract them all
 for i=1:length(Match_Ind)
    j=0;
    TopogError_Str = '';
    length_TopogErr = 0;
   while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
        TopogError_Str = strcat(TopogError_Str, ModelMat(Start_Ind(i)+j));
        if length_TopogErr == length(TopogError_Str)
            break;
        else
            length_TopogErr = length(TopogError_Str);
        end;    
        j = j+1;
   end;
   TopogError(i) = str2num(TopogError_Str);
end;   

% Find  the mean, std, best, and worst 
Mean_TopogError = mean(TopogError);
Std_TopogError = std(TopogError);
Max_TopogError = max(TopogError);
Min_TopogError = min(TopogError);

if Method % GA
    
    %--------  Model_Test_TruePos ------------
	Match_Ind = findstr(ModelMat,Matchstr9);
	Start_Ind = Match_Ind +length(Matchstr9);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_Test_TruePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_Test_TruePos_Str = strcat(Model_Test_TruePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_Test_TruePos(i) = str2num(Model_Test_TruePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_Test_TruePos = mean(Model_Test_TruePos);
	Std_Model_Test_TruePos = std(Model_Test_TruePos);
	Max_Model_Test_TruePos = max(Model_Test_TruePos);
	Min_Model_Test_TruePos = min(Model_Test_TruePos);
    
    % Find the avg. and best models.
    %Difference = Mean_TP - True_Pos
    [Sorted,Ind] = sort(abs(Mean_Model_Test_TruePos - Model_Test_TruePos));
    [Best_TP,Best_Ind] = max(Model_Test_TruePos);
    % Pick the first ones if we have more than one
    Avg_Model_Ind = Ind(1);
    Best_Model_Ind = Best_Ind(1);
    
    %--------  Model_Test_False_Pos ------------
	Match_Ind = findstr(ModelMat,Matchstr10);
	Start_Ind = Match_Ind +length(Matchstr10);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_Test_FalsePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_Test_FalsePos_Str = strcat(Model_Test_FalsePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_Test_FalsePos(i) = str2num(Model_Test_FalsePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_Test_FalsePos = mean(Model_Test_FalsePos);
	Std_Model_Test_FalsePos = std(Model_Test_FalsePos);
	Max_Model_Test_FalsePos = max(Model_Test_FalsePos);
	Min_Model_Test_FalsePos = min(Model_Test_FalsePos);
    
    %--------  Model_Test_False_Neg ------------
	Match_Ind = findstr(ModelMat,Matchstr11);
	Start_Ind = Match_Ind +length(Matchstr11);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_Test_FalseNeg_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_Test_FalseNeg_Str = strcat(Model_Test_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_Test_FalseNeg(i) = str2num(Model_Test_FalseNeg_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_Test_FalseNeg = mean(Model_Test_FalseNeg);
	Std_Model_Test_FalseNeg = std(Model_Test_FalseNeg);
	Max_Model_Test_FalseNeg = max(Model_Test_FalseNeg);
	Min_Model_Test_FalseNeg = min(Model_Test_FalseNeg);
    
    %--------  Model_TruePos ------------
	Match_Ind = findstr(ModelMat,Matchstr12);
	Start_Ind = Match_Ind +length(Matchstr12);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_TruePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_TruePos_Str = strcat(Model_TruePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_TruePos(i) = str2num(Model_TruePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_TruePos = mean(Model_TruePos);
	Std_Model_TruePos = std(Model_TruePos);
	Max_Model_TruePos = max(Model_TruePos);
	Min_Model_TruePos = min(Model_TruePos);
    
    %--------  Model_FalsePos ------------
	Match_Ind = findstr(ModelMat,Matchstr13);
	Start_Ind = Match_Ind +length(Matchstr13);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_FalsePos_Str = '';
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_FalsePos_Str = strcat(Model_FalsePos_Str, ModelMat(Start_Ind(i)+j));
            j = j+1;
       end;
       Model_FalsePos(i) = str2num(Model_FalsePos_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_FalsePos = mean(Model_FalsePos);
	Std_Model_FalsePos = std(Model_FalsePos);
	Max_Model_FalsePos = max(Model_FalsePos);
	Min_Model_FalsePos = min(Model_FalsePos);
    
    %--------  Model_FalseNeg ------------
	Match_Ind = findstr(ModelMat,Matchstr14);
	Start_Ind = Match_Ind +length(Matchstr14);
	
	% Char by char extract them all
     for i=1:length(Match_Ind)
        j=0;
        Model_FalseNeg_Str = '';
        length_Model_TrFN = 0;
       while not(strcmp(ModelMat(Start_Ind(i)+j),' '))
            Model_FalseNeg_Str = strcat(Model_FalseNeg_Str, ModelMat(Start_Ind(i)+j));
            if length_Model_TrFN == length(Model_FalseNeg_Str)
                break;
            else
                length_Model_TrFN = length(Model_FalseNeg_Str);
            end;    
            j = j+1;
       end;
       Model_FalseNeg(i) = str2num(Model_FalseNeg_Str);
	end;   
	
	% Find  the mean, std, best, and worst 
	Mean_Model_FalseNeg = mean(Model_FalseNeg);
	Std_Model_FalseNeg = std(Model_FalseNeg);
	Max_Model_FalseNeg = max(Model_FalseNeg);
	Min_Model_FalseNeg = min(Model_FalseNeg);
    
end;


% Now Read the Prediction File
Defaultfile = '/home/matlab/SBIR/*.prd';
[PredictName, FilePath] = uigetfile(Defaultfile, 'Load the Prediction File for the Model');
Predictfile = strcat(FilePath,PredictName);

if (PredictName ~= 0)
	fid   = fopen(Predictfile,'r');
	PredictMat = fread(fid);
    PredictMat = char(PredictMat');
    fclose(fid);
else
   display('Invalid File Name! Try again ');
   return;
end;

  if Method ==0 % Need to modify these variable values based on Yunde's labels
   Matchstr =  'Prediction Correct rate: ';    % All methods except GA
   Matchstr1 = 'Prediction False Positives: ';
   Matchstr2 = 'Prediction False Negatives: ';    
  else 
   Matchstr =  'Prediction Correct rate: ';    %  GA
   Matchstr1 = 'Prediction False Positives: ';
   Matchstr2 = 'Prediction False Negatives: ';          
   Matchstr3 = 'Model Prediction Correct rate: ';
   Matchstr4 = 'Model Prediction False_pos: ';   
   Matchstr5 = 'Model Prediction False_neg: ';
  end;    
  
  %--------  Prediction Correct rate ------------
  Match_Ind = findstr(PredictMat,Matchstr);
  Start_Ind = Match_Ind +length(Matchstr);
	
  % Char by char extract them all
  for i=1:length(Match_Ind)
      j=0;
      Predict_Corr_Str = '';
      length_PCorr = 0;
      while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
            Predict_Corr_Str = strcat(Predict_Corr_Str, PredictMat(Start_Ind(i)+j));
            if length_PCorr == length(Predict_Corr_Str)
                break;
            else
                length_PCorr = length(Predict_Corr_Str);
            end;    
            j = j+1;
       end;
       Predict_Corr(i) = str2num(Predict_Corr_Str);
	end;   
	
   % Find  the mean, std, best, and worst 
  Mean_Predict_Corr = mean(Predict_Corr);
  Std_Predict_Corr = std(Predict_Corr); 
  Max_Predict_Corr = max(Predict_Corr);
  Min_Predict_Corr = min(Predict_Corr);
  
  %--------  Prediction False Positive ------------
  Match_Ind = findstr(PredictMat,Matchstr1);
  Start_Ind = Match_Ind +length(Matchstr1);
	
  % Char by char extract them all
  for i=1:length(Match_Ind)
      j=0;
      Predict_FalsePos_Str = '';
      length_FalsePos = 0;
      while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
            Predict_FalsePos_Str = strcat(Predict_FalsePos_Str, PredictMat(Start_Ind(i)+j));
            if length_FalsePos == length(Predict_FalsePos_Str)
                break;
            else
                length_FalsePos = length(Predict_FalsePos_Str);
            end;    
            j = j+1;
       end;
       Predict_FalsePos(i) = str2num(Predict_FalsePos_Str);
	end;   
	
   % Find  the mean, std, best, and worst 
  Mean_Predict_FalsePos = mean(Predict_FalsePos);
  Std_Predict_FalsePos = std(Predict_FalsePos); 
  Max_Predict_FalsePos = max(Predict_FalsePos);
  Min_Predict_FalsePos = min(Predict_FalsePos);
 
    %--------  Prediction False Negative ------------
  Match_Ind = findstr(PredictMat,Matchstr2);
  Start_Ind = Match_Ind +length(Matchstr2);
	
  % Char by char extract them all
  for i=1:length(Match_Ind)
      j=0;
      Predict_FalseNeg_Str = '';
      length_FalseNeg = 0;
      while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
            Predict_FalseNeg_Str = strcat(Predict_FalseNeg_Str, PredictMat(Start_Ind(i)+j));
            if length_FalseNeg == length(Predict_FalseNeg_Str)
                break;
            else
                length_FalseNeg = length(Predict_FalseNeg_Str);
            end;    
            j = j+1;
       end;
       Predict_FalseNeg(i) = str2num(Predict_FalseNeg_Str);
	end;   
	
   % Find  the mean, std, best, and worst 
  Mean_Predict_FalseNeg = mean(Predict_FalseNeg);
  Std_Predict_FalseNeg = std(Predict_FalseNeg); 
  Max_Predict_FalseNeg = max(Predict_FalseNeg);
  Min_Predict_FalseNeg = min(Predict_FalseNeg);
    
  %--------  Model Prediction Correct rate ------------
  if Method
  
      Match_Ind = findstr(PredictMat,Matchstr3);
      Start_Ind = Match_Ind +length(Matchstr3);
		
      % Char by char extract them all
      for i=1:length(Match_Ind)
          j=0;
          Model_Predict_Corr_Str = '';
          length_Model_PCorr = 0;
          while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
                Model_Predict_Corr_Str = strcat(Model_Predict_Corr_Str, PredictMat(Start_Ind(i)+j));
                if length_Model_PCorr == length(Model_Predict_Corr_Str)
                    break;
                else
                    length_Model_PCorr = length(Model_Predict_Corr_Str);
                end;    
                j = j+1;
           end;
           Model_Predict_Corr(i) = str2num(Model_Predict_Corr_Str);
		end;   
		
       % Find  the mean, std, best, and worst 
      Mean_Model_Predict_Corr = mean(Model_Predict_Corr);
      Std_Model_Predict_Corr = std(Model_Predict_Corr); 
      Max_Model_Predict_Corr = max(Model_Predict_Corr);
      Min_Model_Predict_Corr = min(Model_Predict_Corr);
      
      %--------  Model_Prediction False Positive ------------
      Match_Ind = findstr(PredictMat,Matchstr4);
      Start_Ind = Match_Ind +length(Matchstr4);
		
      % Char by char extract them all
      for i=1:length(Match_Ind)
          j=0;
          Model_Predict_FalsePos_Str = '';
          length_Model_FalsePos = 0;
          while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
                Model_Predict_FalsePos_Str = strcat(Model_Predict_FalsePos_Str, PredictMat(Start_Ind(i)+j));
                if length_Model_FalsePos == length(Model_Predict_FalsePos_Str)
                    break;
                else
                    length_Model_FalsePos = length(Model_Predict_FalsePos_Str);
                end;    
                j = j+1;
           end;
           Model_Predict_FalsePos(i) = str2num(Model_Predict_FalsePos_Str);
		end;   
		
       % Find  the mean, std, best, and worst 
      Mean_Model_Predict_FalsePos = mean(Model_Predict_FalsePos);
      Std_Model_Predict_FalsePos = std(Model_Predict_FalsePos); 
      Max_Model_Predict_FalsePos = max(Model_Predict_FalsePos);
      Min_Model_Predict_FalsePos = min(Model_Predict_FalsePos);
     
        %--------  Model_Prediction False Negative ------------
      Match_Ind = findstr(PredictMat,Matchstr5);
      Start_Ind = Match_Ind +length(Matchstr5);
		
      % Char by char extract them all
      for i=1:length(Match_Ind)
          j=0;
          Model_Predict_FalseNeg_Str = '';
          length_Model_FalseNeg = 0;
          while not(strcmp(PredictMat(Start_Ind(i)+j),' ')) 
                Model_Predict_FalseNeg_Str = strcat(Model_Predict_FalseNeg_Str, PredictMat(Start_Ind(i)+j));
                if length_Model_FalseNeg == length(Model_Predict_FalseNeg_Str)
                    break;
                else
                    length_Model_FalseNeg = length(Model_Predict_FalseNeg_Str);
                end;    
                j = j+1;
           end;
           Model_Predict_FalseNeg(i) = str2num(Model_Predict_FalseNeg_Str);
		end;   
		
       % Find  the mean, std, best, and worst 
      Mean_Model_Predict_FalseNeg = mean(Model_Predict_FalseNeg);
      Std_Model_Predict_FalseNeg = std(Model_Predict_FalseNeg); 
      Max_Model_Predict_FalseNeg = max(Model_Predict_FalseNeg);
      Min_Model_Predict_FalseNeg = min(Model_Predict_FalseNeg);
      
  end;

%---------------------------------------------
% Generate the Mask file for the average Model found for randomization	
Defaultfile = strcat(FilePath,ModelName(1:end-4),'_Stats_Long.txt');	
[FName, FPath] = uiputfile(Defaultfile,'Txt File to Write Stat Results');
newmaskfile = strcat(FPath,FName);
	
if (FName ~= 0)
  fMask = fopen(newmaskfile, 'w');
  fprintf(fMask,['Data Name: ' data]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Model File Name: ' ModelName]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Prediction File Name: ' PredictName]);
  fprintf(fMask,'\n\n');
  fprintf(fMask, '%s\n', 'TRAINING STATS');
  fprintf(fMask, '%s', '+++++++++++++++++++++++++++++++++++++++++++++++++++++');
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  
  if Method % GA
      fprintf(fMask, '%s\n', 'INDIVIDUAL Model Stats');
  end;
  
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Tr_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Tr_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Tr_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Number of Features: ' num2str(Mean_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Number of Features: ' num2str(Std_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Number of Features: ' num2str(Max_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Number of Features: ' num2str(Min_Var_Num)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Quantization Error: ' num2str(Mean_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Quantization Error: ' num2str(Std_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Quantization Error: ' num2str(Max_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Quantization Error: ' num2str(Min_QuantError)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Topographical Error: ' num2str(Mean_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Topographical Error: ' num2str(Std_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Topographical Error: ' num2str(Max_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Topographical Error: ' num2str(Min_TopogError)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
 
  if Method % GA
      fprintf(fMask,'\n');
      fprintf(fMask, '%s\n', 'COLLECTIVE Model Stats');
      fprintf(fMask, '%s', '-----------------------------------------------------');
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Model_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n'); 
      fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Model_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Model_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Model_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Model_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Model_FalseNeg)]);
      fprintf(fMask,'\n');
  end;
  
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', '====================================================');
  fprintf(fMask,'\n\n');
  fprintf(fMask, '%s\n', 'TEST STATS');
  fprintf(fMask, '%s', '+++++++++++++++++++++++++++++++++++++++++++++++++++++');
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask, '%s\n', 'INDIVIDUAL Model Stats');
  end;
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Test_Correct)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Test_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Test_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask,'\n');
      fprintf(fMask, '%s\n', 'COLLECTIVE Model Stats');
      fprintf(fMask, '%s', '-----------------------------------------------------');
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Model_Test_TruePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n'); 
      fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Model_Test_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Model_Test_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
  end;  
  
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', '====================================================');
  fprintf(fMask,'\n\n');
  fprintf(fMask, '%s\n', 'PREDICTION STATS');
  fprintf(fMask, '%s', '+++++++++++++++++++++++++++++++++++++++++++++++++++++');
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask, '%s\n', 'INDIVIDUAL Model Stats');
  end;
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Predict_Corr)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Predict_FalsePos)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Predict_FalseNeg)]);
  fprintf(fMask,'\n');
  fprintf(fMask,'\n');
  if Method % GA
      fprintf(fMask,'\n');
      fprintf(fMask, '%s\n', 'COLLECTIVE Model Stats');
      fprintf(fMask, '%s', '-----------------------------------------------------');
      fprintf(fMask,'\n');    
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean Correct Rate: ' num2str(Mean_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in Correct Rate: ' num2str(Std_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max Correct Rate: ' num2str(Max_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min Correct Rate: ' num2str(Min_Model_Predict_Corr)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Positive Rate: ' num2str(Mean_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Positive Rate: ' num2str(Std_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Positive Rate: ' num2str(Max_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Positive Rate: ' num2str(Min_Model_Predict_FalsePos)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Mean False Negative Rate: ' num2str(Mean_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Std. in False Negative Rate: ' num2str(Std_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Max False Negative Rate: ' num2str(Max_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask, '%s', ['Min False Negative Rate: ' num2str(Min_Model_Predict_FalseNeg)]);
      fprintf(fMask,'\n');
      fprintf(fMask,'\n');
      
  end;  
  
  fclose(fMask);
end;