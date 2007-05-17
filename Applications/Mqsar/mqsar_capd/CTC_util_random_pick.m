function data_class = CTC_util_random_pick(p, n)
% UTIL_RANDOM_PIC Randomly pick data according to pre-defined probability.
% 
% example: p = [0.3, 0.2, 0.5]' and n = 10, you wish to get: three A, two
% B, and five C in random order, i.e., (C C B A B C A C C A). 
% 
% parameter
%   p - probability of each class [vector]
%   n - total number of sample data [scalar]
% 
% return
%   data class. [vector]


% sum(p) should equal to 1
p = p/sum(p);
p = cumsum(p);
data_class = zeros(n ,1);
temp = rand(n, 1);
for idx = 1:n
    data_class(idx) = find(p >= temp(idx), 1, 'first');
end
