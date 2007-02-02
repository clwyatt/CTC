function qsarresult(userdata,bestgenome,VarNames,CmpNames,Xmat,Ymat,Bins,CrossMat,nModel,fmodel,fout)


for cv=1:size(CrossMat,2)
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

    for ii=1:size(bestgenome,2)
        switch char(userdata.method)
        case 'CM'         
            % Calculate the fitness of the built model
            [Results(ii).class, Results(ii).prd,Dcenter,Results(ii).stats] = cm_model(tmpX(:,find(bestgenome(:,ii))),tmpBins,tmpXTest(:,find(bestgenome(:,ii))), ...
                                                tmpBinsTest, userdata.ClassBins,userdata.Cluster);
            % Write the results to the model file      
            switch userdata.Scaling
            case 1 % No scaling
                FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, ...
                'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd); 
            case 2 % Normalize to Mahalanobis distance
                FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, 'Mean', xmean(find(bestgenome(:,ii))), ...
                'Std', xstd(find(bestgenome(:,ii))),'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd);   
            case 3 % Normalize with respect to data range
                % Don't know exactly when this scling is used, so will
                % fill in later if it is useful
            end       
        case 'FCM'         
            % Calculate the fitness of the built model
            [Results(ii).class, Results(ii).prd,Member_matrix,Dcenter,Results(ii).stats] = fcm_model(tmpX(:,find(bestgenome(:,ii))),tmpBins,tmpXTest(:,find(bestgenome(:,ii))), ...
                                                tmpBinsTest, userdata.ClassBins,userdata.Cluster);
            % Write the results to the model file      
            switch userdata.scaling
            case 'None'
                FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, ...
                     'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd); 
            case 'Mahalanobis' % Normalize to Mahalanobis distance
                FcmResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, 'Mean', xmean(find(bestgenome(:,ii))), ...
                     'Std', xstd(find(bestgenome(:,ii))),'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd);   
            case 'Min-Max' % Normalize with respect to data range
                % Don't know exactly when this scling is used, so will
                % fill in later if it is useful
            end
        case 'KNN'

        case 'MDS'

        case 'MLR'

        case 'NMLR'

        case 'NNET'
            [Results(ii).TrPrdAct,Results(ii).TrPrdBin,Results(ii).TstPrdAct, ...
             Results(ii).TstPrdBin, Results(ii).stats] = nnet_model(tmpX(:,find(bestgenome(:,ii))), ...
                     tmpY, tmpBins, 'Test',tmpXTest(:,find(bestgenome(:,ii))),tmpYTest,tmpBinsTest, ...
                     'Cutoff',userdata.cutoff,'Type',userdata.nntype,'Method',userdata.nnmethod,...
                     'Hidden Layer',[userdata.hiddenstart userdata.hiddenstart2],...
                     [userdata.hiddenstop userdata.hiddenstop2],...
                     'Training',userdata.nntraining,'Function',userdata.nnfunction,...
                     'Epoch',userdata.epoch);   
          
            switch userdata.scaling
            case 'None'
                NNETResult(VarNames(find(bestgenome(:,ii))), tmpNames, Results(ii).stats, tmpY,Results(ii).TrPrdAct, ...
                tmpBins,Results(ii).TrPrdBin, fout, fmodel,'test', nModel, ...
                tmpNamesTest,tmpYTest,Results(ii).TstPrdAct, tmpBinsTest, Results(ii).TstPrdBin); 
            case 'Mahalanobis' % Normalize to Mahalanobis distance
                NNETResult(VarNames(find(bestgenome(:,ii))), tmpNames, Results(ii).stats, tmpY,Results(ii).TrPrdAct, ...
                tmpBins,Results(ii).TrPrdBin, fout, fmodel,'test', nModel, ...
                tmpNamesTest,tmpYTest,Results(ii).TstPrdAct, tmpBinsTest, Results(ii).TstPrdBin, ...
                'Mean', xmean(find(bestgenome(:,ii))),'Std', xstd(find(bestgenome(:,ii))));   
            case 'Min-Max' % Normalize with respect to data range
                % Don't know exactly when this scaling is used, so will
                % fill in later if it is useful
            end              
        case 'PCR'
            %dmin = min(dist)
            %dmax = max(dist)
            %dsum = 1/sum(dist)
            %diversity = mean(dist)
            disp(1/sum(dist));
            fprintf(fout, '%f\n', 1/sum(dist));
        case 'PLS'  
            [B, BestPrinCompn, E, Fitness, Expterm, LogTemp, W, Q, P, MeanStd] = npls(Xmat(:,find(bestgenome(:,ii))), ...
                Ymat, userdata.ComponNum, szVal, szSquare, szExp, szInverse, szLog, szNorm);
            for i=1:size(Fitness, 2)
                if i == 1
                    tmpStr = ['R^2 = ', num2str(Fitness(i))];
                elseif i == 2
                    tmpStr = [tmpStr '  Q^2 = ', num2str(Fitness(i))];
                end
            end
            szStr = strvcat(szStr, tmpStr);
            ShowInf(szStr, handles);
            WritePLSModel(fmodel, B, BestPrinCompn, E, Fitness, ...
                Expterm, LogTemp, W, Q, P, MeanStd);
            WritePLSOut(fout, CmpNames, E, Ymat, BestPrinCompn, Fitness);                                            
        case 'SOM' 
            % Write the results to the model file
            [Results(ii).prd, Results(ii).stats, Results(ii).class] = som_model(tmpX(:,find(bestgenome(:,ii))), tmpBins, ...
                                             VarNames(find(bestgenome(:,ii))), tmpXTest(:,find(bestgenome(:,ii))), ...
                                             'Neighbor',userdata.neighbor,'Shape',userdata.shape,...
                                             'Map',userdata.sommap,'Type',userdata.somtype,'testbin', tmpBinsTest);
            switch userdata.scaling
            case 'None'
                SomResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, ...
                'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd); 
            case 'Mahalanobis' % Normalize to Mahalanobis distance
                SomResult(VarNames(find(bestgenome(:,ii))), Results(ii).stats, Results(ii).class, tmpNames, tmpBins, fout, fmodel, 'Mean', xmean(find(bestgenome(:,ii))), ...
                'Std', xstd(find(bestgenome(:,ii))),'test', nModel, tmpNamesTest, tmpBinsTest, Results(ii).prd);   
            case 'Min-Max' % Normalize with respect to data range
                % Don't know exactly when this scaling is used, so will
                % fill in later if it is useful
            end         
        case 'SVM' 
        end  
        if not(isempty(tmpXTest))
                   Weights(ii) = Results(ii).stats.test_correct_rate*Results(ii).stats.correct_rate;
        else
               Weights(ii) = Results(ii).stats.correct_rate;
        end; 
    end; %for ii=1:size(bestgenome,2)
end; %for CrossMat

% If there are more than one models reported, combine their results
% to a single decision classification
if size(bestgenome,2) > 1
    bindif = 2;
    ResultSize = size(bestgenome,2);
    NumComp = length(tmpBins); 
    binnum = length(userdata.cutoff) + 1;
    Votes = zeros(binnum,NumComp);
    for i=1:NumComp
        for j = 1: ResultSize
            Votes(Results(j).class(i), i) = Votes(Results(j).class(i), i) + Weights(j);
        end;
    end;    
    [Votes, SortInd] = sort(Votes,1);
    Calc_Bins = SortInd(end,:)';

    NumTestComp = length(tmpBinsTest); 
    if ~isempty(tmpBinsTest)
        Votes_Test = zeros(binnum,NumTestComp);
        for i=1:NumTestComp
            for j = 1: ResultSize
                Votes_Test(Results(j).prd(i), i) = Votes_Test(Results(j).prd(i), i) + Weights(j);
            end;
        end;    
        [Votes_Test, SortInd] = sort(Votes_Test,1);
        Calc_TestBins = SortInd(end,:)';  
    end;

    if isempty(tmpBinsTest)
       Decision = modelstats(Calc_Bins, tmpBins, bindif); 
       Decision.class = Calc_Bins;
       szTmp2 = [' Model_TruePos: ', num2str(Decision.correct_rate), ...
             ' Model_False_Pos: ', num2str(Decision.false_pos), ...
             ' Model_False_Neg: ', num2str(Decision.false_neg)];
    else   
       Decision = modelstats_test(Calc_TestBins, tmpBinsTest, Calc_Bins, tmpBins, bindif);
       Decision.class = Calc_Bins;
       Decision.test_class = Calc_TestBins;
       szTmp2 = [' Model_TruePos: ', num2str(Decision.correct_rate), ...
             ' Model_False_Pos: ', num2str(Decision.false_pos), ...
             ' Model_False_Neg: ', num2str(Decision.false_neg), ...
             ' Model_Test_TruePos: ', num2str(Decision.test_correct_rate), ... 
             ' Model_Test_False_Pos: ', num2str(Decision.test_false_pos), ...
             ' Model_Test_False_Neg: ', num2str(Decision.test_false_neg) ];
    end;
    szTmp3 = ['Weights: ', num2str(Weights)];
    fprintf(fmodel, '\n%s\n%s\n',szTmp3, szTmp2);
    fprintf(fout, '\n%s\n%s\n',szTmp3, szTmp2);   
    fprintf(fout, '%s\t\n', 'Training Set');
    fprintf(fout, '%s\t%s\t%s\n', 'Name', 'Observed', 'Model Calculation');
    for i=1:length(NumComp)
        fprintf(fout, '%s\t%i\t%i\n', char(tmpNames(i)), tmpBins(i), Decision.class(i));
    end
    if ~isempty(tmpBinsTest)
        fprintf(fout, '\n%s\n', 'Test Set');
        fprintf(fout, '%s\t%s\t%s\n', 'Name', 'Observed', 'Model Prediction');
        for i=1:length(NumTestComp)
            fprintf(fout, '%s\t%i\t%i\n', char(tmpNamesTest(i)), tmpBinsTest(i), Decision.test_class(i));
        end
    end
    fprintf(fout, '\n');
    fprintf(fmodel, '\n');  
    Bar = '------------------------------------------------';
    fprintf(fmodel, '%s\n', Bar);
    fprintf(fout, '%s\n', Bar);
end;    % if size(bestgenome,2) > 1 