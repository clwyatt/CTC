function [currgenome, cgFitness, cgAge, newgenome, genCounter,tcount] = ...
   ga_feature(oldgenome, ogFitness, ogAge, tmpgenome, tgFitness,tcount, genCounter, varargin);
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
%	fix			*(string)	Specifies the GA process using fix length descriptors by following value   
%	test		*(string)	Specifies the GA process driging by test set, the following arguments
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
			case {'DescLimit'}, 
                i=i+1; DescLimit = varargin{i};
                i = i + 1; DescNum = varargin{i}; 
            case {'MoveTarget'}, i=i+1; MoveTarget = varargin{i};
            case {'Cutoff'}, i = i+1; Cutoffs = varargin{i};
            case {'Gene Life'}, i=i+1; Cycle = varargin{i};   
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
genCounter  = genCounter + 1;
tcount = tcount + 1;
CrossPoints  = MaxCrossOver - ceil((MaxCrossOver-MinCrossOver)/Generation*genCounter);
MutatePoints = MaxMutate - ceil((MaxMutate-MinMutate)/Generation*genCounter);
currgenome = zeros(size(oldgenome));
newgenome = currgenome;

cgAge = zeros(size(ogAge));
cgFitness = zeros(size(ogFitness));
tgAge = ones(size(ogAge));

% Analyze the results, and perform cross over and mutation operations    
% If it is time to collect all subsets in one pool
if (mod(genCounter,PoolFreq+PoolDur) > PoolFreq) | (mod(genCounter,PoolFreq+PoolDur) == 0)     
   
   % Convert the fitness matrices of newgenomes and oldgenomes into vectors 
   ogFitList =  reshape(ogFitness,SubsetPopulation*Subsets,1);
   tgFitList =  reshape(tgFitness,SubsetPopulation*Subsets,1);
   % Combine the fitnesses into one vector to find the elites
   fitList = [ogFitList; tgFitList];
   [fitList, Ind] = sort(fitList);
   
   % Get the indices of the elite genomes
   % eliteness is purely based on accuracy 
   NumElites = round(SubsetPopulation*Subsets/100*Elites);
   % Map the elite indices back to the old and new genome indices 
   
   eliteInd = Ind(end:-1:end-NumElites+1);
   %ogEliteInd = eliteInd(eliteInd <= SubsetPopulation*Subsets);
   %tgEliteInd = eliteInd(eliteInd > SubsetPopulation*Subsets) - SubsetPopulation*Subsets;
   
   % Find the indices of the genomes in the original matrix structure
   newInd = randperm(SubsetPopulation*Subsets);
   ogEliteInd = [];
   tgEliteInd = [];
   %Add the first elite model to the pool and go down the list, check the
   %pool and discard the identical ones from entering the elites' pool.
   if Ind(end) <= SubsetPopulation*Subsets
       currgenome(:,newInd(1)) =  oldgenome(:,Ind(end));
       eliteGroup = oldgenome(:,Ind(end));
       cgAge(newInd(1)) = ogAge(Ind(end))+1;
       cgFitness(newInd(1)) = ogFitness(Ind(end));
       newInd(1) = [];
       ogEliteInd = [ogEliteInd Ind(end)];
   else
       currgenome(:,newInd(1)) =  tmpgenome(:,Ind(end)- SubsetPopulation*Subsets);
       eliteGroup = tmpgenome(:,Ind(end)-SubsetPopulation*Subsets);
       cgAge(newInd(1)) = 2;
       cgFitness(newInd(1)) = tgFitness(Ind(end)-SubsetPopulation*Subsets);
       newInd(1) = [];
       tgEliteInd = [tgEliteInd Ind(end)-SubsetPopulation*Subsets];
   end;
   % if none of the previous elites are identical to the current
   % candidate then put it into the elite's pool else move to the
   % next most fit model
   egroup = 1;
   indPointer = 1;
   
   while egroup < NumElites
       if Ind(end - indPointer) <= SubsetPopulation*Subsets
           newElite = oldgenome(:,Ind(end - indPointer));   
           descCorrelate = corrcoef([newElite eliteGroup]);     
           if isempty(find(descCorrelate(1,2:end) ==1)) & (sum(newElite) > 0)
               currgenome(:,newInd(1)) =  newElite;
               eliteGroup = [eliteGroup newElite];
               cgAge(newInd(1)) = ogAge(Ind(end - indPointer))+1;
               cgFitness(newInd(1)) = ogFitness(Ind(end-indPointer));
               newInd(1) = []; 
               ogEliteInd = [ogEliteInd Ind(end-indPointer)];
               egroup = egroup +1;
           end;
       else
           newElite = tmpgenome(:,Ind(end-indPointer)-SubsetPopulation*Subsets);   
           descCorrelate = corrcoef([newElite eliteGroup]); 
           if isempty(find(descCorrelate(1,2:end) ==1)) & (sum(newElite) > 0)
               currgenome(:,newInd(1)) = newElite ;
               eliteGroup = [eliteGroup newElite];
               cgAge(newInd(1)) = 2;
               cgFitness(newInd(1)) = tgFitness(Ind(end -indPointer)-SubsetPopulation*Subsets);
               newInd(1) = []; 
               tgEliteInd = [tgEliteInd Ind(end -indPointer)-SubsetPopulation*Subsets];
               egroup = egroup + 1;
           end;
       end;
       indPointer = indPointer + 1;
   end;    
   
   % Calculate the relative Fitness of each  
   DescSize= size(oldgenome,1);
   % Calculate the relative age based fitness of each genome
   ogAgeNorm = reshape(ogAge/Cycle,SubsetPopulation*Subsets,1);
   tgAgeNorm = reshape(tgAge/Cycle,SubsetPopulation*Subsets,1);
   % Calculate the relative comlexity based fitness of each genome
   ogComplexity = reshape(squeeze(sum(oldgenome,1)/DescSize),SubsetPopulation*Subsets,1);
   tgComplexity = reshape(squeeze(sum(tmpgenome,1)/DescSize),SubsetPopulation*Subsets,1);
   % Calculate the relative overall fitness of each genome
   % Preference is towards fitter, younger and simpler genomes 
   ogTotalFitness = ogFitList - ogAgeNorm - 0.5*ogComplexity;   
   tgTotalFitness = tgFitList - tgAgeNorm - 0.5*tgComplexity;
   
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
       if (sum(Offspring1) > 0) & (sum(Offspring2) > 0)
           newgenome(:,newInd(1)) =  Offspring1;    
           newgenome(:,newInd(2)) =  Offspring2; 
           newInd(1:2) = [];
           Childs = Childs + 2;
       end;    
   end;      

else       
   % Combine the fitnesses into one matrix to find the elites
   fitList = [ogFitness; tgFitness];
   [fitList, Ind] = sort(fitList,1); 
   % Get the indices of the elite genomes
   NumElites = round(SubsetPopulation/100*Elites);

   % Map the elite indices back to the old and new genome indices 
   
   ogEIndRow = [];
   ogEIndCol = [];
   tgEIndRow = [];
   tgEIndCol = [];
   
   % Place the elites into the survivors' pool. Make sure that they are
   % placed into the same subset that they came from. The order within the
   % subset is random
   for j=1:Subsets
       % Initialize the elites' matrix
       eliteGroup = zeros(size(oldgenome,1),NumElites);
       subInd(j).Pop = randperm(SubsetPopulation);
       %Add the first elite model to the pool and go down the list, check the
       %pool and discard the identical ones from entering the elites' pool.
       if Ind(end,j) <= SubsetPopulation
           currgenome(:,subInd(j).Pop(1),j) =  oldgenome(:,Ind(end,j),j);
           eliteGroup(:,1) = oldgenome(:,Ind(end,j),j);
           cgAge(subInd(j).Pop(1),j) = ogAge(Ind(end,j),j)+1;
           cgFitness(subInd(j).Pop(1),j) = ogFitness(Ind(end,j),j);
           subInd(j).Pop(1) = [];
           ogEIndRow = [ogEIndRow Ind(end,j)];
           ogEIndCol = [ogEIndCol j];
       else
           currgenome(:,subInd(j).Pop(1),j) =  tmpgenome(:,Ind(end,j)-SubsetPopulation,j);
           eliteGroup(:,1) = tmpgenome(:,Ind(end,j)-SubsetPopulation,j);
           cgAge(subInd(j).Pop(1),j) = 2;
           cgFitness(subInd(j).Pop(1),j) = tgFitness(Ind(end,j)-SubsetPopulation,j);
           subInd(j).Pop(1) = [];
           tgEIndRow = [tgEIndRow Ind(end,j)-SubsetPopulation];
           tgEIndCol = [tgEIndCol j];
       end;
   
       egroup = 1;
       indPointer = 1;
       while egroup < NumElites
           if Ind(end - indPointer,j) <= SubsetPopulation
               newElite = oldgenome(:,Ind(end - indPointer,j),j);   
               descCorrelate = corrcoef([newElite eliteGroup(:,1:egroup)]);     
               if isempty(find(descCorrelate(1,2:end) ==1)) & (sum(newElite) > 0)
                   egroup = egroup +1;
                   currgenome(:,subInd(j).Pop(1),j) =  newElite;
                   eliteGroup(:,egroup) = newElite;
                   cgAge(subInd(j).Pop(1),j) = ogAge(Ind(end - indPointer,j),j)+1;
                   cgFitness(subInd(j).Pop(1),j) = ogFitness(Ind(end-indPointer,j),j);
                   subInd(j).Pop(1) = []; 
                   ogEIndRow = [ogEIndRow Ind(end-indPointer,j)];
                   ogEIndCol = [ogEIndCol j];
               end;
           else
               newElite = tmpgenome(:,Ind(end-indPointer,j)-SubsetPopulation,j);   
               descCorrelate = corrcoef([newElite eliteGroup(:,1:egroup)]); 
               if isempty(find(descCorrelate(1,2:end) ==1)) & (sum(newElite) > 0)
                   egroup = egroup + 1;
                   currgenome(:,subInd(j).Pop(1),j) = newElite ;
                   eliteGroup(:,egroup) = newElite;
                   cgAge(subInd(j).Pop(1),j) = 2;
                   cgFitness(subInd(j).Pop(1),j) = tgFitness(Ind(end -indPointer,j)-SubsetPopulation,j);
                   subInd(j).Pop(1) = [];                 
                   tgEIndRow = [tgEIndRow Ind(end-indPointer,j)-SubsetPopulation];
                   tgEIndCol = [tgEIndCol j];     
               end;
           end;
           indPointer = indPointer + 1;
       end; % end while   
   end; % for   
     
   % Calculate the relative Fitness of each  
   DescSize= size(oldgenome,1);
   % Calculate the relative age based fitness of each genome
   ogAgeNorm = ogAge/Cycle;
   tgAgeNorm = tgAge/Cycle;
   % Calculate the relative comlexity based fitness of each genome
   ogComplexity = squeeze(sum(oldgenome,1)/DescSize);
   tgComplexity = squeeze(sum(tmpgenome,1)/DescSize);
   if size(ogComplexity,1)==1
       ogComplexity = ogComplexity';
       tgComplexity = tgComplexity';
   end;    
   % Calculate the relative overall fitness of each genome
   % Preference is towards fitter, younger and simpler genomes 
   ogTotalFitness = ogFitness - ogAgeNorm - 0.5*ogComplexity;   
   tgTotalFitness = tgFitness - tgAgeNorm - 0.5*tgComplexity;
   
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
   % Generate the rest of the survivor pool and pass along their age and
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
           if (sum(Offspring1) > 0) & (sum(Offspring2) > 0)
               newgenome(:,newInd(1),j) =  Offspring1;
               newgenome(:,newInd(2),j) =  Offspring2; 
               newInd(1:2) = [];
               Childs = Childs + 2;
           end;
       end; %while     
   end; % for j=1:Subsets
end; % if pool

switch DescLimit
case {'Fixed'}    
    SumG = squeeze(sum(newgenome,1));
    [ind1,ind2] = find(SumG > DescNum);
    
    for i = 1:length(ind1)
        if (ind1 == ones(size(ind1))) 
            temp = newgenome(:,ind1(i));
        elseif (ind2 == ones(size(ind2))) 
            temp = newgenome(:,ind1(i));
        else    
            temp = newgenome(:,ind1(i),ind2(i));
        end;    
        Fdiff = sum(temp) - DescNum;
        Aleleind  = find(temp ==1);
        Randind = randperm(length(Aleleind));
        temp(Aleleind(Randind(1:Fdiff))) = 0;
        if (ind1 == ones(size(ind1))) 
            newgenome(:,ind2(i)) = temp;
        elseif (ind2 == ones(size(ind2))) 
            newgenome(:,ind1(i)) = temp;
        else    
            newgenome(:,ind1(i),ind2(i)) = temp;
        end;    
     end;
case {'Max Limit'}
    
end;   
        
        
