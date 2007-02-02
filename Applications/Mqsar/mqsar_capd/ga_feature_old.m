function [currgenome, cgFitness, cgAge, newgenome, genCounter,tcount] = ...
   ga_feature_old(oldgenome, ogFitness, ogAge, tmpgenome, tgFitness,tcount, genCounter, varargin);
% Genetic Algorithm based feature selection
%
%	Input arguments 
%	data	(matrix)	training data matrix
%	bins	(vector)	training sample's categories
%	bioact	(vector)	biological activities of compounds
%	varnames(vector)	list of variable names
%	[argID,		(string)  See below. The values which are unambiguous can 
%	value]		(varies)  be given without the preceeding argID.
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
%   Descriptors *(string)   Number of decriptors included in the initial genome models
%
%	SubsetPopulation 
%               *(string)	Specifies the population in each subset 
%	Subsets		*(string)	Specifies number of subsets 
%   Generation  *(string)	Specifies max. number of generations or iterations for GA 
%   PoolFreq    *(string)	Specifies the number of generations between each pooling of 
%                           subsets together      
%   PoolDur     *(string)	Specifies the number of generations during which all the population will 
%                           be run together      
%   MaxMutate   *(string)	Specifies the max number of mutation points 
%   MinMutate   *(string)	Specifies the min number of mutation points 
%   MaxCrossOver
%               *(string)	Specifies the max number of crossover points 
%   MinCrossOver
%               *(string)	Specifies the min number of crossover points 
%    
%   Elites      *(string)	Specifies the number of parent to be directly included in the next set
%   Pmutatue    *(string)   Specifies the probability of mutations
%   Pcross      *(string)   Specifies the probability of crossovers.   
%
%	cycle		*(string)	Specifies the cycle number without improvement to stop iteration 
%	fix			*(string)	Specifies the SA process using fix length descriptors by following value   
%	test		*(string)	Specifies the SA process driging by test set, the following arguments
%

% Ersin Bayram
% June, 2004 ebayram@wfubmc.edu
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
warning off;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% default values

Descriptors = 20;
SubsetPopulation = 20;
Generation = 200;
Subsets = 20;
PoolFreq = 20;
PoolDur = 5;
MaxMutate = 10;
MinMutate = 0;
MaxCrossOver = 10;
MinCrossOver = 0;
Elites = 20;
Pmutation = 0.1;
Pcross = 0.7;
MoveTarget = 0;
EliteDuration = 10;

fix = [];
tbins = [];

%% Check arguments
i=1;
while i<=size(varargin,2), 
	argok = 1;
	if ischar(varargin{i}), 
		switch varargin{i}, 
			% unambiguous values
            case {'Descriptors'}, i = i + 1; Descriptors = varargin{i};     
            case {'SubsetPopulation'}, i = i + 1; SubsetPopulation = varargin{i};   
			case {'Generation'}, i = i + 1; Generation = varargin{i};
            case {'Subsets'}, i = i + 1; Subsets = varargin{i};     
			case {'PoolFreq'}, i = i + 1; PoolFreq = varargin{i};
            case {'PoolDur'}, i=i+1; PoolDur = varargin{i};   
            case {'MaxMutate'}, i=i+1; MaxMutate = varargin{i};   
            case {'MinMutate'}, i=i+1; MinMutate = varargin{i};
            case {'MaxCrossOver'}, i=i+1; MaxCrossOver = varargin{i};
            case {'MinCrossOver'}, i=i+1; MinCrossOver = varargin{i};   
            case {'Elites'}, i=i+1; Elites = varargin{i};        
			case {'Pmutation'}, i=i+1; Pmutation = varargin{i};
            case {'Pcross'}, i=i+1; Pcross = varargin{i};    
			case {'fix'}, i=i+1; fix = varargin{i}; 
            case {'MoveTarget'}, i=i+1; MoveTarget = varargin{i};
            case {'Cutoff'}, i = i+1; Cutoffs = varargin{i};
            case {'cycle'}, i=i+1; Cycle = varargin{i};   
            case {'EliteDuration'}, i=i+1; EliteDuration = varargin{i};       
			otherwise argok=0; 
		end
	end
	if ~argok,
		disp(['gasom - Ignoring invalid argument: #' num2str(i)]); 
	end
	i = i+1; 
end;

% Start the cycles of Genetic Algorithm
genCounter  = genCounter + 1
CrossPoints  = MaxCrossOver - ceil((MaxCrossOver-MinCrossOver)/Generation*genCounter);
MutatePoints = MaxMutate - ceil((MaxMutate-MinMutate)/Generation*genCounter);
Occupied = zeros(size(ogAge));
currgenome = zeros(size(oldgenome));
newgenome = currgenome;

cgAge = zeros(size(ogAge));
cgFitness = zeros(size(ogFitness));
tgAge = ones(size(ogAge));

% Analyze the results, and perform cross over and mutation operations    
% If it is time to collect all subsets in one pool
if (floor(genCounter/PoolFreq) >= 1) & (mod(genCounter,PoolFreq) <= PoolDur)      
   
   % Convert the fitness matrices of newgenomes and oldgenomes into vectors 
   ogFitList =  reshape(ogFitness,SubsetPopulation*Subsets,1);
   tgFitList =  reshape(tgFitness,SubsetPopulation*Subsets,1);
   % Combine the fitnesses into one vector to find the elites
   fitList = [ogFitList; tgFitList];
   [fitList, Ind] = sort(fitList);
   
   % Get the indices of the elite genomes
   NumElites = round(SubsetPopulation*Subsets/100*Elites);
   % Map the elite indices back to the old and new genome indices 
   eliteInd = Ind(end:-1:end-NumElites+1);
   ogEliteInd = eliteInd(eliteInd <= length(ogFitList));
   tgEliteInd = eliteInd(eliteInd > length(ogFitList)) - length(ogFitList);
   
   % Find the indices of the genomes in the original matrix structure
   newInd = randperm(SubsetPopulation*Subsets);
   if ~isempty(ogEliteInd)
       currgenome(:,newInd(1:length(ogEliteInd))) =  oldgenome(:,ogEliteInd);
       cgAge(newInd(1:length(ogEliteInd))) = ogAge(ogEliteInd)+1;
       cgFitness(newInd(1:length(ogEliteInd))) = ogFitness(ogEliteInd);
       newInd(1:length(ogEliteInd)) = [];
   end;
   if ~isempty(tgEliteInd)
   currgenome(:,newInd(1:length(tgEliteInd))) =  tmpgenome(:,tgEliteInd);
   cgAge(newInd(1:length(tgEliteInd))) = 2;
   cgFitness(newInd(1:length(tgEliteInd))) = tgFitness(tgEliteInd);
   newInd(1:length(tgEliteInd)) = [];
   end;
   
   % Calculate the relative Fitness of each  
   descNum= size(oldgenome,1);
   % Calculate the relative age based fitness of each genome
   ogAgeNorm = reshape(ogAge/Cycle,SubsetPopulation*Subsets,1);
   tgAgeNorm = reshape(tgAge/Cycle,SubsetPopulation*Subsets,1);
   % Calculate the relative comlexity based fitness of each genome
   ogComplexity = reshape(squeeze(sum(oldgenome,1)/descNum),SubsetPopulation*Subsets,1);
   tgComplexity = reshape(squeeze(sum(tmpgenome,1)/descNum),SubsetPopulation*Subsets,1);
   % Calculate the relative overall fitness of each genome
   % Preference is towards fitter, younger and simpler genomes 
   ogTotalFitness = ogFitList - ogAgeNorm - ogComplexity;   
   tgTotalFitness = tgFitList - tgAgeNorm - tgComplexity;
   
   % Elites are already included in the surviving generation. Discard them
   % for the rest of the survivor search process. 
   if ~isempty(ogEliteInd)
       ogTotalFitness(ogEliteInd) = min(min(ogTotalFitness),min(tgTotalFitness))-1;
   end 
   if ~isempty(tgEliteInd)
       tgTotalFitness(tgEliteInd) = min(min(ogTotalFitness),min(tgTotalFitness))-1;
   end  
   
   TotalFitList = [ogTotalFitness; tgTotalFitness];
   
   %ogEliteSubsetInd = ceil(ogEliteInd/SubsetPopulation);
   %ogEliteSubsetPopInd = rem(ogEliteInd,SubsetPopulation);
   %ogEliteSubsetPopInd(ogEliteSubsetPopInd == 0) = SubsetPopulation;
   
   % Find the remaining genomes that will survive to compete with the next
   % generation genomes. Survival is based on the relative total fitness,
   % which is a combined metric of age, performance, and complexity
   restSize = SubsetPopulation*Subsets - NumElites; 
   [restList, Ind] = sort(TotalFitList);
   % Map the elite indices back to the old and new genome indices 
   restInd = Ind(end:-1:end-restSize+1);
   ogRestInd = restInd(restInd <= length(ogTotalFitness));
   tgRestInd = restInd(restInd > length(ogTotalFitness)) - length(ogTotalFitness); 
   
   if ~isempty(ogRestInd)
       currgenome(:,newInd(1:length(ogRestInd))) =  oldgenome(:,ogRestInd);
       cgAge(newInd(1:length(ogRestInd))) = ogAge(ogRestInd)+1;
       cgFitness(newInd(1:length(ogRestInd))) = ogFitness(ogRestInd);
       newInd(1:length(ogRestInd)) = [];
   end;
   if ~isempty(tgRestInd)
   currgenome(:,newInd(1:length(tgRestInd))) =  tmpgenome(:,tgRestInd);
   cgAge(newInd(1:length(tgRestInd))) = 2;
   cgFitness(newInd(1:length(tgRestInd))) = tgFitness(tgRestInd);
   newInd(1:length(tgRestInd)) = [];
   end;
   
   % Now create the new set of offsprings for the next iteration
   newInd = randperm(SubsetPopulation*Subsets);
   Childs = 0;
   while Childs < SubsetPopulation*Subsets         
       ParentInd = gaselect(fitList, 2, 'roulette');         
       % Select the parents for offspring generation 
       if ParentInd <= length(ogFitList)
           Parent1 = oldgenome(:,ParentInd(1));
           Parent2 = oldgenome(:,ParentInd(2));
       elseif ParentInd > length(ogFitList) 
           Parent1 = tmpgenome(:,ParentInd(1)- length(ogFitList));
           Parent2 = tmpgenome(:,ParentInd(2)- length(ogFitList));  
       elseif (ParentInd <= length(ogFitList)) == [1 0]
           Parent1 = oldgenome(:,ParentInd(1));
           Parent2 = tmpgenome(:,ParentInd(2)- length(ogFitList));
       else
           Parent1 = tmpgenome(:,ParentInd(1)- length(ogFitList));
           Parent2 = oldgenome(:,ParentInd(2));
       end;    
       % Perform crossover 
       [Offspring1, Offspring2, SameFlag] = gacrossover(Parent1, Parent2, Pcross,'point', CrossPoints);
       % Apply mutation to the offsprings
       [Offspring1, SameFlag] = gamutate(Offspring1, Pmutation, MutatePoints);
       [Offspring2, SameFlag] = gamutate(Offspring2, Pmutation, MutatePoints);
       % Pass the offsprings into the next generation matrix
       newgenome(:,newInd(1)) =  Offspring1;
       newgenome(:,newInd(2)) =  Offspring2; 
       newInd(1:2) = [];
       Childs = Childs + 2;
   end;      

else       
   % Combine the fitnesses into one matrix to find the elites
   fitList = [ogFitness; tgFitness];
   [fitList, Ind] = sort(fitList,1);
   % Get the indices of the elite genomes
   NumElites = round(SubsetPopulation/100*Elites);
   % Map the elite indices back to the old and new genome indices 
   eliteInd = Ind(end:-1:end-NumElites+1,:);
   [ogEIndRow,ogEIndCol] = find(eliteInd <= SubsetPopulation);
   ogEIndRow = eliteInd(eliteInd <= SubsetPopulation);
   [tgEIndRow,tgEIndCol] = find(eliteInd > SubsetPopulation);
   tgEIndRow = eliteInd(eliteInd > SubsetPopulation)- SubsetPopulation;
 
   for j=1:Subsets
      subInd(j).Pop = randperm(SubsetPopulation);
   end;   
   % Place the elites into the survivors' pool. Make sure that they are
   % placed into the same subset that they came from. The order within the
   % subset is random
   for ii =1:length(ogEIndRow)
       currgenome(:,subInd(ogEIndCol(ii)).Pop(1),ogEIndCol(ii)) =  oldgenome(:,ogEIndRow(ii),ogEIndCol(ii));
       cgAge(subInd(ogEIndCol(ii)).Pop(1),ogEIndCol(ii)) = ogAge(ogEIndRow(ii),ogEIndCol(ii))+1;
       cgFitness(subInd(ogEIndCol(ii)).Pop(1),ogEIndCol(ii)) = ogFitness(ogEIndRow(ii),ogEIndCol(ii));
       subInd(ogEIndCol(ii)).Pop(1) = [];
   end;
   for ii =1:length(tgEIndRow)
       currgenome(:,tgEIndRow(ii),tgEIndCol(ii)) =  tmpgenome(:,tgEIndRow(ii),tgEIndCol(ii));
       cgAge(subInd(tgEIndCol(ii)).Pop(1),tgEIndCol(ii)) = 2;
       cgFitness(subInd(tgEIndCol(ii)).Pop(1),tgEIndCol(ii)) = tgFitness(tgEIndRow(ii),tgEIndCol(ii));
       subInd(tgEIndCol(ii)).Pop(1) = [];
   end;
   
   % Calculate the relative Fitness of each  
   descNum= size(oldgenome,1);
   % Calculate the relative age based fitness of each genome
   ogAgeNorm = ogAge/Cycle;
   tgAgeNorm = tgAge/Cycle;
   % Calculate the relative comlexity based fitness of each genome
   ogComplexity = squeeze(sum(oldgenome,1)/descNum);
   tgComplexity = squeeze(sum(tmpgenome,1)/descNum);
   % Calculate the relative overall fitness of each genome
   % Preference is towards fitter, younger and simpler genomes 
   ogTotalFitness = ogFitness - ogAgeNorm - ogComplexity;   
   tgTotalFitness = tgFitness - tgAgeNorm - tgComplexity;
   
   % Elites are already included in the surviving generation. Discard them
   % for the rest of the survivor search process. 
   for ii =1:length(ogEIndRow)
       ogTotalFitness(ogEIndRow(ii),ogEIndCol(ii)) = min(min(min(ogTotalFitness)),min(min(tgTotalFitness)))-1;
   end 
   for ii =1:length(tgEIndRow)
       tgTotalFitness(tgEIndRow(ii),tgEIndCol(ii)) = min(min(min(ogTotalFitness)),min(min(tgTotalFitness)))-1;
   end 
   TotalFitList = [ogTotalFitness; tgTotalFitness];
   
   % Find the remaining genomes that will survive to compete with the next
   % generation genomes. Survival is based on the relative total fitness,
   % which is a combined metric of age, performance, and complexity
   restSize = SubsetPopulation - NumElites; 
   [restList, Ind] = sort(TotalFitList,1);
   % Map the elite indices back to the old and new genome indices 
   restInd = Ind(end:-1:end-restSize+1,:);
   [ogRIndRow,ogRIndCol] = find(restInd <= SubsetPopulation);
   ogRIndRow = restInd(restInd <= SubsetPopulation);
   [tgRIndRow,tgRIndCol] = find(restInd > SubsetPopulation);
   tgRIndRow = restInd(restInd > SubsetPopulation)- SubsetPopulation;
   % Generate the rest of the survivior pool and pass along their age and
   % fitness values
   for ii =1:length(ogRIndRow)
       currgenome(:,subInd(ogRIndCol(ii)).Pop(1),ogRIndCol(ii)) =  oldgenome(:,ogRIndRow(ii),ogRIndCol(ii));
       cgAge(subInd(ogRIndCol(ii)).Pop(1),ogRIndCol(ii)) = ogAge(ogRIndRow(ii),ogRIndCol(ii))+1;
       cgFitness(subInd(ogRIndCol(ii)).Pop(1),ogRIndCol(ii)) = ogFitness(ogRIndRow(ii),ogRIndCol(ii));
       subInd(ogRIndCol(ii)).Pop(1) = [];
   end;
   for ii =1:length(tgRIndRow)
       currgenome(:,tgRIndRow(ii),tgRIndCol(ii)) =  tmpgenome(:,tgRIndRow(ii),tgRIndCol(ii));
       cgAge(subInd(tgRIndCol(ii)).Pop(1),tgRIndCol(ii)) = 2;
       cgFitness(subInd(tgRIndCol(ii)).Pop(1),tgRIndCol(ii)) = tgFitness(tgRIndRow(ii),tgRIndCol(ii));
       subInd(tgRIndCol(ii)).Pop(1) = [];
   end;
   
   % Now create the new set of offsprings for the next iteration
   for j =1:Subsets
       newInd = randperm(SubsetPopulation);
       Childs = 0;
       while Childs < SubsetPopulation         
           ParentInd = gaselect(fitList(:,j), 2, 'roulette');         
           % Select the parents for offspring generation 
           if ParentInd <= SubsetPopulation
               Parent1 = oldgenome(:,ParentInd(1),j);
               Parent2 = oldgenome(:,ParentInd(2),j);
           elseif ParentInd > SubsetPopulation 
               Parent1 = tmpgenome(:,ParentInd(1)- SubsetPopulation,j);
               Parent2 = tmpgenome(:,ParentInd(2)- SubsetPopulation,j);  
           elseif (ParentInd <= SubsetPopulation) == [1 0]
               Parent1 = oldgenome(:,ParentInd(1),j);
               Parent2 = tmpgenome(:,ParentInd(2)- SubsetPopulation,j);
           else
               Parent1 = tmpgenome(:,ParentInd(1)- SubsetPopulation,j);
               Parent2 = oldgenome(:,ParentInd(2),j);
           end;    
           % Perform crossover 
           [Offspring1, Offspring2, SameFlag] = gacrossover(Parent1, Parent2, Pcross,'point', CrossPoints);
           % Apply mutation to the offsprings
           [Offspring1, SameFlag] = gamutate(Offspring1, Pmutation, MutatePoints);
           [Offspring2, SameFlag] = gamutate(Offspring2, Pmutation, MutatePoints);
           % Pass the offsprings into the next generation matrix
           newgenome(:,newInd(1),j) =  Offspring1;
           newgenome(:,newInd(2),j) =  Offspring2; 
           newInd(1:2) = [];
           Childs = Childs + 2;
       end; %while     
   end; % for j=1:Subsets
end; % if pool

genCounter = genCounter + 1;
tcount = tcount + 1;
