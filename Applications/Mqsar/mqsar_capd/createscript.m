function createscript(userdata);
% Create the script file to submit jobs

%Just to be sure, remove the pre-existing file
fid = fopen(userdata.scriptfile,'rt');
com1= ['rm ' userdata.scriptfile];
if (fid ~= -1)
[r,s] = unix(com1);
end;
fid = fopen(userdata.scriptfile,'wt');

% Matlab 6-5 version Omit
%InitMatlab = '/opt/matlab-6.5/bin/glnx86/matlab -nodisplay -nosplash -nojvm <<E_O_F';
% Matlab 7.0.1
InitMatlab = '/gpfs0/matlab-7.0.1/bin/glnx86/MATLAB -nodisplay -nosplash -nojvm <<E_O_F';

path1 = 'path(path,''/home/ebayram/qsar/mqsar'');'; 
path2 = 'path(path,''/home/ebayram/qsar/matlab/somtoolbox2'');'; 

% Comment out the MAtlab 6-5 version
%export_library = 'export LD_LIBRARY_PATH=/opt/matlab-6.5/sys/os/glnx86:/opt/matlab-6.5/bin/glnx86:/opt/matlab-6.5/extern/lib/glnx86:/opt/matlab-6.5/sys/java/jre/glnx86/jre/lib/i386/native_threads:/opt/matlab-6.5/sys/java/jre/glnx86/jre/lib/i386/hotspot:/opt/matlab-6.5/sys/java/jre/glnx86/jre/lib/i386:/opt/matlab-6.5/sys/opengl/lib/glnx86';

% Use Matlab 7.01 Note that hotspot directory from 6-5 is omitted couldn't locate it
export_library = 'export LD_LIBRARY_PATH=/gpfs0/matlab-7.0.1/sys/os/glnx86:/gpfs0/matlab-7.0.1/bin/glnx86:/gpfs0/matlab-7.0.1/extern/lib/glnx86:/gpfs0/matlab-7.0.1/sys/java/jre/glnx86/jre/glnx86/jre1.4.2/lib/i386/native_threads:/gpfs0/matlab-7.0.1/sys/java/jre/glnx86/jre1.4.2/lib/i386:/gpfs0/matlab-7.0.1/sys/opengl/lib/glnx86';

export_license = 'export LM_LICENSE_FILE=$HOME/wfubmc-matlab-license.dat';

clusterrun = ['Result = cluster_run(''' char(userdata.Paramfile) ''')'];
qsar_dir = 'cd $HOME/qsar/mqsar';

count = fprintf(fid,'%s\n','#!/bin/bash');
count = fprintf(fid,'%s\n','#PBS -j oe');
count = fprintf(fid,'%s\n','### send job emails to yourself (MUST HAVE THIS LINE)');
count = fprintf(fid,'%s\n','#PBS -m abe'); 
count = fprintf(fid,'%s\n','#PBS -M ebayram@wfubmc.edu');
count = fprintf(fid,'%s\n','#PBS -W group_list=generalGrp');

if userdata.modelflag
	count = fprintf(fid,'%s\n','### Request a maximum real, wall clock time limit of 100 hours');
	count = fprintf(fid,'%s\n','#PBS -l walltime=200:00:00');
	count = fprintf(fid,'%s\n','### Request a maximum CPU processing time limit of 100 hours');
	count = fprintf(fid,'%s\n','#PBS -l cput=200:00:00');
else
    count = fprintf(fid,'%s\n','### Request a maximum real, wall clock time limit of 2 hours');
	count = fprintf(fid,'%s\n','#PBS -l walltime=5:00:00');
	count = fprintf(fid,'%s\n','### Request a maximum CPU processing time limit of 2 hours');
	count = fprintf(fid,'%s\n','#PBS -l cput=5:00:00');
end;

count = fprintf(fid,'%s\n','###  Request one random virtual processor with ethernet :myrenet for myrenet ones');
count = fprintf(fid,'%s\n\n','#PBS -l nodes=1:ethernet');
count = fprintf(fid,'%s\n\n',qsar_dir);
count = fprintf(fid,'%s\n\n',export_library);
count = fprintf(fid,'%s\n\n',export_license);
count = fprintf(fid,'%s\n',InitMatlab);
count = fprintf(fid,'%s\n',path1);
count = fprintf(fid,'%s\n',path2);
count = fprintf(fid,'%s\n',clusterrun);
%count = fprintf(fid,'%s\n','quit');
count = fprintf(fid,'%s\n','E_O_F');

fclose(fid);

% Command to make the script executable
makeexe = ['chmod 777 ' userdata.scriptfile];
[r,s] = unix(makeexe);
