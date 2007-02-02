function Models = read_model_ga(modelfile)

fmodel = fopen(modelfile, 'r');
bar = '-------';
Model_No = 0;
tline = fgetl(fmodel);
while tline ~= -1
    
    Submodel = 0;       
    while isempty(findstr(bar,tline))
  
        WeightStr = 'Weights:';
        Wind = findstr(tline,WeightStr);
        if ~isempty(Wind)
            Weights = str2num(tline(Wind+length(WeightStr):end));
        end;
        
        Model_Str = 'Model_';
        Mind = findstr(tline,Model_Str);A
        
        if ~isempty(Mind)
            Model_No = Model_No+1;
            Models(Model_No).Test_CorrectRate = str2num(tline(Mind(1)+length('Model_Test_CorrectRate: '):Mind(2)-1));
            Models(Model_No).Test_TruePos = str2num(tline(Mind(2)+length('Model_Test_TruePos: '):Mind(3)-1)); 
            Models(Model_No).Test_FalsePos = str2num(tline(Mind(3)+length('Model_Test_FalsePos: '):Mind(4)-1));  
            Models(Model_No).Test_FalseNeg = str2num(tline(Mind(4)+length('Model_Test_FalseNeg: '):Mind(5)-1));
            Models(Model_No).CorrectRate = str2num(tline(Mind(5)+length('Model_CorrectRate: '):Mind(6)-1));
            Models(Model_No).TruePos = str2num(tline(Mind(6)+length('Model_TruePos: '):Mind(7)-1)); 
            Models(Model_No).FalsePos = str2num(tline(Mind(7)+length('Model_FalsePos: '):Mind(8)-1));
            Models(Model_No).FalseNeg = str2num(tline(Mind(8)+length('Model_FalseNeg: '):end));
        end;
        
        [str tline] = strtok(tline, '	');
        if strcmpi(str, 'Descriptors:')        
            Submodel = Submodel+1;                       
            Models(Model_No).SubModel(Submodel).Weight = Weights(Submodel);                             
            varnamesel = [];
            i = 1;
            while(~isempty(tline))
                [str tline] = strtok(tline, '	');
                Models(Model_No).SubModel(Submodel).varnamesel{i} = (str);
                
                i = i + 1;
            end	
        end
        tline = fgetl(fmodel);
        if ~ischar(tline)
            tline = -1; break; 
        end
    end;   
               
    if (Submodel> 0)
       tline = fgetl(fmodel);     
    end;         
end;
fclose(fmodel);
