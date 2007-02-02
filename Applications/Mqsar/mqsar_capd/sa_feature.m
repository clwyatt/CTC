function [currgenome, currfitness, bestgenome, bestfitness, newgenome, Tcurrent,tcount] = sa_feature(currgenome, currfitness, bestgenome, bestfitness, tempfitness, tempgenome, ...
                                             Tcurrent, Tstep, cycle, tcount, mutate, varargin);		

%  simulated annealing based feature reduction
% 
%	Input and output arguments ([]'s are optional): 
%	data	(matrix)	training data matrix
%	bins	(vector)	training sample's categories
%	bioact	(vector)	biological activities of compounds
%	varnames(vector)	list of variable names
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
%	varsel	(vector)	list of selected variables by stepwise
%	stats	(struct)	the statistical result
%   outclass(vector)    the calculated bins for the data
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
%	start		*(string)	Specifies the SA start temperature by following value
%	end			*(string)	Specifies the SA end temperature by following value
%	Tstep		*(string)	Specifies the temperature drop rate by following value
%	cycle		*(string)	Specifies the cycle number for each temperature step by following value
%	mutate		*(string)	Specifies the mutate number between generations by following value
%	Fix_Desc_Num			*(string)	Specifies the SA process using fix length descriptors by following value 
%	test		*(string)	Specifies the SA process driging by test set, the following arguments
%								are test data set and test bins information

%
% Ersin Bayram, ebayram@wfubmc.edu

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Ersin commented out state reset. Consequtive iterations might have the
% same state. No need to reseed every time. I moved this to pentad.m 
% rand('state',sum(100*clock));	%re-seed random number generator
warning off;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% default values
tbins = [];

%% Check arguments
i=1;
while i<=size(varargin,2), 
	argok = 1;
	if ischar(varargin{i}), 
		switch varargin{i}, 
			% unambiguous values
			case {'DescLimit'},	
                i = i + 1; 
                DescLimit = varargin{i};
                i = i + 1;
                DescNum = varargin{i};
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['(sasom) Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end

% fitness-based simulated annealing
% Start the cycles of Simulated Annealing

tcount = tcount + 1;       

if tempfitness > bestfitness
    bestfitness = tempfitness;
    currfitness = tempfitness;
	bestgenome = tempgenome;
	currgenome = tempgenome;
    tcount = 0;
    Tcurrent = Tcurrent * Tstep;
else % decide to include or discard mutation(s) in question
     bestfitness = bestfitness;
     bestgenome = bestgenome;
     currgenome = currgenome;
     currfitness = currfitness;
    if (tempfitness >= currfitness) & (tcount <= cycle)
		currgenome = tempgenome;
		currfitness = tempfitness;
    elseif (tempfitness < currfitness) & (tcount <= cycle)        
        if rand(1) < exp(-(tempfitness-bestfitness)/Tcurrent)
			currgenome = tempgenome;
            currfitness = tempfitness;
		end
    else 
        currfitness = bestfitness;
		currgenome = bestgenome;
        Tcurrent = Tcurrent * Tstep;
        tcount = 0;
    end;
end;

% create genome mutation mask and implement mutations
switch DescLimit
case {'None','Fixed Initial'}
    mutind = randperm(length(currgenome));
	newgenome = tempgenome;
	newgenome(mutind(1:mutate)) = not(newgenome(mutind(1:mutate)));
case {'Fixed'}
    newgenome = tempgenome;
    mutind = find(newgenome);
    mutind1 = find(newgenome==0);
	nrand = randperm(length(mutind));
	newgenome(mutind(nrand(1:ceil(mutate/2)))) = 0;
	nrand = randperm(length(mutind1));
	newgenome(mutind1(nrand(1:ceil(mutate/2)))) = 1;
case 'Max Limit'
    mutind = randperm(length(currgenome));
	newgenome = tempgenome;
	newgenome(mutind(1:mutate)) = not(newgenome(mutind(1:mutate)));
    mutind = find(newgenome);
    if length(mutind) > DescNum
       nrand = randperm(length(mutind));
       newgenome(mutind(nrand(1:length(mutind)-DescNum))) = 0;
    end;   
end;       
