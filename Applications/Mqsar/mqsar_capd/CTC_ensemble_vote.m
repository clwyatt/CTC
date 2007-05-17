function ensemble_target = ...
    CTC_ensemble_vote(train_data, train_target, test_data, test_targets)
% CTC_ENSEMBLE_VOTE Ensemble classification result with vote mechanism.
%
% Using simple vote to ensemble classification result. No method implemented 
% to break tie at this time.
% 
% Input:
%  train_data - m patterns with each has d features. [m x d matrix]
%  train_target - 1 for the first class, 2 for the second class. Current 
%                 implementation support two classes only. [m x 1 vector]
%  test_data - n patterns with each has d features. [n x d matrix]
%  test_targets - n patterns, each column corresponds to the result from 
%                 one component classifier. [n x c matrix]
%
% Output:
%  ensemble_target - 1 for the first class, 2 for the sceond class. [n x 1
%                    vector]

ensemble_target = mode(test_targets, 2);
