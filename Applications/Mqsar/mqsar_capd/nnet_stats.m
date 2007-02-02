function Feature_Freq = model_stats(Method,columnames)
% 
% Synopsis:
%         model_stats(varargin)
%
% This m-file reads the model and mask files for a specific run, and 
% finds the best and the average models using the Test correct Rates of 
% the models, and then generates the mask files for these two cases by 
% simply repeating the corresponding partiton column from the original 
% mask file. 
% 
% Optional Inputs
% 'GA', 'ga', 'Ga' : indicates that the method is Genetic algorithm which
% is different than others. 
%
% Ersin Bayram, January 2004
%---------------------------------------------------------------
 
%% Check arguments

warning off;
switch Method
case {'GA', 'ga', 'Ga'},
    % GA is the feature selection method
    Modelstr = 'SubModel';   % GA
otherwise
    % The rest
    Modelstr = 'Model';    % All methods except GA
end

[ModelName, FilePath] = uigetfile({'*.mod', 'Model Files {*.mod}'; ...
    '*.*', 'All Files (*.*)';}, 'Pick the Model File to Load');
modelfile = strcat(FilePath,ModelName);

if (ModelName ~= 0)
    ModelMat = textread(modelfile,'%s','delimiter','\t');
else
   display('Invalid File Name! Try again ');
   return;
end;
 
fid   = fopen(modelfile,'r');
ModelNum = fread(fid);
fclose(fid);
ModelNum = char(ModelNum');
Match_Ind = findstr(ModelNum,Modelstr);
Runs = length(Match_Ind);

Feature_Freq = zeros(length(columnames)-1,1);

% Now we are right at the starting point of each Numeric Test Correct Value
% Char by char extract them all
for i=1:length(Feature_Freq)
   Matchstr  = columnames(i);  
   k=0;
   for j=1:length(ModelMat) 
         if strcmp(ModelMat(j),Matchstr)
             k=k+1;
         end;      
   end;  
   Feature_Freq(i) = k;
end;
Feature_Freq = Feature_Freq/Runs*100;

 
