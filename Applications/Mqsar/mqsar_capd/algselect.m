function Result = algselect(Parameter_file);

load Parameter_file;

if userdata.ModelFlag || strcmp(userdata.szMethod, 'KNN') || strcmp(userdata.szMethod, 'SOM')
   % Read file according the specify options
   [CmpNames VarNames, Xmat, Ymat, Ynames, Bins] = qsarimport(userdata.szDataName, 'bioact', ...
    userdata.bioact, 'binning', userdata.bin, 'ex', userdata.exc, 'cutoffs', userdata.cutoff);
end;

userdata.cmpnames = CmpNames;
userdata.xnames = VarNames;
userdata.ynames = Ynames;
userdata.x = Xmat;
userdata.y = Ymat;
userdata.bins = Bins;

if userdata.ModelFlag
    if ~isempty(userdata.PartitionFile)
        [rownames columnames Maskmat] = qsarimport(userdata.PartitionFile); 
    end
    
    switch userdata.Scaling
        case 2
            xmean = mean(Xmat);
            xstd = std(Xmat);
            for i=1:size(Xmat, 2)
                Xmat(:, i) = (Xmat(:, i) - xmean(i))/xstd(i);
            end
        case 3
            xmin = min(Xmat);
            xmax = max(Xmat);
            for i=1:size(Xmat, 2)
                Xmat(:, i) = (Xmat(:, i) - xmin(i))/(xmax(i) - xmin(i));
            end
    end
    
    % Open the model file
    fmodel = fopen(userdata.szModelName, 'w');
    fprintf(fmodel, '%s\t%s\n', char(strcat(userdata.szMethod, ' model')), char(date));

else
    % XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    % The following block is designed for using the built model(s) to
    % predict the specify external data
    % XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

% Read prediction file to get external data
[PrdCmpNames PrdVarNames, PrdX] = qsarimport(userdata.szPrdName);    
fmodel = fopen(userdata.szModelName, 'r');
fprintf(fout, '%s%s\n', 'The predictions of ', char(userdata.szPrdName));

switch char(userdata.szMethod)
        case 'KNN'
        case 'MDS'
        case 'MLR'
        case 'NMLR'
        case 'PCR'
        case 'PLS'
        case 'SOM'
            tline = fgetl(fmodel);
            while tline ~= -1
                [str tline] = strtok(tline, '	');
                if strcmpi(str, 'Descriptors:')
                    varnamesel = [];
                    i = 1;
                    while(~isempty(tline))
                        [str tline] = strtok(tline, '	');
                        varnamesel{i} = (str);
                        i = i + 1;
                    end
					[prd, stats, class] = sompredict(Xmat, Bins, VarNames, varnamesel, PrdX, PrdVarNames);
					szStr = [' Correct rate: ', num2str(stats.correct_rate), ...
                                   ' and variable number: ', num2str(stats.varnum)];
					fprintf(fout, '%s\n', szStr);
					szTmp = char(varnamesel(1));
					for i=2:length(varnamesel)
						szTmp = [szTmp, '	', char(varnamesel(i))];
					end
					szStr = strvcat(szStr, szTmp);
					ShowInf(szStr, handles);
					fprintf(fout, '%s\n', char(szTmp));
					fprintf(fout, '%s\t%s\n', 'Name', 'Predition');
					for i=1:length(PrdCmpNames)
						fprintf(fout, '%s\t%i\n', char(PrdCmpNames(i)), prd(i));
					end
                end
                tline = fgetl(fmodel);
                if isempty(tline)
                    tline = 1;
                end
            end
            fclose(fmodel);
        case 'SVM'
    end
    % ---------------------------------------------------------------
    % the end of prediction block
end
fclose(fout);
% The End of Cluster job
%---------------------------------------------------------------------

    
    
switch Parameters.Mode
case 'Model'    
   display('Calling ga_start!')
   ga_start(Parameters);
case 'Predict'
   display('Calling ga_restart!')
   ga_restart(Parameters);     
case 'Both'
   display('Calling ga_start!')
   ga_start(Parameters);
   
   display('Calling ga_restart!')
   ga_restart(Parameters);
end;
