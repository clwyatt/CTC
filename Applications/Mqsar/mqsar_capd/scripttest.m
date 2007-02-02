
%

[s,w] =system('beostatus');

%[s,w] = system('bpstat');
%i = find(w=='-')

i = find(w=='-')
[r,w] = system('hostname');
if (findstr(w,'zappa'))
    %ud.RunHost = 'Direct';
    ud.HostName = 'zappa';
    %set(ud.hRunHost,'Value',1);
else
    %ud.RunHost = 'zappa';
    %set(ud.hRunHost,'Value',2);
    ud.HostName = 'zorn';
end;


if (strcmp(ud.HostName,'zappa'))
    Default ='/usr/people/matlab/gsomgui/GSOMTEST/';
else
    Default ='/lama/pentad/ersin/eb_qsar/';
end;   

% Ask user to select the output file to write
[tmpfile, ud.pname] = uigetfile(strcat(Default,'*.mat'), ....
                          'Select the Matlab Parameter File', 50, 50);
ud.tmpmatfile = strcat(ud.pname,tmpfile);


 fid = fopen(ud.tmpfile,'rt');
  com1= ['rm ' ud.tmpfile];
  if (fid ~= -1)
    [r,s] = unix(com1);
  end;
  fid = fopen(ud.tmpfile,'wt');

  %nodestr = ['NODE=' num2str(ud.Node)];
  %echonode = 'echo "node = ${NODE}"';
  %opennode = 'bpsh ${NODE} sh<<E_O_F';

  InitMatlab = '/usr/local/matlab12/bin/matlab -nodisplay -nojvm << E_O_F';
  path1 = 'path(path,''/home/matlab/gsomgui/''); addguipath;'; 
  loadmatfile = ['load ' ud.tmpmatfile];

  count = fprintf(fid,'%s\n','#');

  %count = fprintf(fid,'%s\n',nodestr);
  %count = fprintf(fid,'%s\n',echonode);
  %count = fprintf(fid,'%s\n',opennode);
 
  count = fprintf(fid,'%s\n',InitMatlab);
  count = fprintf(fid,'%s\n',path1);
  count = fprintf(fid,'%s\n',loadmatfile);

  casemode = 'Result = ga_algselect(Parameters)';
  count = fprintf(fid,'%s\n',casemode);


  count = fprintf(fid,'%s\n','quit');
  count = fprintf(fid,'%s\n','E_O_F');

  fclose(fid);

  % Reset the number of Training files
  ud.Training_Counter = 0;

  % Command to make the script executable
  makeexe = ['chmod 777 ' ud.tmpfile];
  [r,s] = unix(makeexe);
end;

%%%%execommand = ['sh ' ud.tmpfile];
%%%[r,s] = unix(execommand);

% Now run the file

%%%[r,s] = unix(ud.tmpfile);
%setstatus('Now you can quit and START the script!!');