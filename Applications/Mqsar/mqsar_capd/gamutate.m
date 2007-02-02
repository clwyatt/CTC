function [Offspring, SameFlag] = gamutate(Parent, pM, Mutations);		

% gamutate Mutate the chromosomes based with a probability of pM 
%
% [Offspring, SameFlag] = ga_mutate(Parent, pM, Mutations)  
%
%  Input and output arguments ([]'s are optional): 
%  Parent    (struct)  chromosome struct
%  pM        (scalar)  probability of mutations
%  Mutations (scalar)  number of mutation points 
%
%  Offspring  Binary mask file for the child
%  SameFlag    Sets to 1 if the childs are same as parents. If not it is 0
%  
%
% Ersin Bayram, September 2003
% Targacept, Inc. http://www.targacept.com

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% check arguments


% Parent1
ChromSize = length(Parent);

SameFlag = 0;

Offspring = Parent;
Change = 0;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Randomly select Mutations number of mutations points

if (Mutations >=1)    
    RandInd = randperm(ChromSize);
    Ind_Mutate = sort(RandInd(1:Mutations));
    % Perform the crossover operation
    for i = 1:Mutations
        if (rand(1) <= pM)
            Offspring(Ind_Mutate(i)) = not(Parent(Ind_Mutate(i)));
            Change = 1;    
        end;      
    end;
else
    SameFlag = 1;
end;        

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% If no change on parents then choose parent to copy
if (Change == 0)
    SameFlag = 1;
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
