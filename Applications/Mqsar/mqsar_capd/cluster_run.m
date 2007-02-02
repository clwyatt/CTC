function Result = cluster_run(Parameter_file);
 
% Ersin reseed the random number generator
rand('state',sum(100*clock));	%re-seed random number generator

load(Parameter_file);

Result = userdata;
 
% Import the data and prepare according to user defined filter parameters
[CmpNames, VarNames, Xmat, Ymat, Ynames, Bins,Maskmat] = prepdata(userdata);
% Create a model


if userdata.ShuffleFlag
   RandInd = randperm(length(Ymat)); 
   Ymat = Ymat(RandInd);
   Bins = Bins(RandInd);    
end   

 

switch userdata.validation
case 'None'
    ModelLimit = size(Maskmat, 2);
case {'Leave-One-Out','Leave-Some-Out'}      
    ModelLimit = 1;
end;

if (userdata.modelflag)
    for nModel=1:ModelLimit

        [bestgenome,CrossMat] = qsarmodel(userdata,VarNames,CmpNames,Xmat,Ymat,Bins,Maskmat,nModel);
        % Best descriptors are found. Create the model for final statistics
        if nModel ==1
            % Open the model file
            fmodel = fopen(userdata.modelfile, 'w');
            fprintf(fmodel, '%s\t%s\n', char(strcat(userdata.method, ' model')), char(date));

            % Check if the output file name is valid
            try
                fout = fopen(userdata.outputfile, 'w');
                fprintf(fout, '%s\t%s\n', char(strcat(userdata.method, ' results')), char(date));
            catch
                errordlg('Please enter a valid output file name!','I/O Error');
                return;
            end
        end;
        qsarresult(userdata,bestgenome,VarNames,CmpNames,Xmat,Ymat,Bins,CrossMat,nModel,fmodel,fout)  
    end % for nModel
    fclose(fmodel);
    fclose(fout); 
end;
if userdata.predictflag  % Predict bioactivity/Class
% XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
% The following block is designed for using the built model(s) to
% predict the specify external data
% XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  
    % Read file according to the specified options
   [PrdCmpNames, PrdVarNames, PrdX, PrdY, PrdYname, PrdBins] = qsarimport(userdata.tstfile, 'bioact', ...
                                                     userdata.tstbioact, 'binning', userdata.tstbin, ...
                                                    'cutoffs', userdata.cutoff, 'ex', userdata.tstexclude);
  if isempty(PrdY)
      PrdY = zeros(length(PrdCmpNames),1);
  end;
  if isempty(PrdBins)
      PrdBins = ones(length(PrdCmpNames),1);
  end;
  
   for nModel=1:ModelLimit
        
        Results = qsarpredict(userdata,VarNames,CmpNames,Xmat,Ymat,Bins,...
                                       PrdCmpNames, PrdVarNames, PrdX, PrdY, PrdBins,Maskmat,nModel);
        % Best descriptors are found. Create the model for final statistics
        %qsarresult(userdata,bestgenome,VarNames,CmpNames,Xmat,Ymat,Bins,CrossMat,nModel,fmodel,fout)  
    end % for nModel    
end;


    
