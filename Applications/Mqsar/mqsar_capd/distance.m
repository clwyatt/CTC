function [dist] = distance(point1, point2, varargin)

% DISTANCE calculate the distance between two points under a specify type
% 
% [dist] = distance(point1, point2, 'hamming')
%
%	Input and output arguments ({}'s are optional);
%	point1:	array:	the first point coordinates
%	point2:	array:	the second point coordinates
%
%	dist:	real:	the distance value between these two points
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

dist = 0;
switch char(type)
	case 'Hamming'		% Hamming Distance (range: 0 to infinite)
		dist = Hamming(point1, point2);
	case 'Euclidean'	% Euclidean Distance (range: 0 to infinite) -- default
		dist = Euclidean(point1, point2);
	case 'Soergel'		% Soergel Distance  (range: 0 to 1)
		dist = Soergel(point1, point2);
	case 'Tanimoto'		% Tanimoto Coefficient (range: 0 to 1.333)
		dist = Tanimoto(point1, point2);
	case 'Dice'			% Dice Coefficient (range: 0 to 2)
		dist = Dice(point1, point2);
	case 'Cosine'		% Cosine Coefficient (range: 0 to 2)
		dist = Cosine(point1, point2);
end

% 0 - Hamming Distance (range: 0 to infinite)
% --------------------------------------------
function [dist] = Hamming(point1, point2)
if ~(length(point1) == length(point2))
	disp('the two points is not fit each other');
	return;
end
dist = 0.0;
for nVar=1:length(point1)
	dist = dist + fabs(point1(nVar) - point2(nVar));
end
% ----------------------------------------------

% 1 - Euclidean Distance (range: 0 to infinite) -- default
% --------------------------------------------
function [dist] = Euclidean(point1, point2)
if ~(length(point1) == length(point2))
	disp('the two points is not fit each other');
	return;
end
dist = 0.0;
for nVar=1:length(point1)
	dist = dist + (point1(nVar) - point2(nVar)) * (point1(nVar) - point2(nVar));
end
dist = sqrt(dist);
% --------------------------------------------

% 2 - Soergel Distance  (range: 0 to 1)
% --------------------------------------------
function [dist] = Soergel(point1, point2)
if ~(length(point1) == length(point2))
	disp('the two points is not fit each other');
	return;
end
dist = 0.0;
tmp = 0.0;
for nVar=1:length(point1)
	dist = dist + fabs(point1(nVar) - point2(nVar));
	tmp = tmp + max(point1(nVar), point2(nVar));
end
dist = dist/tmp;
% --------------------------------------------

% 3 - Tanimoto Coefficient (range: 0 to 1.333)
% --------------------------------------------
function [dist] = Tanimoto(point1, point2)
if ~(length(point1) == length(point2))
	disp('the two points is not fit each other');
	return;
end
tmp1 = 0.0;
tmp2 = 0.0;
cross = 0.0;
for nVar=1:length(point1)
	tmp1 = tmp1 + point1(nVar) * point1(nVar);
	tmp2 = tmp2 + point2(nVar) * point2(nVar);
	cross = cross + point1(nVar) * point2(nVar);
end
dist = 1.0 - cross/(tmp1 + tmp2 - cross);
% --------------------------------------------

% 4- Dice Coefficient (range: 0 to 2)
% --------------------------------------------
function [dist] = Dice(point1, point2)
if ~(length(point1) == length(point2))
	disp('the two points is not fit each other');
	return;
end
tmp1 = 0.0;
tmp2 = 0.0;
cross = 0.0;
for nVar=1:length(point1)
	tmp1 = tmp1 + point1(nVar) * point1(nVar);
	tmp2 = tmp2 + point2(nVar) * point2(nVar);
	cross = cross + point1(nVar) * point2(nVar);
end
dist = 1.0 - 2*cross/(tmp1 + tmp2);
% --------------------------------------------

% 5 - Cosine Coefficient (range: 0 to 2)
% --------------------------------------------
function [dist] = Cosine(point1, point2)
if ~(length(point1) == length(point2))
	disp('the two points is not fit each other');
	return;
end
tmp1 = 0.0;
tmp2 = 0.0;
cross = 0.0;
for nVar=1:length(point1)
	tmp1 = tmp1 + point1(nVar) * point1(nVar);
	tmp2 = tmp2 + point2(nVar) * point2(nVar);
	cross = cross + point1(nVar) * point2(nVar);
end
dist = cross/(tmp1 * tmp2);
% --------------------------------------------