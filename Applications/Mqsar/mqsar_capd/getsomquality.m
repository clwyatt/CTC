function [results] = getsomquality(sM, sD, class);
% --------------------------------------------
% Excutes to find the category of data (sD)
for i=1:size(sD.data,1)
	results(i,1) = class(find(som_hits(sM, sD.data(i,:), 'crisp')));
end
