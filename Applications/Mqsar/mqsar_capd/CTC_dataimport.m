function [rownames, colnames, xdata, ydata, ynames, bins, maskmat]= ...
    CTC_dataimport(file_train, file_test)
% CTC_DATAIMPORT import train/test data file and construct data for mqsar.
%
% each row in both train.dat and test.dat has following format:
% bin,row id,features

DATAROW_BEGIN = 0;
DATACOL_BEGIN = 2;

xdata_train = csvread(file_train, DATAROW_BEGIN, DATACOL_BEGIN);
xdata_test = csvread(file_test, DATAROW_BEGIN, DATACOL_BEGIN);

fid = fopen(file_train);
text_col = textscan(fid, '%s%s%*[^\n]', 'delimiter', ',');
fclose(fid);
bin_train = text_col{1};
rownames_train = text_col{2};

fid = fopen(file_test);
text_col= textscan(fid, '%s%s%*[^\n]', 'delimiter', ',');
fclose(fid);
bin_test = text_col{1};
rownames_test = text_col{2};


rownames = [rownames_train; rownames_test];
xdata = [xdata_train; xdata_test];
bins = str2num(cell2mat([bin_train; bin_test]));
ydata = bins;
ynames = 'YNAMES';
maskmat = [zeros(size(bin_train, 1), 1); ones(size(bin_test, 1), 1)];

% column name
colnames = {};
for idx = 1:size(xdata, 2)
    colnames{idx, 1} = strcat('COLUMN_', num2str(idx));
end
