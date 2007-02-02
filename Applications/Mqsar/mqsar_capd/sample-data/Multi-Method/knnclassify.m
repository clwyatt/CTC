function class = knnclassify(sample, TRAIN, group, K, distance,tieBreaker)
%KNNCLASSIFY K-Nearest Neighbor Classifier.
%   CLASS = KNNCLASSIFY(SAMPLE,TRAINING,GROUP) classifies each row of the
%   data in SAMPLE into one of the groups in TRAINING using the nearest
%   neighbour method.  SAMPLE and TRAINING must be matrices with the same
%   number of columns.  GROUP is a grouping variable for TRAINING.  Its
%   unique values define groups, and each element defines the group to
%   which the corresponding row of TRAINING belongs.  GROUP can be a
%   numeric vector, a string array, or a cell array of strings.  TRAINING
%   and GROUP must have the same number of rows.  CLASSIFY treats NaNs or
%   empty strings in GROUP as missing values, and ignores the corresponding
%   rows of TRAINING. CLASS indicates which group each row of SAMPLE has
%   been assigned to, and is of the same type as GROUP.
%
%   CLASS = KNNCLASSIFY(SAMPLE,TRAINING,GROUP,K) allows you to specify K,
%   the number of nearest neighbors used in the classification. The default
%   is 1.
%
%   CLASS = KNNCLASSIFY(SAMPLE,TRAINING,GROUP,K,DISTANCE) allows you to
%   select the distance metric. Choices are:
%            {'sqEuclidean'} - Squared Euclidean distance
%             'cityblock'    - Sum of absolute differences, a.k.a. L1
%             'cosine'       - One minus the cosine of the included angle
%                              between points (treated as vectors)
%             'correlation'  - One minus the sample correlation between
%                              points (treated as sequences of values)
%             'Hamming'      - Percentage of bits that differ (only
%                              suitable for binary data)
%
%   CLASS = KNNCLASSIFY(SAMPLE,TRAINING,GROUP,K,DISTANCE,TIEBREAK) allows
%   you to specify the method used to break a tie in the number of nearest
%   neighbors. Options are 'random', which selects a random tiebreaker, and
%   'nearest' which uses the nearest neighbor amongst the tied groups to
%   break the tie.
%
%   Examples:
%
%      % training data: two normal components
%      training = [mvnrnd([ 1  1],   eye(2), 100); ...
%                  mvnrnd([-1 -1], 2*eye(2), 100)];
%      group = [repmat(1,100,1); repmat(2,100,1)];
%      gscatter(training(:,1),training(:,2),group);hold on;
%       
%
%      % some random sample data
%      sample = unifrnd(-5, 5, 100, 2);
%       % classify the sample using the nearest neighbor classification
%      c = knnclassify(sample, training, group);
%
%      gscatter(sample(:,1),sample(:,2),c,'mc'); hold on;
%       c3 = knnclassify(sample, training, group, 3);
%      gscatter(sample(:,1),sample(:,2),c3,'mc','o'); 

%   Copyright 1993-2002 The MathWorks, Inc. 
%   $Revision: $  $Date: 2002/04/11 14:24:07 $

%   References:
%     [1] 

if nargin < 3
    error('Requires at least three arguments.');
end

% grp2idx sorts a numeric grouping var ascending, and a string grouping
% var by order of first occurrence
[gindex,groups] = grp2idx(group);
nans = find(isnan(gindex));
if length(nans) > 0
    TRAIN(nans,:) = [];
    gindex(nans) = [];
end
ngroups = length(groups);
gsize = hist(gindex,1:ngroups);

[n,d] = size(TRAIN);
if size(gindex,1) ~= n
    error('The length of GROUP must equal the number of rows in TRAINING.');
elseif size(sample,2) ~= d
    error('SAMPLE and TRAINING must have the same number of columns.');
end
m = size(sample,1);

if nargin < 4
    K = 1;
elseif ~isnumeric(K)
    error('K must be numeric.');
end

if nargin < 5 || isempty(distance)
    distance  = 'sqeuclidean';
end

if ischar(distance) 
    distNames = {'sqeuclidean','cityblock','cosine','correlation','hamming'};
    i = strmatch(lower(distance), distNames);
    if length(i) > 1
        error('stats:knn:AmbiguousDistance', ...
            'Ambiguous ''distance'' parameter value:  %s.', distance);
    elseif isempty(i)
        error('stats:knn:UnknownDistance', ...
            'Unknown ''distance'' parameter value:  %s.', distance);
    end
    distance = distNames{i};
else
    error('stats:knn:InvalidDistance', ...
        'The ''distance'' parameter value must be a string.');
end

if nargin < 6
    tieBreaker = 'random';
elseif ischar(tieBreaker)
    %     if K == 1
    %         warning('Tie breaker is not necessary when K is 1.');
    %     else
    tieNames = {'random','nearest','farthest'};
    i = strmatch(lower(tieBreaker), tieNames);
    if length(i) > 1
        error('stats:knn:AmbiguousDistance', ...
            'Ambiguous ''distance'' parameter value:  %s.', distance);
    elseif isempty(i)
        error('stats:knn:UnknownDistance', ...
            'Unknown ''distance'' parameter value:  %s.', distance);
    end
    tieBreaker = tieNames{i};
    %     end
else
    error('stats:knn:InvalidDistance', ...
        'The ''distance'' parameter value must be a string.');
end

% Calculate the distances from all points in the training set to all points
% in the test set.

dists = distfun(sample,TRAIN,distance);

% find the K nearest 

if K >1
    [dSorted,dIndex] = sort(dists,2);
    dIndex = dIndex(:,1:K);
    classes = gindex(dIndex);
    % special case when we have one input -- this gets turned into a   
    % column vector, so we have to turn it back into a row vector.
    if size(classes,2) == 1
        classes = classes';
    end
    % count the occurences of the classes
    
    counts = zeros(m,ngroups);
    for outer = 1:m
        for inner = 1:K
            counts(outer,classes(outer,inner)) = counts(outer,classes(outer,inner)) + 1;
        end
    end
    
    [L,class] = max(counts,[],2);
    
    % we need to check case where L <= K/2 for possible ties
    
    checkRows = find(L<=(K/2));
    
    for i = 1:numel(checkRows)
        ties = counts(checkRows(i),:) == L(checkRows(i));
        numTies = sum(ties);
        if numTies > 1
            choice = find(ties);
            switch tieBreaker
                case 'random'
                    % random tie break 
                    
                    tb = randsample(numTies,1);
                    class(checkRows(i)) = choice(tb);
                case 'nearest'
                    % find the use the closest element of the equal groups
                    % to break the tie
                    for inner = 1:K
                        if ismember(classes(checkRows(i),inner),find(ties))
                            class(checkRows(i)) = classes(checkRows(i),inner);
                            break
                        end
                    end
                case 'farthest'
                    % find the use the closest element of the equal groups
                    % to break the tie
                    for inner = K:-1:1
                        if ismember(classes(checkRows(i),inner),find(ties))
                            class(checkRows(i)) = classes(checkRows(i),inner);
                            break
                        end
                    end
            end
        end
    end
    
else
    % Need to deal with a tie
    [dSorted,dIndex] = min(dists,[],2);
    class = gindex(dIndex);
end

% Convert back to original grouping variable
if isnumeric(group)
    groups = str2num(char(groups));
    class = groups(class);
elseif ischar(group)
    groups = char(groups);
    class = groups(class,:);
else %if iscellstr(group)
    class = groups(class);
end


function D = distfun(Train, Test, dist)
%DISTFUN Calculate distances from training points to test points.
[n,p] = size(Train);
D = zeros(n,size(Test,1));
numTest = size(Test,1);

switch dist
    case 'sqeuclidean'
        for i = 1:numTest
            D(:,i) = sum((Train - Test(repmat(i,n,1),:)).^2, 2);
        end
    case 'cityblock'
        for i = 1:numTest
            D(:,i) = sum(abs(Train - Test(repmat(i,n,1),:)), 2);
        end
    case {'cosine','correlation'}
        % Normalized both the training and test data.
        normTrain = sqrt(sum(Train.^2, 2));
        normTest = sqrt(sum(Test.^2, 2));
        normData = sqrt(sum([Train;Test].^2, 2));
        Train = Train ./ normTrain(:,ones(1,size(Train,2)));
        if any(normData < eps) % small relative to unit-length data points
            error('stats:knn:ZeroTestentroid', ...
                'Zero cluster centroid created at iteration %d.',iter);
        end
        % This can be done without a loop, but the loop saves memory allocations
        for i = 1:numTest
            D(:,i) = 1 - (Train * Test(i,:)') ./ normTest(i);
        end
    case 'hamming'
        if ~all(ismember(X(:),[0 1]))
            error('Non-binary data cannot be clustered using Hamming distance.');
        end
        for i = 1:numTest
            D(:,i) = sum(abs(Train - Test(repmat(i,n,1),:)), 2) / p;
        end
end