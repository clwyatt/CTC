function CTC_generate_train_test_data()
% CTC_GENERATE_TRAIN_TEST_DATA Generate 'train.data' and 'test.dat' file 

PROB_TRAIN_TEST = [0.8, 0.2]';  % fraction of train and test data

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% gaussian data
% 
% NUM_DATA = 200;  % number of data
% DIM_DATA = 12;  % dimension of data
% MEAN1 = -1; VAR1 = 1;
% MEAN2 = 1; VAR2 = 1;
% data_class = util_random_pick(PROB_TRAIN_TEST, NUM_DATA);
%
% % generate data for class 1
% data = randn(NUM_DATA, DIM_DATA) * VAR1 + MEAN1;
% train_data_1 = data(data_class == 1, :);
% train_bin_1 = ones(size(train_data_1, 1), 1);
% test_data_1 = data(data_class == 2, :);
% test_bin_1 = ones(size(test_data_1, 1), 1);
% 
% % generate data for class 2
% data = randn(NUM_DATA, DIM_DATA) * VAR2 + MEAN2;
% train_data_2 = data(data_class == 1, :);
% train_bin_2 = ones(size(train_data_2, 1), 1) * 2;
% test_data_2 = data(data_class == 2, :);
% test_bin_2 = ones(size(test_data_2, 1), 1) * 2;
% 
% % combine data of class 1 and class 2
% train_data = [train_data_1; train_data_2];
% train_bin = [train_bin_1; train_bin_2];
% test_data = [test_data_1; test_data_2];
% test_bin = [test_bin_1; test_bin_2];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% spiral data

% load('spirals.mat');
% patterns = patterns';
% targets = targets' + 1; % 
% 
% data_class = util_random_pick(PROB_TRAIN_TEST, size(patterns, 1));
% 
% train_data = patterns(data_class == 1, :);
% train_bin = targets(data_class == 1, :);
% test_data = patterns(data_class == 2, :);
% test_bin = targets(data_class == 2, :);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% HMM (Hidden Markov Model) data

tp = [0.9, 0.1; 0.2, 0.8];
mu = [-1, 1]';
sigma = [1, 1]';
n = 100;

vis_symbol = zeros(n, 1);
vis_symbol_state = zeros(n + 1, 1);
% assumre initially we are in the first state
vis_symbol_state(1) = 1;
for idx = 1:n
    tmp = cumsum(tp(vis_symbol_state(idx), :));
    vis_symbol(idx) = randn() * sigma(vis_symbol_state(idx)) + ...
                        mu(vis_symbol_state(idx));
    next_state_idx = find(tmp >= rand());
    vis_symbol_state(idx + 1) = next_state_idx(1);
end
vis_symbol_state(end) = [];

feature_num = 2;
switch feature_num
    case 1
        % one feature
        data = vis_symbol;
        bin = vis_symbol_state;
    case 2
        % two features: (1, 2), (2, 3), (3, 4), .... The bin value is based on the 
        % second element in each pair: 2 in (1, 2), 3 in (2, 3), ...
        data = [vis_symbol, circshift(vis_symbol, -1)];
        data(end, :) = [];
        bin = circshift(vis_symbol_state, -1);
        bin(end, :) = [];
    case 3
        % three features: (1, 2, 3), (2, 3, 4), (3, 4, 5), .... The bin value is 
        % based on the middle element in each triple: 2 in (1, 2, 3), 3 in (2, 3,
        % 4), ...
        data = [vis_symbol, circshift(vis_symbol, -1), circshift(vis_symbol, -2)];
        data(end - 1:end, :) = [];
        bin = circshift(vis_symbol_state, -1);
        bin(end - 1:end, :) = [];
    otherwise
        error('how many features in HMM data? %g', feature_num);
end

data_class = util_random_pick(PROB_TRAIN_TEST, size(data, 1));
train_data = data(data_class == 1, :);
train_bin = bin(data_class == 1, :);
test_data = data(data_class == 2, :);
test_bin = bin(data_class == 2, :);

% classify using Viterbi algorithm
for idx = 1:size(mu, 1)
    ep(idx, :) = normpdf(vis_symbol, mu(idx), sigma(idx))';  % emission matrix
end
seq = [1:size(vis_symbol, 1)];
estimated_state = hmmviterbi(seq, tp, ep)';

stats = struct();
stats.error_rate = sum((estimated_state ~= vis_symbol_state))/size(vis_symbol_state, 1);
stats.correct_rate = sum((estimated_state == vis_symbol_state))/size(vis_symbol_state, 1);
stats.true_positive = sum((estimated_state == 1).*(vis_symbol_state == 1))/sum(vis_symbol_state == 1);
stats.false_positive = sum((estimated_state == 1).*(vis_symbol_state == 2))/sum(vis_symbol_state == 2);
stats.true_negative = sum((estimated_state == 2).*(vis_symbol_state == 2))/sum(vis_symbol_state == 2);
stats.false_negative = sum((estimated_state == 2).*(vis_symbol_state == 1))/sum(vis_symbol_state == 1);
stats.sensitivity = stats.true_positive;
stats.specificity = stats.true_negative;

disp('Viterbi classification result:');
disp(stats);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%

rowids = cumsum(ones(size(train_data, 1) + size(test_data, 1), 1));
train_rowids = rowids(1:size(train_data,1), :);
test_rowids = rowids(size(train_data,1) + 1:end, :);

temp_train = num2str(train_data(:, 1));
for idx = 2:size(train_data, 2)
    temp_train = strcat(temp_train, ',', num2str(train_data(:, idx)));
end

temp_test = num2str(test_data(:, 1));
for idx = 2:size(test_data, 2)
    temp_test = strcat(temp_test, ',', num2str(test_data(:, idx)));
end

train_output = strcat(num2str(train_bin), ',', num2str(train_rowids), ',', temp_train);
test_outpout = strcat(num2str(test_bin), ',', num2str(test_rowids), ',', temp_test);

% write train.dat and test.dat
fid = fopen('train.dat', 'w+');
for idx = 1:size(train_output, 1)
    fprintf(fid, '%s\n', train_output(idx, :));
end
fclose(fid);

fid = fopen('test.dat', 'w+');
for idx = 1:size(test_outpout, 1)
    fprintf(fid, '%s\n', test_outpout(idx, :));
end
fclose(fid);


