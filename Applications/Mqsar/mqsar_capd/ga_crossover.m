function [Offspring1, Offspring2, SameFlag] = ga_crossover(Parent1, Parent2, pC, varargin);		

% GA_CROSSOVER Crossover two parent chromosomes to generatetwo childs from
% two parents
%
% [Offspring1, Offspring2, SameFlag] = ga_crossover(Parent1, Parent2, pC, 'uniform') 
% [Offspring1, Offspring2, SameFlag] = ga_crossover(Parent1, Parent2, 0.7,'point', 4)  
%
%  Input and output arguments ([]'s are optional): 
%  Parent1   (struct)  chromosome struct
%  Parent2   (struct)  chromosome struct
%  pC        (scalar)  probability of crossing parents
%  [argID,   (string)  See below. The values which are unambiguous can 
%    value]  (varies)  be given without the preceeding argID.
%
%  Offspring1  Binary mask file for the first child
%  Offspring2  Binary mask file for the second child
%  SameFlag    Sets to 1 if the childs are same as parents. If not it is 0
%
%  Valid argument IDs and corresponding values. 
%
%  'uniform'  *(string)  uniform type of crossover
%  'point'    Indicates a point based crossover and followd by the 
%             the number of crossover points.    
%
% Ersin Bayram, September 2003
% Targacept, Inc. http://www.targacept.com

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% check arguments

rand('state',sum(100*clock));	%re-seed random number generator

% Parent1
ChromSize = length(Parent1);


% defaults
type = 'point';
Points = 2;
SameFlag = 0;

% varargin
i=1; 
while i<=size(varargin,1), 
  argok = 1; 
  if ischar(varargin{i}), 
    switch varargin{i}, 
     case 'point', type = varargin{i}; i=i+1; Points = varargin{i}; 
     case {'uniform'}, type = varargin{i};
     otherwise argok=0; 
    end
  end
  if ~argok, 
    disp(['(ga_crossover) Ignoring invalid argument: #' num2str(i+1)]); 
  end
  i = i+1; 
end

Offspring1 = Parent1;
Offspring2 = Parent2;
Change = 0;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% select crossover method and build make crossover mask

% If we are performing the crossover
switch type
case 'point'
	if (Points >=1)    
        RandInd = randperm(ChromSize-1);
        Pivots = sort(RandInd(1:Points)+1);
        
        for i = 1:2:Points-1
            if (rand(1) <= pC)
                Offspring1(Pivots(i):Pivots(i+1)-1) = Parent2(Pivots(i):Pivots(i+1)-1);
                Offspring2(Pivots(i):Pivots(i+1)-1) = Parent1(Pivots(i):Pivots(i+1)-1);
                Change = 1;    
            end;    
       
        end;
        
        % If number of crossover points is odd, then handle the last crossover 
        % at the end of the chromosomes seperately
        if (rand(1) <= pC) & (mod(Points,2)==1)
            Offspring1(Pivots(Points):end) = Parent2(Pivots(Points):end);
            Offspring2(Pivots(Points):end) = Parent1(Pivots(Points):end);
            Change = 1;
        end;  
        
	else
        SameFlag = 1;
	end;        
case 'uniform'
     
	if (rand(1) <= pC)
        RandInd = round(rand(ChromSize,1));
        Offspring1(RandInd == 1) = Parent2(RandInd == 1);
        Offspring2(RandInd == 1) = Parent1(RandInd == 1);
	else
       SameFlag = 1;  
	end;

end;


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% If no change on parents then choose parent to copy

if (Change == 0)
    SameFlag = 1;
end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
