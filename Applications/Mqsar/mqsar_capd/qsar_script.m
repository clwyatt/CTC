  
  Default = pwd;
  Defaultfile = [Default 'Scr_' userdata.szModelName];
    
  % Ask user to select the output file to write
  [scriptfile, userdata.pname] = uiputfile(Defaultfile,...
                          'Save the Executable Script File', 50, 50);
  userdata.scriptfile = strcat(userdata.pname,scriptfile);
  % Update the user data structure
  
  userdata.Paramfile = [Default strcat('Prms_',userdata.szModelName,'.mat')];
  
  if (scriptfile~=0)
    save(userdata.Paramfile,'userdata');
  end;
  
  fid = fopen(userdata.scriptfile,'rt');
  com1= ['rm ' userdata.scriptfile];
  if (fid ~= -1)
    [r,s] = unix(com1);
  end;
  fid = fopen(userdata.scriptfile,'wt');
  
  %nodestr = ['NODE=' num2str(userdata.Node)];
  %echonode = 'echo "node = ${NODE}"';
  %opennode = 'bpsh ${NODE} sh<<E_O_F';

  InitMatlab = '/usr/local/matlab12/bin/matlab -nodisplay -nojvm << E_O_F';
  path1 = 'path(path,''/home/matlab/gsomgui/''); addguipath;'; 
  loadmatfile = ['load ' userdata.tmpmatfile];

  count = fprintf(fid,'%s\n','#');

  %count = fprintf(fid,'%s\n',nodestr);
  %count = fprintf(fid,'%s\n',echonode);
  %count = fprintf(fid,'%s\n',opennode);
 
  count = fprintf(fid,'%s\n',InitMatlab);
  count = fprintf(fid,'%s\n',path1);
  count = fprintf(fid,'%s\n',loadmatfile);
  
  
  
  %casemode = 'Result = ga_algselect(Parameters)';
  %count = fprintf(fid,'%s\n',casemode);


  count = fprintf(fid,'%s\n','quit');
  count = fprintf(fid,'%s\n','E_O_F');

  fclose(fid);

  % Reset the number of Training files
  userdata.Training_Counter = 0;

  % Command to make the script executable
  makeexe = ['chmod 777 ' userdata.scriptfile];
  [r,s] = unix(makeexe);