function Results = qsarpredict(userdata,VarNames,CmpNames,Xmat,Ymat,Bins,...
                                             PrdCmpNames, PrdVarNames, PrdX, PrdY, PrdBins,...
                                             Maskmat,nModel);

%Shuffle the bioact and bins if we are running a randomized experiment
%if userdata.ShuffleFlag
%   RandInd = randperm(length(Ymat)); 
%   Ymat = Ymat(RandInd);
%   Bins = Bins(RandInd);    
%end  

if nModel ==1  
    % Check if the output file name is valid
    try
        fout = fopen(userdata.outputfile, 'w');
        fprintf(fout, '%s\t%s\n', char(strcat(userdata.method, ' results')), char(date));
    catch
        errordlg('Please enter a valid output file name!','I/O Error');
        return;
    end
end;

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

fmodel = fopen(userdata.modelfile, 'r');   
nModel = 0;
tline = fgetl(fmodel);
while tline ~= -1
    [str tline] = strtok(tline);
    if strcmpi(str, 'Descriptors:')
        nModel = nModel + 1; 
        varnamesel = [];
        i = 1;
        while(~isempty(tline))
            [str tline] = strtok(tline);
            varnamesel{i} = (str);
            i = i + 1;
        end

        for i=1:length(varnamesel)
            for j=1:length(VarNames)
                if strcmp(varnamesel(i), VarNames(j))
                    Selected(nModel).ind(i) = j;
                    break;
                end
            end
            % define the key descriptor in the prediction set. 
            for j=1:length(PrdVarNames)
                if strcmp(varnamesel(i), PrdVarNames(j))
                    PrdVar(nModel).ind(i) = j;
                    break;
                end
            end
        end
        if length(Selected(nModel).ind)~= length(varnamesel)
            disp(char(varnamesel));
            disp('Please check your training set and the key descriptors');
            prd = [];
            stats = [];
            class = [];
            return;
        end
    end;

    
    tline = fgetl(fmodel);
    if isempty(tline)
        tline = 1;
    end
end
% fclose(fmodel);  

for cv = 1:size(CrossMat,2)  
    index = find(CrossMat(:, cv));
    % Training set information
    tmpXmat = Xmat(:, Selected(cv).ind);
    tmpX = tmpXmat;
    tmpX(index, :) = [];
    tmpY = Ymat;
    tmpY(index, :) = [];
    tmpPrdX = PrdX(:,PrdVar(cv).ind);
     
    tmpBins = Bins;
    tmpBins(index) = [];
    tmpNames = CmpNames;
    tmpNames(index) = [];

    % test set information
    tmpXTest = [tmpPrdX; tmpXmat(index, :)];
    tmpBinsTest = [PrdBins ; Bins(index)];
    tmpNamesTest = [PrdCmpNames ; CmpNames(index)];
    tmpYTest = [PrdY; Ymat(index, :)];

    switch char(userdata.method)
    case 'CM'
        [Results(cv).class, Results(cv).prd,Dcenter, Results(cv).stats] = cm_model(tmpX,tmpBins,tmpXTest, ...
                                    tmpBinsTest, userdata.ClassBins,userdata.Cluster);
                                % Write the results to the model file      
    
        FcmResult(VarNames(Selected(cv).ind), Results(cv).stats, Results(cv).class, tmpNames, tmpBins, fout, fmodel, ...
        'test', cv, tmpNamesTest, tmpBinsTest, Results(cv).prd); 
      
    case 'FCM'
        [Results(cv).class, Results(cv).prd,Dcenter, Results(cv).stats] = fcm_model(tmpX,tmpBins,tmpXTest, ...
                                    tmpBinsTest, userdata.ClassBins,userdata.Cluster);
        FcmResult(VarNames(Selected(cv).ind), Results(cv).stats, Results(cv).class, tmpNames, tmpBins, fout, fmodel, ...
        'test', cv, tmpNamesTest, tmpBinsTest, Results(cv).prd);                         
    case 'KNN'

    case 'MDS'

    case 'MLR'

    case 'NMLR'

    case 'NNET'
        [Results(cv).TrPrdAct,Results(cv).TrPrdBin, Results(cv).TstPrdAct, ...
        Results(cv).TstPrdBin, Results(cv).stats] = nnet_model(tmpX, tmpY, tmpBins, ...
                            'Test',tmpXTest,tmpYTest,tmpBinsTest, ...
                            'Cutoff',userdata.cutoff,'Type',userdata.nntype,'Method',userdata.nnmethod,...
                            'Hidden Layer',[userdata.hiddenstart userdata.hiddenstart2],...
                            [userdata.hiddenstop userdata.hiddenstop2],...
                            'Training',userdata.nntraining,'Function',userdata.nnfunction,...
                            'Epoch',userdata.epoch,'Bin Sort',userdata.sortgoodtobad); 
                        
         NNETResultPrd(VarNames(Selected(cv).ind), tmpNames, Results(cv).stats, tmpY,Results(cv).TrPrdAct, ...
                tmpBins,Results(cv).TrPrdBin, fout,'test', cv, ...
                tmpNamesTest,tmpYTest,Results(cv).TstPrdAct, tmpBinsTest, Results(cv).TstPrdBin);                 
    case 'PCR'
        dist = pairwise(tmpXTest);
        for i=1:length(dist)
            dist(i) = 1/dist(i);
        end

    case 'PLS'    
        [B, BestPrinCompn, E, Fitness, Expterm, ...
         LogTemp, W, Q, P, MeanStd] = npls(tmpX, tmpY, ...
                                userdata.ComponNum, szVal, szSquare, szExp, szInverse, szLog, szNorm);
            
    case 'SOM' 
        [Results(cv).prd, Results(cv).stats, Results(cv).class] = som_model(tmpX, tmpBins, ...
                             VarNames(Selected(cv).ind), tmpXTest, ...
                             'Neighbor',userdata.neighbor,'Shape',userdata.shape,...
                             'Map',userdata.sommap,'Type',userdata.somtype,'testbin', tmpBinsTest, ...
                             'Bin Sort',userdata.sortgoodtobad);      
        
        SomResultPrd(VarNames(Selected(cv).ind), Results(cv).stats, Results(cv).class, tmpNames, tmpBins, fout, ...
                'test', cv, tmpNamesTest, tmpBinsTest, Results(cv).prd);                     
    case 'SVM'

    end   
    
end; % for cv - This is the cross validation based modeling loop






