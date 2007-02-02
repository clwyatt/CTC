function userdata = initialsetup(userdata);
% Initial global set up parameters

% General setup parameters
userdata.sortgoodtobad = 0;

% Data I/O related settings
userdata.corrfilt = 1.0;
userdata.stdfilt  = 0.0;
userdata.scaling = 'None';
userdata.metric = 'Euclidian';
userdata.validation = 'None';
userdata.DescLimit = 'None';
userdata.Descriptors = 10;
userdata.percentout = 20;
userdata.nfold = 20;

% Variable Selection method
% GA related parameters   
userdata.MaxCrossOver = 10;
userdata.MinCrossOver = 0;
userdata.MaxMutate = 10;
userdata.MinMutate = 2;
userdata.Pmutation = 0.2;
userdata.Pcross = 0.7;
userdata.SubsetPopulation = 100;
userdata.Subsets = 1;
userdata.Generation = 400;
userdata.Elites = 10;
userdata.PoolDur = 1;
userdata.PoolFreq = 400;
userdata.GeneLife = 20;

% SA related parameters    
userdata.StartT = 100;
userdata.EndT = 1.0e-6;
userdata.Droprate = 0.9;
userdata.Cycle = 20;
userdata.Mutate = 2;

% Hill Climbing Related
userdata.HGeneration = 400;
userdata.Hcycle = 40;
userdata.Mutate = 2;

%MODELING PARAMETERS 

 %NNET related
userdata.nntype = 'Feed Forward';
userdata.nntraining = 'Default';
userdata.nnfunction = 'Default';
userdata.nnmethod = 'Predict';
userdata.hiddenstart = 3;
userdata.hiddenstop = 7;
userdata.hiddenstart2 = 3;
userdata.hiddenstop2 = 7;
userdata.epoch = 100;

%SOM related
userdata.somtype = 'Supervised';
userdata.neighbor = 'gaussian';
userdata.shape = 'sheet';
userdata.sommap = 'big';

% These are the temporary parameters that are needed for now. 
userdata.ComponNum = 3;
userdata.Val = 1;
userdata.Nonlinear = 0;
userdata.Square = 0;
userdata.Exp = 0;
userdata.Inverse = 0;
userdata.Log = 0;
userdata.Fin = 4.0;
userdata.Fout = 4.0;
userdata.ChkCluster = 0;
userdata.Cluster = 1;
userdata.Dist = 1;

userdata.FixLen = 10;
userdata.Norm = 0;