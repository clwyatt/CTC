function test_target = CTC_nnet_model(train_data, train_target, test_data)
% CTC_NNET_MODEL Neural Network classifier.
% 
% Using Neural Network Toolbox in Matlab to classify test data.
% 
% Input:
%  train_data - m patterns with each has d features. [m x d matrix]
%  train_target - 1 for the first class, 2 for the second class. Current 
%                 implementation support two classes only. [m x 1 vector]
%  test_data - n patterns with each has d features. [n x d matrix]
%
% Output:
%  test_target - 1 for the first class, 2 for the sceond class. [n x 1
%                vector]

train_target(train_target == 2) = -1;

NUM_NEURONS = [3, 3, 1];
TRANSFER_FUNC = 'tansig';
TRAIN_FUNC = 'trainlm';
NUM_EPOCH = 1000;

% In order to avoid overtraining, Bayesian Regularization method is used 
% (see Neural Network Toolbox -> Backpropagation -> Improve
% Generalization).

%train_data = mapminmax(train_data')';
net = newff(minmax(train_data'), NUM_NEURONS, ... 
    {TRANSFER_FUNC, TRANSFER_FUNC, 'purelin'}, TRAIN_FUNC);
net.trainParam.epochs = NUM_EPOCH;
net.trainParam.show = Inf;
[net, tr, y, e] = train(net, train_data', train_target');

%test_target = sim(net, mapminmax(gen_samps'));
test_target = sim(net, test_data');
test_target(test_target >= 0) = 1;
test_target(test_target < 0) = 2;
test_target = test_target';

return;

