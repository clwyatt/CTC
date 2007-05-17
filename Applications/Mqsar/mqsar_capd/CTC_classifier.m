%function CTC_classifier(file_train, file_test, file_output)
function CTC_classifier()
% CTC_CLASSIFIER CTC classifier.
% 
% Input:
%  file_train - filename contains train data.
%  file_test - filename contains test data.
%  file_output - filename for output.
%
% Note:
%  For the data format in both train and test files, see comments in 
%  function CTC_dataimport.

file_train = './sample-data/CTC-data/train.dat';
file_test = './sample-data/CTC-data/test.dat';
file_output = './sample-data/CTC-data/ensemble_1000_training.xls';

[rownames, colnames, xdata, ydata, ynames, bins, maskmat] = ...
    CTC_dataimport(file_train, file_test);

for idx = 1:50
    maskmat = CTC_util_random_pick([0.8, 0.2], numel(maskmat));

    train_data = xdata(maskmat == 1, :);
    train_target = bins(maskmat == 1);
    test_data = xdata(maskmat == 2, :);
    test_target_true = bins(maskmat == 2);
    
    % subsample to about 1000 train data
    train_data = train_data(1:15:end, :);
    train_target = train_target(1:15:end, :);

    % classifying
    test_target_nnet = CTC_nnet_model(train_data, train_target, test_data);
    
    KNN_NEAREST = 3;
    test_target_knn = CTC_knn_model(train_data, train_target, test_data, KNN_NEAREST);
    
    DISC_FUNC_TYPE = 'quadratic';
    test_target_disc_func = CTC_discriminant_func_model(train_data, train_target, test_data, DISC_FUNC_TYPE);
    
    test_target_ensemble = CTC_ensemble_vote(train_data, train_target, test_data, ...
        [test_target_nnet, test_target_knn, test_target_disc_func]);

    % statistic analysis
    total = numel(test_target_true);
    acc_ensemble = sum(test_target_ensemble == test_target_true) / total;
    err_ensemble = 1 - acc_ensemble;
    tp_ensemble = sum((test_target_ensemble == 1).*(test_target_true == 1)) / sum(test_target_true == 1);
    tn_ensemble = sum((test_target_ensemble == 2).*(test_target_true == 2)) / sum(test_target_true == 2);
    ppv_ensemble = sum((test_target_ensemble == 1).*(test_target_true == 1)) / sum(test_target_ensemble == 1);
    npv_ensemble = sum((test_target_ensemble == 2).*(test_target_true == 2)) / sum(test_target_ensemble == 2);

    acc_nnet = sum(test_target_nnet == test_target_true) / total;
    err_nnet = 1 - acc_nnet;
    tp_nnet = sum((test_target_nnet == 1).*(test_target_true == 1)) / sum(test_target_true == 1);
    tn_nnet = sum((test_target_nnet == 2).*(test_target_true == 2)) / sum(test_target_true == 2);
    ppv_nnet = sum((test_target_nnet == 1).*(test_target_true == 1)) / sum(test_target_nnet == 1);
    npv_nnet = sum((test_target_nnet == 2).*(test_target_true == 2)) / sum(test_target_nnet == 2);
    
    acc_knn = sum(test_target_knn == test_target_true) / total;
    err_knn = 1 - acc_knn;
    tp_knn = sum((test_target_knn == 1).*(test_target_true == 1)) / sum(test_target_true == 1);
    tn_knn = sum((test_target_knn == 2).*(test_target_true == 2)) / sum(test_target_true == 2);
    ppv_knn = sum((test_target_knn == 1).*(test_target_true == 1)) / sum(test_target_knn == 1);
    npv_knn = sum((test_target_knn == 2).*(test_target_true == 2)) / sum(test_target_knn == 2);

    acc_disc_func = sum(test_target_disc_func == test_target_true) / total;
    err_disc_func = 1 - acc_disc_func;
    tp_disc_func = sum((test_target_disc_func == 1).*(test_target_true == 1)) / sum(test_target_true == 1);
    tn_disc_func = sum((test_target_disc_func == 2).*(test_target_true == 2)) / sum(test_target_true == 2);
    ppv_disc_func = sum((test_target_disc_func == 1).*(test_target_true == 1)) / sum(test_target_disc_func == 1);
    npv_disc_func = sum((test_target_disc_func == 2).*(test_target_true == 2)) / sum(test_target_disc_func == 2);

    xlswrite(file_output, {' ', 'sensitivity(TP)', 'specificity(TN)', 'PPV', 'NPV', 'FP', 'FN', 'accuracy', 'error-rate','test no.'}, idx);
    xlswrite(file_output, {'nnet', tp_nnet, tn_nnet, ppv_nnet, npv_nnet, 1 - tn_nnet, 1 - tp_nnet, acc_nnet, err_nnet, total}, idx, 'A2');
    xlswrite(file_output, {'knn', tp_knn, tn_knn, ppv_knn, npv_knn, 1 - tn_knn, 1 - tp_knn, acc_knn, err_knn, total}, idx, 'A3');
    xlswrite(file_output, {'disc_func', tp_disc_func, tn_disc_func, ppv_disc_func, npv_disc_func, 1 - tn_disc_func, 1 - tp_disc_func, acc_disc_func, err_disc_func, total}, idx, 'A4');
    xlswrite(file_output, {'ensemble', tp_ensemble, tn_ensemble, ppv_ensemble, npv_ensemble, 1 - tn_ensemble, 1 - tp_ensemble, acc_ensemble, err_ensemble, total}, idx, 'A5');

    % confusion matrix
    xlswrite(file_output, {'NNET', 'actual 1', 'actual 2'}, idx, 'A8');
    xlswrite(file_output, {'test outcome 1'; 'test outcome 2'}, idx, 'A9');
    conf_mat_11 = sum((test_target_nnet == 1).*(test_target_true == 1));
    conf_mat_12 = sum((test_target_nnet == 1).*(test_target_true == 2));
    conf_mat_21 = sum((test_target_nnet == 2).*(test_target_true == 1));
    conf_mat_22 = sum((test_target_nnet == 2).*(test_target_true == 2));
    xlswrite(file_output, [conf_mat_11, conf_mat_12; conf_mat_21, conf_mat_22], idx, 'B9');

    xlswrite(file_output, {'KNN', 'actual 1', 'actual 2'}, idx, 'F8');
    xlswrite(file_output, {'test outcome 1'; 'test outcome 2'}, idx, 'F9');
    conf_mat_11 = sum((test_target_knn == 1).*(test_target_true == 1));
    conf_mat_12 = sum((test_target_knn == 1).*(test_target_true == 2));
    conf_mat_21 = sum((test_target_knn == 2).*(test_target_true == 1));
    conf_mat_22 = sum((test_target_knn == 2).*(test_target_true == 2));
    xlswrite(file_output, [conf_mat_11, conf_mat_12; conf_mat_21, conf_mat_22], idx, 'G9');

    xlswrite(file_output, {'DISC_FUNC', 'actual 1', 'actual 2'}, idx, 'A12');
    xlswrite(file_output, {'test outcome 1'; 'test outcome 2'}, idx, 'A13');
    conf_mat_11 = sum((test_target_disc_func == 1).*(test_target_true == 1));
    conf_mat_12 = sum((test_target_disc_func == 1).*(test_target_true == 2));
    conf_mat_21 = sum((test_target_disc_func == 2).*(test_target_true == 1));
    conf_mat_22 = sum((test_target_disc_func == 2).*(test_target_true == 2));
    xlswrite(file_output, [conf_mat_11, conf_mat_12; conf_mat_21, conf_mat_22], idx, 'B13');

    xlswrite(file_output, {'ENSEMBLE', 'actual 1', 'actual 2'}, idx, 'F12');
    xlswrite(file_output, {'test outcome 1'; 'test outcome 2'}, idx, 'F13');
    conf_mat_11 = sum((test_target_ensemble == 1).*(test_target_true == 1));
    conf_mat_12 = sum((test_target_ensemble == 1).*(test_target_true == 2));
    conf_mat_21 = sum((test_target_ensemble == 2).*(test_target_true == 1));
    conf_mat_22 = sum((test_target_ensemble == 2).*(test_target_true == 2));
    xlswrite(file_output, [conf_mat_11, conf_mat_12; conf_mat_21, conf_mat_22], idx, 'G13');
    
    
    target_header = {'row', 'target-nnet', 'target-knn', 'target-disc-fucn', ...
                     'target-ensemble', 'target-true'};
    target_data = [test_target_nnet, test_target_knn, test_target_disc_func, ...
                    test_target_ensemble, test_target_true];
    xlswrite(file_output, target_header, idx, 'A20');
    xlswrite(file_output, find(maskmat == 2), idx, 'A21');
    xlswrite(file_output, target_data, idx, 'B21');
    test_targets{idx} = [find(maskmat == 2), test_target_nnet, test_target_knn, test_target_disc_func, test_target_ensemble, test_target_true];
end

save('sample-data/CTC-data/test_targets', 'test_targets');


%%
function knn()

file_train = './sample-data/CTC-data/train.dat';
file_test = './sample-data/CTC-data/test.dat';
file_output = './sample-data/CTC-data/knn_1_2_N.xls';

[rownames, colnames, xdata, ydata, ynames, bins, maskmat] = ...
    CTC_dataimport(file_train, file_test);

maskmat = CTC_util_random_pick([0.8, 0.2], numel(maskmat));
train_data = xdata(maskmat == 1, :);
train_target = bins(maskmat == 1);
test_data = xdata(maskmat == 2, :);
test_target_true = bins(maskmat == 2);

% subsample to about 1000 train data
% train_data = train_data(1:15:end, :);
% train_target = train_target(1:15:end, :);

N = 51;
for knn_nearest = 1:2:N
    % classifying
    test_target_knn = CTC_knn_model(train_data, train_target, test_data, knn_nearest);

    % statistic analysis
    total = numel(test_target_true);
    idx = ceil(knn_nearest/2);
    acc_knn(idx) = sum(test_target_knn == test_target_true) / total;
    err_knn(idx) = 1 - acc_knn(idx);
    tp_knn(idx) = sum((test_target_knn == 1).*(test_target_true == 1)) / sum(test_target_true == 1);
    tn_knn(idx) = sum((test_target_knn == 2).*(test_target_true == 2)) / sum(test_target_true == 2);
    ppv_knn(idx) = sum((test_target_knn == 1).*(test_target_true == 1)) / sum(test_target_knn == 1);
    npv_knn(idx) = sum((test_target_knn == 2).*(test_target_true == 2)) / sum(test_target_knn == 2);
end
xlswrite(file_output, {'N', 'sensitivity(TP)', 'specificity(TN)', 'PPV', 'NPV', 'FP', 'FN', 'accuracy', 'error-rate'}, 1);
xlswrite(file_output, [(1:2:N)', tp_knn', tn_knn', ppv_knn', npv_knn', 1 - tn_knn', 1 - tp_knn', acc_knn', err_knn'], 1, 'A2');

figure; hold;
x = 1:2:N;
plot(x, acc_knn, 'r');
plot(x, tp_knn, 'g');
plot(x, tn_knn, 'b');
plot(x, ppv_knn, 'm');
plot(x, npv_knn, 'k');
legend('accuracy', 'sensitivity', 'specificity', 'ppv', 'npv');
hold off;

return;
