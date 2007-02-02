function [tempgenome, bestgenome, bestfitness,genCounter,tcount] = hill_feature(bestgenome, bestfitness, tempfitness, tempgenome, ...
                                              maxrun, genCounter,tcount, mutate, varargin);
%  hillclimbing algorithm
%
% [tempgenome, tempfitness, bestgenome, bestfitness,tcount] = hill_feature(bestgenome, bestfitness, tempfitness, tempgenome, ...
%                                              maxrun, tcount, mutate, 'Fix_Desc_Num', 20);
% 
%	Input  arguments ([]'s are optional): 
%	data	(matrix)	training data matrix
%	bins	(vector)	training sample's categories
%	bioact	(vector)	biological activities of compounds
%	varnames(vector)	list of variable names
%   maxrun		(scalar)  Specifies the max iterations by following value
%	mutate		(scalar)  Spedifies the mutate number between generations by following value
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
%	Fix_Desc_Num		*(string)	Specifies the SA process using fix
%	                    length descriptors by following value 
%  
%   Output arguments
%
%

% June 16, 2004
% Ersin Bayram  ebayram@wfubmc.edu
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% Check arguments
i=1;
while i<=size(varargin,2), 
	argok = 1;
	if ischar(varargin{i}), 
		switch varargin{i}, 
            case {'DescLimit'},	
                i = i + 1; 
                DescLimit = varargin{i};
                i = i + 1;
                DescNum = varargin{i};
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(hillsom) Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end

% Execute a single step of Hill Climbing
tcount = tcount + 1;
genCounter  = genCounter + 1;
if tempfitness > bestfitness
    bestfitness = tempfitness;
	bestgenome = tempgenome;     
    tcount = 0;
end;     

% create genome mutation mask and implement mutations
tempgenome = bestgenome;
switch DescLimit
case {'None','Fixed Initial'}
    mutind = randperm(length(bestgenome));
	tempgenome(mutind(1:mutate)) = not(tempgenome(mutind(1:mutate)));
case {'Fixed'}
    mutind = find(tempgenome);
    mutind1 = find(tempgenome==0);
	nrand = randperm(length(mutind));
	tempgenome(mutind(nrand(1:ceil(mutate/2)))) = 0;
	nrand = randperm(length(mutind1));
	tempgenome(mutind1(nrand(1:ceil(mutate/2)))) = 1;
case 'Max Limit'
    mutind = randperm(length(tempgenome));
	tempgenome(mutind(1:mutate)) = not(tempgenome(mutind(1:mutate)));
    mutind = find(tempgenome);   
    if length(mutind) > DescNum
       nrand = randperm(length(mutind));
       tempgenome(mutind(nrand(1:length(mutind)-DescNum))) = 0;
    end;   
end;       

