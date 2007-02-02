function [center, class] = cmeans(data, cluster_n, options)
% CMEANS Find clusters with c-means clustering.
% c-means clustering.
%   Synopsis
%   [center,obj_fcn] = fcm(data,cluster_n)
%   Description
%   [center, obj_fcn] = fcm(data, cluster_n) applies the c-means
%   clustering method to a given data set.
%   The input arguments of this function are:
%   data: data set to be clustered; each row is a sample data point
%   cluster_n: number of clusters (greater than one)
%   The output arguments of this function are:
%   center: final cluster centers, where each row is a center
%   obj_fcn: values of the objective function during iterations
%   cmeans(data,cluster_n,options) uses an additional argument variable, options,
%   to control clustering parameters, introduce a stopping criteria, and/or set
%   the iteration information display:
%   options(1): exponent for the partition matrix U (default: 2.0)
%   options(2): maximum number of iterations (default: 1000)
%   options(3): minimum amount of improvement (default: 1e-5)
%   options(4): info display during iteration (default: 0)
%   If any entry of options is NaN, the default value for that option is used
%   instead. The clustering process stops when the maximum number of iteration
%   is reached, or when the objective function improvement between two
%   consecutive iteration is less than the minimum amount of improvement
%   specified.
%   Example
%   data = rand(100, 2);
%   [center,U,obj_fcn] = fcm(data, 2);
%   plot(data(:,1), data(:,2),'o');
%   maxU = max(U);
%   index1 = find(U(1,:) == maxU);
%   index2 = find(U(2, :) == maxU);
%   line(data(index1,1),data(index1,2), ...
%   	'marker','*','color','g');
%   line(data(index2,1),data(index2,2), ...
%   	'marker','*','color','r');
%


if nargin ~= 2 & nargin ~= 3,
	error('Too many or too few input arguments!');
end

data_n = size(data, 1);
in_n = size(data, 2);

% Change the following to set default options
default_options = [2;	% exponent for the partition matrix U
		1000;	% max. number of iteration
		1e-6;	% min. amount of improvement
		0];	% info display during iteration 

if nargin == 2,
	options = default_options;
else
	% If "options" is not fully specified, pad it with default values.
	if length(options) < 4,
		tmp = default_options;
		tmp(1:length(options)) = options;
		options = tmp;
	end
	% If some entries of "options" are nan's, replace them with defaults.
	nan_index = find(isnan(options)==1);
	options(nan_index) = default_options(nan_index);
	if options(1) <= 1,
		error('The exponent should be greater than 1!');
	end
end

expo = options(1);		% Exponent for U
max_iter = options(2);		% Max. iteration
min_impro = options(3);		% Min. improvement
display = options(4);		% Display info or not

%  Randomly generate initial fuzzy partition matrix for fuzzy c-means 
%  clustering, that is CLUSTER_N by DATA_N, where CLUSTER_N is number of
%  clusters and DATA_N is number of data points. The summation of each
%  column of the generated U is equal to unity, as required by fuzzy
%  c-means clustering.
%

U = ceil(3*rand(data_n,1));
oldcenter= zeros(cluster_n,in_n);
for i=1:cluster_n
   oldcenter(i,:) = mean(data(U==i,:));
end;


% Main loop
done = 0;
k=0;
while not(done),
   dist = distfcm(oldcenter, data);       % fill the distance matrix
   [mindist,minind] = min(dist,[],1);
   newcenter= zeros(cluster_n,in_n);
   for i=1:cluster_n
       newcenter(i,:) = mean(data(minind==i,:)); % new center
   end;
   
    if sum(sqrt((newcenter - oldcenter).^2)) < 10^(-6) | (k > max_iter)      
        center = newcenter;
        done = 1;
    else
       oldcenter = newcenter;
    end;   
    class = minind';
    k = k+1;
end

