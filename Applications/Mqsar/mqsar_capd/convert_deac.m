function convert_deac(data)
% This m-file converts the scripts and parameter variables to a compatible
% format with deac cluster.
%
% Input
% data: string to indicate the type of data: a4b2, d3, d2, dhfr, Topliss, or NCI
% Optional Inputs
% 'GA', 'ga', 'Ga' : indicates that the method is Genetic algorithm which
% is different than others. 
%
% Ersin Bayram, October 2003
% Targacept, Inc. 
% http://www.targacept.com
%---------------------------------------------------------------

%% Check arguments
i=1;

if isunix
    Defaultfile = '/home/matlab/SBIR/*.mat';
else    
    Defaultfile = ['C:\QSAR\Deac\' data '\*.mat'];
end

[PrmName, FilePath] = uigetfile(Defaultfile, 'Pick the Parameter File to Load');
prmfile = strcat(FilePath,PrmName);

load(prmfile)
Loc = findstr(userdata.trfile,filesep);

if strcmp(data,'NCI') | strcmp(data,'nci')
    userdata.szDataName = ['/home/ebayram/qsar/Data/NCI/' userdata.trfile(Loc(end)+1:end)];
else        
    userdata.trfile = ['/home/ebayram/qsar/' data '/Data/' userdata.trfile(Loc(end)+1:end)];
end

Loc = findstr(userdata.modelfile,filesep);
userdata.modelfile = ['/home/ebayram/qsar/' data '/Model/' userdata.modelfile(Loc(end)+1:end)];

if ~isempty(userdata.tstfile)
    Loc = findstr(userdata.tstfile,filesep);
	if strcmp(data,'NCI') | strcmp(data,'nci')
        userdata.tstfile = ['/home/ebayram/qsar/Data/NCI/' userdata.tstfile(Loc(end)+1:end)];
	else
        userdata.tstfile = ['/home/ebayram/qsar/' data '/Data/'  userdata.tstfile(Loc(end)+1:end)];
	end;
end;

  
Loc = findstr(userdata.outputfile,filesep);
if userdata.predictflag
  userdata.outputfile = ['/home/ebayram/qsar/' data '/Prediction/' userdata.outputfile(Loc(end)+1:end)];    
else    
  userdata.outputfile = ['/home/ebayram/qsar/' data '/Output/' userdata.outputfile(Loc(end)+1:end)];
end;

if ~isempty(userdata.partitionfile)
    Loc = findstr(userdata.partitionfile,filesep);
    if strcmp(data,'NCI') | strcmp(data,'nci')
        userdata.partitionfile = ['/home/ebayram/qsar/Data/NCI/'  userdata.partitionfile(Loc(end)+1:end)];
    else
        userdata.partitionfile = ['/home/ebayram/qsar/Data/'  userdata.partitionfile(Loc(end)+1:end)];
    end;
end;

fid   = fopen(userdata.scriptfile,'r');
Script = fread(fid);
Script = char(Script');
fclose(fid);

Loc = findstr(userdata.scriptfile,filesep);
scriptname = userdata.scriptfile;

userdata.scriptfile = ['/home/ebayram/qsar/' data '/Scripts/' userdata.scriptfile(Loc(end)+1:end)];

ScriptLoc = findstr(Script,userdata.Paramfile);
Plength = length(userdata.Paramfile);

Loc = findstr(userdata.Paramfile,filesep);
paramname = userdata.Paramfile;

userdata.Paramfile = ['/home/ebayram/qsar/' data '/Scripts/' userdata.Paramfile(Loc(end)+1:end)]

Script = [Script(1:ScriptLoc(1)-1), userdata.Paramfile Script(ScriptLoc(1)+Plength:end)];

%save(paramname,'userdata','-v6');
save(paramname,'userdata');

fid = fopen(scriptname,'w');
fwrite(fid,Script,'char');
fclose(fid);
