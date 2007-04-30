function classify_HMM_data()


%% analyze data
tic;
analyze_data('output_train_20.xls');
analyze_data('output_test_20.xls');
analyze_data('output_bundle_20.xls');
toc;

return;

% parameters for HMM data
tp = [0.1, 0.9; 0.9, 0.1];
mu = [-1, 1]';
sigma = [1, 1]';
n = 1000;

tic;
%% generate and classify data
NUM_EXPERIMENT = 20;
for idx_experiment = 1:NUM_EXPERIMENT
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

    % classify using Viterbi algorithm
    for idx = 1:size(mu, 1)
        ep(idx, :) = normpdf(vis_symbol, mu(idx), sigma(idx))';  % emission matrix
    end
    seq = [1:size(vis_symbol, 1)];
    estimated_state = hmmviterbi(seq, tp, ep)';
    output_viterbi = estimated_state;
    if feature_num == 2
        output_viterbi = output_viterbi(2:end);
    elseif feature_num == 3
        output_viterbi = output_viterbi(2:end - 1);
    end

    % classify using Bayesian rule
    threshold = sum(mu)/2;

    if feature_num == 1
        output_bayesian = zeros(n, 1);
        output_bayesian(data >= threshold) = 2;
        output_bayesian(data < threshold) = 1;
    elseif feature_num == 2
        output_bayesian = zeros(n - 1, 1);
        output_bayesian(data(:, 2) >= threshold) = 2;
        output_bayesian(data(:, 2) < threshold) = 1;
    elseif feature_num == 3
        output_bayesian = zeros(n - 2, 1);
        output_bayesian(data(:, 2) >= threshold) = 2;
        output_bayesian(data(:, 2) < threshold) = 1;
    end

    % classify using nnet model in mqsar

    %generate tarin/test data
    PROB_TRAIN_TEST = [0.8, 0.2]';  % fraction of train and test data
    data_class = util_random_pick(PROB_TRAIN_TEST, size(data, 1));
    train_data = data(data_class == 1, :);
    train_bin = bin(data_class == 1, :);
    test_data = data(data_class == 2, :);
    test_bin = bin(data_class == 2, :);

    rowids = cumsum(ones(size(train_data, 1) + size(test_data, 1), 1));
    train_rowids = rowids(1:size(train_data,1), :);
    test_rowids = rowids(size(train_data,1) + 1:end, :);

    % write those data
    xlswrite('input_train.xls', [train_bin, train_rowids, train_data], idx_experiment);
    xlswrite('input_test.xls', [test_bin, test_rowids, test_data], idx_experiment);
    xlswrite('input_bundle.xls', [bin, rowids, data], idx_experiment);

    
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

    % train and test with nnet
    [output_nnet_train_y, output_nnet_train, output_nnet_test_y, output_nnet_test, stats] = ...
        nnet_model(train_data, train_bin, train_bin, ...
                    'Test',test_data, test_bin, test_bin, ...
                    'Cutoff', [], 'Type', 'Feed Forward', 'Method', 'Classify', ...
                    'Hidden Layer',[3 3], [7 7],...
                    'Training', 'Default', 'Function', 'Default',...
                    'Epoch', 100,'Bin Sort', 0);
    %output_nnet_train = ones(size(train_data, 1), 1);
    %output_nnet_test = ones(size(test_data, 1), 1);

    % write output of Bayesian, viterbi, and nnet classifiers
    output_bayesian_train = output_bayesian(data_class == 1, :);
    output_bayesian_test = output_bayesian(data_class == 2, :);
    output_viterbi_train = output_viterbi(data_class == 1, :);
    output_viterbi_test = output_viterbi(data_class == 2, :);
    output_nnet = zeros(size(bin, 1), 1);
    output_nnet(data_class == 1, :) = output_nnet_train;
    output_nnet(data_class == 2, :) = output_nnet_test;
    output_train = [train_bin, output_bayesian_train, output_viterbi_train, output_nnet_train];
    output_test = [test_bin, output_bayesian_test, output_viterbi_test, output_nnet_test];
    output_bundle = [bin, output_bayesian, output_viterbi, output_nnet];

    xlswrite('output_train.xls', output_train, idx_experiment);
    xlswrite('output_test.xls', output_test, idx_experiment);
    xlswrite('output_bundle.xls', output_bundle, idx_experiment);
    disp(sprintf('done experiment no. %g', idx_experiment));
end
toc;

%% analyze data


function analyze_data(filename)
% output file format: actual bin, predicated bin by Bayesian, predicated
% bin by Viterbi, predicated bin by NNet
NUM_EXPERIMENT = 20;
for idx_experiment = 1:NUM_EXPERIMENT
    output = xlsread(filename, idx_experiment);
    actual_bin = output(:, 1);
    bayesian_bin = output(:, 2);
    viterbi_bin = output(:, 3);
    nnet_bin = output(:, 4);
    num_data = size(output, 1);
    num_pos_data = sum(actual_bin == 1);  % class 1 is positive
    num_neg_data = sum(actual_bin == 2);  % class 2 is negative

    error_rate(idx_experiment, :) = [ ...
                    sum((bayesian_bin ~= actual_bin)) / num_data, ...
                    sum((viterbi_bin ~= actual_bin)) / num_data, ...
                    sum((nnet_bin ~= actual_bin)) / num_data ];

    accuracy(idx_experiment, :) = [...
                    sum((bayesian_bin == actual_bin)) / num_data, ...
                    sum((viterbi_bin == actual_bin)) / num_data, ...
                    sum((nnet_bin == actual_bin)) / num_data ];

    true_positive(idx_experiment, :) = [...
                    sum((bayesian_bin == 1).*(actual_bin == 1))/num_pos_data, ...
                    sum((viterbi_bin == 1).*(actual_bin == 1))/num_pos_data, ...
                    sum((nnet_bin == 1).*(actual_bin == 1))/num_pos_data ];

    false_negative(idx_experiment, :) = [...
                    sum((bayesian_bin == 2).*(actual_bin == 1))/num_pos_data, ...
                    sum((viterbi_bin == 2).*(actual_bin == 1))/num_pos_data, ...
                    sum((nnet_bin == 2).*(actual_bin == 1))/num_pos_data ];

    true_negative(idx_experiment, :) = [...
                    sum((bayesian_bin == 2).*(actual_bin == 2))/num_neg_data, ...
                    sum((viterbi_bin == 2).*(actual_bin == 2))/num_neg_data, ...
                    sum((nnet_bin == 2).*(actual_bin == 2))/num_neg_data ];

    false_positive(idx_experiment, :) = [...
                    sum((bayesian_bin == 1).*(actual_bin == 2))/num_neg_data, ...
                    sum((viterbi_bin == 1).*(actual_bin == 2))/num_neg_data, ...
                    sum((nnet_bin == 1).*(actual_bin == 2))/num_neg_data ];
end
sensitivity = true_positive;
specificity = true_negative;

mean_error_rate = mean(error_rate);
mean_accuracy = mean(accuracy);
mean_sensitivity = mean(sensitivity);
mean_specificity = mean(specificity);
mean_true_positive = mean(true_positive);
mean_false_negative = mean(false_negative);
mean_true_negative = mean(true_negative);
mean_false_positive = mean(false_positive);
var_error_rate = var(error_rate);
var_accuracy = var(accuracy);
var_sensitivity = var(sensitivity);
var_specificity = var(specificity);
var_true_positive = var(true_positive);
var_false_negative = var(false_negative);
var_true_negative = var(true_negative);
var_false_positive = var(false_positive);

disp(sprintf('statistics for : %s', filename));
disp(sprintf('\t                      \tBayesian\tViterbi  \tNNet'));
disp(sprintf('\tmean_error_rate:      \t%g\t%g\t%g', mean_error_rate));
disp(sprintf('\tmean_accuracy:        \t%g\t%g\t%g', mean_accuracy));
disp(sprintf('\tmean_sensitivity:     \t%g\t%g\t%g', mean_sensitivity));
disp(sprintf('\tmean_specificity:     \t%g\t%g\t%g', mean_specificity));
disp(sprintf('\tmean_true_positive:   \t%g\t%g\t%g', mean_true_positive));
disp(sprintf('\tmean_false_negative:  \t%g\t%g\t%g', mean_false_negative));
disp(sprintf('\tmean_true_negative:   \t%g\t%g\t%g', mean_true_negative));
disp(sprintf('\tmean_false_positive:  \t%g\t%g\t%g', mean_false_positive));
disp(sprintf('\n'));
% disp(sprintf('\tvar_error_rate:      \t%g\t%g\t%g', var_error_rate));
% disp(sprintf('\tvar_accuracy:        \t%g\t%g\t%g', var_accuracy));
% disp(sprintf('\tvar_sensitivity:     \t%g\t%g\t%g', var_sensitivity));
% disp(sprintf('\tvar_specificity:     \t%g\t%g\t%g', var_specificity));
% disp(sprintf('\tvar_true_positive:   \t%g\t%g\t%g', var_true_positive));
% disp(sprintf('\tvar_false_negative:  \t%g\t%g\t%g', var_false_negative));
% disp(sprintf('\tvar_true_negative:   \t%g\t%g\t%g', var_true_negative));
% disp(sprintf('\tvar_false_positive:  \t%g\t%g\t%g', var_false_positive));
% disp(sprintf('\n'));

x = 1:NUM_EXPERIMENT;
figure;
subplot(3, 1, 1); title(filename);
hold on;
plot(x, accuracy(:, 1), 'r');
plot(x, accuracy(:, 2), 'g');
plot(x, accuracy(:, 3), 'b');
xlabel('Experiment'); ylabel('Accuracy');
legend('Bayesian', 'Viterbi', 'NNet');
hold off;

subplot(3, 1, 2); title(filename);
hold on;
plot(x, true_positive(:, 1), 'r'); xlabel('experiment');
plot(x, true_positive(:, 2), 'g'); xlabel('experiment');
plot(x, true_positive(:, 3), 'b'); xlabel('experiment');
xlabel('Experiment'); ylabel('True Positive');
legend('Bayesian', 'Viterbi', 'NNet');
hold off;

subplot(3, 1, 3); title(filename);
hold on;
plot(x, true_negative(:, 1), 'r');
plot(x, true_negative(:, 2), 'g');
plot(x, true_negative(:, 3), 'b');
xlabel('Experiment'); ylabel('True Negative');
legend('Bayesian', 'Viterbi', 'NNet');
hold off;


