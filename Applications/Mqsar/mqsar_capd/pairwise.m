function [dist] = pairwise(data, varargin)

% DISTANCE calculate the pairwise distances under a specify type
% 
% [dist] = distance(point1, point2, 'hamming')
%
%	Input and output arguments ({}'s are optional);
%	data:	matrix:	the coordinates
%
%	dist:	arrary:	n X (n-1)/2 pairwise distance values
%
% Valid argument IDs and corresponding values. Values which are
% unambiguous (marked with '*') can be given without the preceeding argID.
%
%	type	*(string):	specify which type of distance will be calculated, the default
%			distance type is 'Euclidean', the options are 'Hamming', 'Euclidean',
%			'Soergel', 'Tanimoto', 'Dice', and 'Cosine'

%
% Copyright 2003, Yun-De Xiao

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% default type
type = 'Euclidean';

%% Check arguments
i=1;
while i<=size(varargin, 2),
	argok = 1;
	if ischar(varargin{i}),
		switch varargin{i},
			% unambiguous values
			case {'Hamming', 'Euclidean', 'Soergel', 'Tanimoto', 'Dice', 'Cosine'},
				type = varargin{i};
			otherwise argok = 0;
		end
	end
	if ~argok,
		disp(['(distance) Ignoring invalid argument: #' num2str(i)]);
	end
	i = i + 1;
end

dist = zeros(size(data, 1)*(size(data, 1) - 1)/2, 1);
nCount = 1;
for i=1:size(data, 1)
    point1 = data(i, :);
    for j=i+1:size(data, 1)
        point2 = data(j, :);
        dist(nCount) = distance(point1, point2, type);
        nCount = nCount + 1;
    end
end