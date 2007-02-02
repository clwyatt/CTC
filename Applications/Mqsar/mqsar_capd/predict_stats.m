fid = fopen('../WFU_Models/a4b2/Model/gasom_3bin_20_AvgRep.mod');

bar = '------------------------------------------------';
while 1
    tline = fgets(fid);
    if ~ischar(tline), break, end
    disp(tline)
end
fclose(fid);