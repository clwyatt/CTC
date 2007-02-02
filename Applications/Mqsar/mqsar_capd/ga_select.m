function [index] = ga_select(fitness, NumSelects, type);		

%GA_SELECT Given population fitnesses, selects one member
%
% parentindice = ga_select(fitness, 'type','roulette') 
% parentindice = ga_select(fitness, 'tournament') 
% 
% fitness     (vector)  n x 1 vector of chromosome fitnesses
% 
% NumSelects  (scalar) number of chromosomes to be selected as parents for crossover 
%
% type     *(string)  type of selection:
%                           'roulette'   - fitness proportionate (default)
%                           'random'     - fitness independent
%                           'tournament' - fitness based not implemented yet
%                           'rank'       - not implemented yet
%
% Ersin Bayram September, 2003
% Targacept, Inc. http://www.targacept.com

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% check arguments

Dim = length(fitness);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% choose selection method, return index of selected chromosome

switch type
 case 'roulette'
     
    fitnessLUT = cumsum(fitness);    
    token  = sort(fitnessLUT(end)*rand(NumSelects,1));
    for i =1:NumSelects
        target = find(floor(fitnessLUT(:) / token(i)));
        index(i) = target(1);
    end;
 case 'random'

    index = ceil(Dim*rand(NumSelects,1));
 otherwise
    display('Slection type not recognized or not implemented yet');
    return;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
