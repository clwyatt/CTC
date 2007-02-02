function D = distfun2(Train, Test, dist)
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
    case {'cosine'}
        normTrain = sqrt(sum(Train.^2, 2));
        normTest = sqrt(sum(Test.^2, 2));
        % This can be done without a loop, but the loop saves memory allocations
        for i = 1:numTest
            D(:,i) = (Train*Test(i,:)')./(normTest(i)*normTrain);
        end;
    case {'correlation'}
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