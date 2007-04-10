function [bestgenome,CrossMat] = qsarmodel(userdata,VarNames,CmpNames,Xmat,Ymat,Bins,Maskmat,nModel);

%Shuffle the bioact and bins if we are running a randomized experiment
%if userdata.ShuffleFlag
%   RandInd = randperm(length(Ymat)); 
%   Ymat = Ymat(RandInd);
%   Bins = Bins(RandInd);    
%end  


switch userdata.validation
case 'None'
    CrossMat = Maskmat(:, nModel);
case 'Leave-One-Out'      
    CrossMat = eye(size(Xmat,1));
case 'Leave-Some-Out'    
     if sum(sum(Maskmat))==0
      CrossMat = zeros(size(Xmat,1),userdata.nfold);
      i=1;
      while i <= userdata.nfold
          tempind = randperm(size(Xmat,1));
          CrossMat(tempind(1:round(userdata.percentout/100*size(Xmat,1))),i) = 1;
          trind = find(CrossMat(:,i));
          tmpX = Xmat;
          tmpX(trind, :) = [];
          if isempty(find(std(tmpX)==0))
              i=i+1;
          else
              CrossMat(:,i) = 0;
          end;    
      end;    
    else
      CrossMat = Maskmat;
    end;  
end; 

% choose initial genome (descriptor inclusion-mask) configuration
% 1. build the initial mask as uniformly distributed
switch userdata.varselect
case 'None' % Non
    genome = ones(size(Xmat,2),1);
case 'GA'  % GA has more than one solutions so need a multidimensional genome mask   
    genome = zeros(size(Xmat,2),userdata.SubsetPopulation,userdata.Subsets);
    for i = 1:userdata.Subsets
        for j=1:userdata.SubsetPopulation
            %if not(userdata.chkFixInitial)
            switch userdata.DescLimit
            case 'None'
                genome(:,j,i) = round(rand(size(Xmat,2),1));
            case {'Fixed Initial','Fixed'}
                locs = randperm(size(Xmat,2));
                genome(locs(1:userdata.DescNum),j,i) = 1;
            case 'Max Limit'
                locs = randperm(size(Xmat,2));
                randstart = randperm(userdata.DescNum);
                randstart = randstart(1);
                genome(locs(1:randstart),j,i) = 1;
            end;    
        end;
    end;  
otherwise     
    genome = zeros(size(Xmat,2),1);
    switch userdata.DescLimit
    case 'None'
        genome = round(rand(size(Xmat,2),1));
    case {'Fixed Initial','Fixed'}
        locs = randperm(size(Xmat,2));
        genome(locs(1:userdata.DescNum)) = 1;
    case 'Max Limit'
        locs = randperm(size(Xmat,2));
        randstart = randperm(userdata.Descriptors);
        randstart = randstart(1);
        genome(locs(1:randstart)) = 1;
    end;       
end;    

% Initialize the method dependent variables. 
doneflag = 0;
bestgenome = ones(size(genome,1),1);
bestfitness = 0;
tcount = 0;
currgenome = zeros(size(genome));
initgenome = genome;
bestgenome_movtar = [];
genCounter = 1;

switch userdata.varselect
case 'GA' % GA    
    % GA related parameters
    if userdata.MoveTarget
       Results = [];
       Finalgenome = [];
    end; 
    currAge = ones(userdata.SubsetPopulation,userdata.Subsets);
    currfitness = zeros(userdata.SubsetPopulation,userdata.Subsets);
case 'SA' % GA  
    Tcurrent = userdata.StartT;
    currfitness = 0;
otherwise  
    currfitness = 0;
end;  

 % Modeling Step
% Use the selected descriptors to perform classification
tempgenome = initgenome;
[G,P,S] = size(tempgenome);
tempfitness = zeros(size(CrossMat,2),P,S);

while not(doneflag)          
    for cv = 1:size(CrossMat,2)  
        index = find(CrossMat(:, cv));
        % Training set information
        tmpX = Xmat;
        tmpX(index, :) = [];
        tmpY = Ymat;
        tmpY(index, :) = [];
        tmpBins = Bins;
        tmpBins(index) = [];
        tmpNames = CmpNames;
        tmpNames(index) = [];

        % test set information
        tmpXTest = Xmat(index, :);
        tmpBinsTest = Bins(index);
        tmpNamesTest = CmpNames(index);
        tmpYTest = Ymat(index, :);
        for ii=1:P
            for jj=1:S
                switch char(userdata.method)
                case 'CM'
                    [class, prd,Dcenter, stats] = cm_model(tmpX(:,find(tempgenome(:,ii,jj))),tmpBins,tmpXTest(:,find(tempgenome(:,ii,jj))), ...
                                                tmpBinsTest, userdata.ClassBins,userdata.Cluster);
                case 'FCM'
                    [class, prd,Member_matrix,Dcenter, stats] = fcm_model(tmpX(:,find(tempgenome(:,ii,jj))),tmpBins,tmpXTest(:,find(tempgenome(:,ii,jj))), ...
                                                tmpBinsTest, userdata.ClassBins,userdata.Cluster);
       
                case 'KNN'

                case 'MDS'

                case 'MLR'

                case 'NMLR'

                case 'NNET'
                    [TrPrdAct,TrPrdBin, TstPrdAct,TstPrdBin, stats] = nnet_model(tmpX(:,find(tempgenome(:,ii,jj))), tmpY, tmpBins, ...
                                        'Test',tmpXTest(:,find(tempgenome(:,ii,jj))),tmpYTest,tmpBinsTest, ...
                                        'Cutoff',userdata.cutoff,'Type',userdata.nntype,'Method',userdata.nnmethod,...
                                        'Hidden Layer',[userdata.hiddenstart userdata.hiddenstart2],...
                                        [userdata.hiddenstop userdata.hiddenstop2],...
                                        'Training',userdata.nntraining,'Function',userdata.nnfunction,...
                                        'Epoch',userdata.epoch,'Bin Sort',userdata.sortgoodtobad);                         
                case 'PCR'
                    dist = pairwise(tmpXTest);
                    for i=1:length(dist)
                        dist(i) = 1/dist(i);
                    end

                case 'PLS'    
                    [B, BestPrinCompn, E, Fitness, Expterm, ...
                     LogTemp, W, Q, P, MeanStd] = npls(tmpX(:,find(tempgenome(:,ii,jj))), tmpY, ...
                                            userdata.ComponNum, szVal, szSquare, szExp, szInverse, szLog, szNorm);

                    % Calculate the fitness of the built model on the test dataset of
                    % the training . For now use the Fitness(1) as I don't
                    % know what exactly each component is. 
                    if not(isempty(tmpXTest))
                       tempfitness(cv,ii,jj) = Fitness(1);
                    else
                       tempfitness(cv,ii,jj) = Fitness(1);
                    end;       
                case 'SOM' 
                    [prd, stats, class] = som_model(tmpX(:,find(tempgenome(:,ii,jj))), tmpBins, ...
                                         VarNames(find(tempgenome(:,ii,jj))), tmpXTest(:,find(tempgenome(:,ii,jj))), ...
                                         'Neighbor',userdata.neighbor,'Shape',userdata.shape,...
                                         'Map',userdata.sommap,'Type',userdata.somtype,'testbin', tmpBinsTest,...
                                         'Bin Sort',userdata.sortgoodtobad);                  
                case 'SVM'

                end   
                % Calculate the fitness of the built model 
                 % Calculate the fitness of the built model 
                if not(isempty(tmpXTest))
                    switch userdata.optimize
                    case 'Accuracy'  
                        tempfitness(cv,ii,jj) = stats.test_correct_rate;  
                    case 'False Pos'
                        tempfitness(cv,ii,jj) = 1 - stats.test_false_pos ;    
                    case 'False Neg'
                        tempfitness(cv,ii,jj) =  1 - stats.test_false_neg; 
                    case 'RMSE'    
                        tempfitness(cv,ii,jj) = stats.test_rmserror;
                    end;    
                else
                   switch userdata.optimize
                    case 'Accuracy'  
                        tempfitness(cv,ii,jj) = stats.correct_rate;  
                    case 'False Pos'
                        tempfitness(cv,ii,jj) = 1 - stats.false_pos;    
                    case 'False Neg'
                        tempfitness(cv,ii,jj) = 1 - stats.false_neg; 
                    case 'RMSE'    
                        tempfitness(cv,ii,jj) = stats.rmserror;
                    end; 
                end;  
                
                % Map the classifaction of the models with the initial class allocation. 
                % bin #3 in the initial classification might be bin #2 in the
                % model classification

            end; % for Subset Population  
        end;     % for Subset
    end; % for cv - THis is the cross validation based modeling loop
    % 
    % Sum the tempfitness across the cross validation set: training data or
    % left out compounds -if there is a test set
    tempfitness = squeeze(sum(tempfitness,1))/size(CrossMat,2);
    
    % Descriptor Selection Step
    switch userdata.varselect
    case 'None'  % non
        doneflag = 1;
    case 'GA'  % GA
        [currgenome, currfitness, currAge, ...
        tempgenome, genCounter,tcount] = ga_feature(currgenome, currfitness, currAge, ...
                                    tempgenome, tempfitness',tcount, genCounter, ...
                                    'SubsetPopulation', userdata.SubsetPopulation, ...
                                    'Generation', userdata.Generation, 'Subsets', userdata.Subsets, ...
                                    'PoolFreq', userdata.PoolFreq, 'PoolDur', userdata.PoolDur, ...
                                    'MaxMutate', userdata.MaxMutate, 'MinMutate', userdata.MinMutate, ... 
                                    'MaxCrossOver', userdata.MaxCrossOver, 'MinCrossOver', userdata.MinCrossOver, ...
                                    'Elites', userdata.Elites, 'Pmutation', userdata.Pmutation, ...
                                    'Pcross', userdata.Pcross, 'MoveTarget', userdata.MoveTarget, 'Gene Life', userdata.GeneLife, ...
                                    'DescLimit', userdata.DescLimit,userdata.Descriptors,...
                                    'Cutoff', userdata.cutoff, 'EliteDuration', userdata.EliteDuration);

        if  genCounter > userdata.Generation
            doneflag = 1;
        end;                            
    case 'Hill Climbing'  % Hill Climbing   
        [tempgenome, bestgenome, bestfitness,genCounter,tcount] = hill_feature(bestgenome, bestfitness, tempfitness, tempgenome, ...
                          userdata.Generation, genCounter,tcount, userdata.Mutate, 'DescLimit', userdata.DescLimit,userdata.Descriptors);
        if (tcount >= userdata.Hcycle) | (genCounter >= userdata.HGeneration)
              doneflag = 1;
        end;    
    case 'SA'  % SA    
        [currgenome, currfitness, bestgenome, bestfitness, tempgenome, Tcurrent,tcount] = sa_feature(currgenome, currfitness, bestgenome, bestfitness, tempfitness, tempgenome, ...
                         Tcurrent, userdata.Droprate, userdata.Cycle, tcount, userdata.Mutate, 'DescLimit', userdata.DescLimit,userdata.Descriptors); 
        if Tcurrent <= userdata.EndT
              doneflag = 1;
        end;    
    case 'Stepwise'  % Stepwise
        disp('Stepwise KNN');
    end
end; % while not(doneflag)

switch userdata.varselect
case 'GA'  % GA
   if userdata.MoveTarget
       bestgenome =  bestgenome_movtar;
   else    
       % Convert the fitness matrices of newgenomes and oldgenomes into vectors 
       fitList =  reshape(currfitness,userdata.SubsetPopulation*userdata.Subsets,1);
       [fitList, Ind] = sort(fitList);
       % Get the indices of the elite genomes
       NumElites = round(userdata.SubsetPopulation*userdata.Subsets/100*userdata.Elites);
       % Map the elite indices back to the old and new genome indices 
       eliteInd = Ind(end:-1:end-NumElites+1);
       % Find the indices of the genomes in the original matrix
       % structure
       bestgenome =  currgenome(:,eliteInd);
   end;
end;



