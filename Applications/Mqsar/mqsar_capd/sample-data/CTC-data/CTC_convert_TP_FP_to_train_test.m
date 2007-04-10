function CTC_convert_TP_FP_to_train_test(file_tp, file_fp, train_percentage)
% CTC_convert_TP_FP_to_train_test Convert 'false.csv' and 'true.csv' file to
% 'train.dat' and 'test.dat' file.
%
% each row in both train.dat and test.dat has following format:
% bin,row id,features

fid = fopen(file_tp);
text_tp = textscan(fid, '%s%s%s%s%s%*[^\n]', 'delimiter', ',');
fseek(fid, 0, 'bof');
data_tp = textscan(fid, '%*s%*s%*s%*s%*s%s%s%s%s%s%s%s%s%s%s', 'delimiter', ',');
fclose(fid);
fid = fopen(file_fp);
text_fp = textscan(fid, '%s%s%s%s%s%*[^\n]', 'delimiter', ',');
fseek(fid, 0, 'bof');
data_fp = textscan(fid, '%*s%*s%*s%*s%*s%s%s%s%s%s%s%s%s%s%s', 'delimiter', ',');
fclose(fid);

% row id: MRN + series_id + x + y + z
ROWID_DELIM = '|';
rowids_tp = {};
for idx = 1:size(text_tp, 2)
    rowids_tp = strcat(rowids_tp, text_tp{:, idx}, ROWID_DELIM);
end
rowids_fp = {};
for idx = 1:size(text_fp, 2)
    rowids_fp = strcat(rowids_fp, text_fp{:, idx}, ROWID_DELIM);
end

% row data
data_tp = strcat(data_tp{1}, ',', data_tp{2}, ',', data_tp{3}, ',', ...
    data_tp{4}, ',', data_tp{5}, ',', data_tp{6}, ',', data_tp{7}, ',', ...
    data_tp{8}, ',', data_tp{9}, ',', data_tp{10});

data_fp = strcat(data_fp{1}, ',', data_fp{2}, ',', data_fp{3}, ',', ...
    data_fp{4}, ',', data_fp{5}, ',', data_fp{6}, ',', data_fp{7}, ',', ...
    data_fp{8}, ',', data_fp{9}, ',', data_fp{10});

% mask matrix - how many sample data will be used as testing data. 
% 0 - train, 1 - test
prob = [train_percentage, 1 - train_percentage]; 
is_test_tp = util_random_pick(prob, size(rowids_tp, 1)) - 1;
is_test_fp = util_random_pick(prob, size(rowids_fp, 1)) - 1;

train = {};
test = {};
% '1,' - class 1 (true positive)
% '2,' - class 2 (false positive)

if sum(is_test_tp == 0) > 0 
    train = strcat('1,', {rowids_tp{is_test_tp == 0}}, ',', {data_tp{is_test_tp == 0}});
end
if sum(is_test_fp == 0) > 0
    train = [train, strcat('2,', {rowids_fp{is_test_fp == 0}}, ',', {data_fp{is_test_fp == 0}})];
end
if sum(is_test_tp == 1) > 0
    test = strcat('1,', {rowids_tp{is_test_tp == 1}}, ',', {data_tp{is_test_tp == 1}});
end
if sum(is_test_fp == 1) > 0
    test = [test, strcat('2,', {rowids_fp{is_test_fp == 1}}, ',', {data_fp{is_test_fp == 1}})];
end

% write train.dat and test.dat
fid = fopen('train.dat', 'w+');
for idx = 1:size(train, 2)
    fprintf(fid, '%s\n', train{idx});
end
fclose(fid);

fid = fopen('test.dat', 'w+');
for idx = 1:size(test, 2)
    fprintf(fid, '%s\n', test{idx});
end
fclose(fid);


