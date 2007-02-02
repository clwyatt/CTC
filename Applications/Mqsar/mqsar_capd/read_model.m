function [varnamesel, stats, nmodels] = read_model(modelfile);
    
fmodel = fopen(modelfile, 'r');
nModel = 0;
tline = fgetl(fmodel);
while tline ~= -1
[str tline] = strtok(tline, '	');
if strcmpi(str, 'Descriptors:')
    
    nModel = nModel + 1;              
    index = find(Maskmat(:, nModel));
            
    varnamesel = [];
    i = 1;
    while(~isempty(tline))
        [str tline] = strtok(tline, '	');
        varnamesel{i} = (str);
        i = i + 1;
    end

end
tline = fgetl(fmodel);
if isempty(tline)
    tline = 1;
end
end
fclose(fmodel);