function test_target = ...
    CTC_discriminant_func_model(train_data, train_target, test_data, type)
% CTC_DISCRIMINANT_FUNC_MODEL Discriminant function classifier.
% 
% Using discriminant function to classify test data.
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


test_target = classify(test_data, train_data, train_target, type);
