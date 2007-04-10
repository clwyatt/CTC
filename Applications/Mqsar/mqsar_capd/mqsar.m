function mqsar(varargin)
% Matlab based QSAR modeling program
%
% Ersin Bayram March 23, 2005
% 

% setup GLOBAL variables
global Mdat;
global userdata;
warning off;

if isempty(Mdat)
currdir = pwd;
sepind= findstr(currdir,filesep);
path(path,strcat(currdir(1:sepind(end)),'somtoolbox2'));
% Ersin: Moved the random seed generator to here. If seed is reset based on
% clock too frequently, we might get the same random numbers.  
rand('state',sum(100*clock));	%re-seed random number generator

userdata = initialsetup(userdata);

Mdat = struct('version', 1.00,...
              'hsetgui',     [],  ...  % general setupfig handles
              'hMsetgui',   [], ...    % model setup gui handles
              'hVsetgui',   [], ...    % model setup gui handles
              'hAgui',   [], ...       % Analysis gui handles
              'hgui',      0 );        % handles to GUI elements
              
mFig = figure('Name',sprintf('mqsar  %5.2f    WFU BME Dept.',Mdat.version),...
              'NumberTitle','off',...
              'Units','pixels',...
              'Position',[4,8,450,410],...
              'ToolBar','none',...  
              'MenuBar','none',... 
              'Resize','off',...
              'DockControls','off');
set(mFig,'CloseRequestFcn',{@closemqsar,mFig});
          
% have mqsar popup in the upper left corner of screen
screenSize=get(0,'MonitorPosition');   % [0 0 width height] for primary monitor
currPos = get(mFig,'Position');   % [ x y width height]  for figure
currPos = [4 screenSize(1,4)-500 currPos(3) currPos(4)];
%currPos = [4 4 currPos(3) currPos(4)];
set(mFig,'Position',currPos);

% ========== CREATE MENUBAR ===========================

%----------------------------------------------------------
mbar = uimenu('Label','File');
%----------------------------------------------------------
mmbar1= uimenu(mbar,'Label','Select');
uimenu(mmbar1,'Label','Training Data','Callback',{@selectfile,'train'});
uimenu(mmbar1,'Label','Test Data','Callback',{@selectfile,'test'});
uimenu(mmbar1,'Label','Model File','Callback',{@selectfile,'model'});
uimenu(mmbar1,'Label','Partition File','Callback',{@selectfile,'partition'});
uimenu(mmbar1,'Label','Output File','Callback',{@selectfile,'output'});

mmbar2=uimenu(mbar,'Label','Save');
uimenu(mmbar2,'Label','Parameters','Callback',@saveparams);
uimenu(mbar,'Label','Preferences','Callback',@qsarsetup);
uimenu(mbar,'Label','Exit','Callback',{@closemqsar,mFig});

% ========== CREATE PANELS ===========================
%-------------------------                 
% JOB PANEL
%-------------------------
JobPanel = uipanel('Parent', mFig,'Title','Jobs','BorderType','line',...
                 'Units','pixels',...
                 'ForegroundColor','black','Position',[1,370,449,40]);              
handles.chkmodel = uicontrol('Parent', JobPanel,'Style','radiobutton',...
                        'Units','pixels',...
                        'Value',1,'String','Build Model',...
                        'Position',[2,5,100,20]);
handles.chkpredict = uicontrol('Parent', JobPanel,'Style','radiobutton',...
                        'Units','pixels',...
                        'Value',0,'String','Predict',...
                        'Position',[105,5,70,20]);                  
handles.script = uicontrol('Parent', JobPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Cluster Script',...
                        'Position',[180,5,100,20],'Callback',@qsarscript);
handles.cluster = uicontrol('Parent', JobPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Cluster Run',...
                        'Position',[285,5,90,20],'Callback',@qsarcluster);                     
handles.run = uicontrol('Parent', JobPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Run',...
                        'Position',[380,5,60,20],'Callback',@qsarrun);                                   
%-------------------------                 
% DATA PANEL
%-------------------------
DataPanel = uipanel('Parent', mFig,'Title','Data','BorderType','line',...
                 'BorderType','line','Units','pixels',...
                 'ForegroundColor','black','Position',[1,190,449,180]); 
             
handles.loadtr = uicontrol('Parent', DataPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Training Data',...
                        'Position',[2,135,110,20],'Callback',{@selectfile,'train'});
handles.trfile = uicontrol('Parent', DataPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', '', 'Position', [115 135 320 20]);

handles.loadtst = uicontrol('Parent', DataPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Test Data',...
                        'Position',[2,105,110,20],'Callback',{@selectfile,'test'});
handles.tstfile = uicontrol('Parent', DataPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', '', 'Position', [115 105 320 20]);
                     
handles.loadpartition = uicontrol('Parent', DataPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Partition File',...
                        'Position',[2,75,110,20],'Callback',{@selectfile,'partition'});
handles.partitionfile = uicontrol('Parent', DataPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', '', 'Position', [115 75 320 20]);

handles.loadmodel = uicontrol('Parent', DataPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Model File',...
                        'Position',[2,45,110,20],'Callback',{@selectfile,'model'});
handles.modelfile = uicontrol('Parent', DataPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', '', 'Position', [115 45 320 20]);
                     
handles.loadoutput = uicontrol('Parent', DataPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Output File',...
                        'Position',[2,15,110,20],'Callback',{@selectfile,'output'});
handles.outputfile = uicontrol('Parent', DataPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', '', 'Position', [115 15 320 20]);   

%-------------------------                 
% PREPROCESS PANEL
%-------------------------    
ProcessPanel = uipanel('Parent', mFig,'Title','PreProcess','BorderType','line',...
                 'BorderType','line','Units','pixels',...
                 'ForegroundColor','black','Position',[1,75,449,115]);
             
txtprotein = uicontrol('Parent', ProcessPanel,'Style', 'text', ...
                         'String', 'Protein', 'Position', [10 50 70 20]);                     
handles.targetmenu = uicontrol('Parent', ProcessPanel,'Style', 'popupmenu', 'Units','pixels',...
                        'BackgroundColor','white',...
                        'String','Other|a4b2|cox1|cox2|d2|d3|dhfr|topliss|NCI',...
                        'Position',[2,30,85,20],'Callback',{@selecttarget});       

txtbioact = uicontrol('Parent', ProcessPanel,'Style', 'text', ...
                         'String', 'Bioact', 'Position',[100 55 50 20]);
txttraining = uicontrol('Parent', ProcessPanel,'Style', 'text', ...
                         'String', 'Train', 'Position',[165 80 40 20]);                     
handles.bioact = uicontrol('Parent', ProcessPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', 'None', 'Position', [151 60 80 20]);  
txttest = uicontrol('Parent', ProcessPanel,'Style', 'text', ...
                         'String', 'Test', 'Position',[245 80 40 20]); 
handles.tstbioact = uicontrol('Parent', ProcessPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', 'None', 'Position', [235 60 80 20]);  

txtbins = uicontrol('Parent', ProcessPanel,'Style', 'text', ...
                         'String', 'Bins', 'Position', [100 30 50 20]);
handles.bins = uicontrol('Parent', ProcessPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', 'None', 'Position', [151 35 80 20]);
handles.tstbins = uicontrol('Parent', ProcessPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', 'None', 'Position', [235 35 80 20]);                     
                         
txtexclude = uicontrol('Parent', ProcessPanel,'Style', 'text', ...
                         'String', 'Exclude', 'Position',[100 5 50 20]);
handles.exclude = uicontrol('Parent', ProcessPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', 'None', 'Position', [151 10 80 20]);  
handles.tstexclude = uicontrol('Parent', ProcessPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', 'None', 'Position', [235 10 80 20]); 
                     
txtcutoff = uicontrol('Parent', ProcessPanel,'Style', 'text', ...
                         'String', 'Cutoff', 'Position', [355 80 50 20]);
handles.cutoff = uicontrol('Parent', ProcessPanel,'Style', 'edit', 'BackgroundColor','white',...
                         'String', 'None', 'Position', [335 65 100 20]);
handles.sortgoodtobad = uicontrol('Parent', ProcessPanel,'Style', 'radiobutton',...
                         'Value',0,'String','Order: Good 1st','Position', [335 40 113 20]);
handles.partition = uicontrol('Parent', ProcessPanel,'Style', 'pushbutton', ...
                         'String', 'Partition', 'Position', [335 10 100 20]);
                     
%-------------------------                 
% Method PANEL
%-------------------------    
MethodPanel = uipanel('Parent', mFig,'Title','Methods','BorderType','line',...
                 'BorderType','line','Units','pixels',...
                 'ForegroundColor','black','Position',[1,0,449,75]);
handles.analyze = uicontrol('Parent', MethodPanel,'Style', 'pushbutton', 'Units','pixels',...
                        'String','Analyze',...
                        'Position',[2,20,90,20],'Callback',{@analyze});
txtvarsel = uicontrol('Parent',MethodPanel,'Style', 'text', ...
                         'String', 'Variable Selection', 'Position', [105 40 110 20]);
handles.varsel = uicontrol('Parent',MethodPanel,'Style', 'popupmenu','BackgroundColor','white',...
                         'String', 'None|GA|Hill Climbing|SA|Stepwise', ...
                         'Position', [105 20 110 20],'Callback',@updatevarsel);
txtmethod = uicontrol('Parent',MethodPanel,'Style', 'text', ...
                         'String', 'Modeling', 'Position',[245 40 70 20]);
handles.method = uicontrol('Parent',MethodPanel,'Style', 'popupmenu', 'BackgroundColor','white',...
                         'String', 'CM|FCM|kNN|MDS|MLR|NNET|PCR|PLS|SOM|SVM', ...
                         'Position', [230 20 100 20],'Callback',@updatemethod);                     
handles.setup = uicontrol('Parent',MethodPanel,'Style', 'pushbutton', 'Units','pixels',...
                        'String','Setup',...
                        'Position',[350,20,85,20],'Callback',{@qsarsetup});
             
handles.mFig=mFig;
Mdat.hgui=handles;

end       
%==================================================
% FUNCTIONS
%==================================================
% ----------------------------------------------------
% Get the valid file name
function [pname] = selectfile(obj,ev,method)
% Ask user to select the file by popping up a load file window
%
global Mdat

switch method
case 'train'
    [FileName, FilePath] = uigetfile({'*.txt;*.dat','Training Data (*.txt, *.dat)'; ...
                      '*.*', 'All Files (*.*)'}, 'Pick Training file');
    set(Mdat.hgui.trfile,'String',strcat(FilePath,FileName));
case 'test'
    [FileName, FilePath] = uigetfile({'*.txt;*.dat','Test Data (*.txt, *.dat)'; ...
                      '*.*', 'All Files (*.*)'}, 'Pick Test file');   
     set(Mdat.hgui.tstfile,'String',strcat(FilePath,FileName));
case 'partition'   
    [FileName, FilePath] = uigetfile({'*.txt;*.dat','Partition Mask (*.txt, *.dat)'; ...
                      '*.*', 'All Files (*.*)'}, 'Pick Partition file');
    set(Mdat.hgui.partitionfile,'String',strcat(FilePath,FileName));             
case 'model'
    if get(Mdat.hgui.chkmodel,'Value')
        [FileName, FilePath] = uiputfile({'*.mod', 'Model File (*.mod)'; ...
                      '*.*', 'All Files (*.*)'}, 'Pick Model file');         
    else
        [FileName, FilePath] = uigetfile({'*.mod', 'Model File (*.mod)'; ...
                      '*.*', 'All Files (*.*)'}, 'Pick Model file');          
    end;  
    set(Mdat.hgui.modelfile,'String',strcat(FilePath,FileName));
case 'output'
    if get(Mdat.hgui.chkmodel,'Value')
        [FileName, FilePath] = uiputfile({'*.out', 'Output File (*.out)'; ...
                          '*.*', 'All Files (*.*)'}, 'Pick Output file');
    else
       [FileName, FilePath] = uiputfile({'*.prd', 'Test Files (*.prd)'; ...
                      '*.*', 'All Files (*.*)'}, 'Pick Prediction file'); 
    end   
    set(Mdat.hgui.outputfile,'String',strcat(FilePath,FileName));
end;    

% ----------------------------------------
% Select the type of the target data file.
function selecttarget(hObject, eventdata, handles)
global Mdat;
szBioact = 'None';
szBin = 'None';
szExcl = 'None';
sztstBioact = 'None';
sztstBin = 'None';
sztstExcl = 'None';
szCutoff = [];
sortgoodtobad = 0;

tgtname = get(Mdat.hgui.targetmenu, 'String');
tgtname = deblank(tgtname(get(Mdat.hgui.targetmenu, 'Value'),:));

switch  tgtname  
    case 'a4b2',     % A4b2
        szBioact = 'start';
        szCutoff = '[200 1000]'; %'[100 250]';
        sztstBioact = 'start';
        sztstCutoff = '[200 1000]'; %'[100 250]';
        sortgoodtobad = 1;
    case 'cox1',     % cox2
        szBioact = 'start';
        szCutoff = '[-0.8 0.125 1.5]'; 
        sztstBioact = 'start';
        sztstCutoff = '[-0.8 0.125 1.5]'; 
        szBin = '2';
        sztstBin = '2';
        szExcl = '[3 4]';
        sztstExcl = '[3 4]';
    case 'cox2',     % cox3
        szBioact = '3';
        szCutoff = '[-0.8 0.125 1.5]'; %'[100 500]';  
        sztstBioact = '3';
        sztstCutoff = '[-0.8 0.125 1.5]'; %'[100 500]';  
        szBin = '4';
        sztstBin = '4';
        szExcl = '[1 2]';
        sztstExcl = '[1 2]';
    case 'd2',     % D2
        szBioact = 'start';
        szCutoff = '[2 3]'; %'[100 500]';
        sztstBioact = 'start';
        sztstCutoff = '[2 3]'; %'[100 500]';
        sortgoodtobad = 1;
    case 'd3',     % D3
        szBioact = 'start';
        szCutoff = '[2 3]'; %'[100 500]';
        sztstBioact = 'start';
        sztstCutoff = '[2 3]'; %'[100 500]';
        sortgoodtobad = 1;
    case 'dhfr',   % DHFR
        szBioact = 'start';
        szCutoff = '[6.75 7.75]';
        sztstBioact = 'start';
        sztstCutoff = '[6.75 7.75]';
    case 'topliss',% Topliss
        szBioact = 'start';
        szCutoff = '[1.5 2.5 3.5]';
        sztstBioact = 'start';
        sztstCutoff = '[1.5 2.5 3.5]';
    case 'NCI'
        szBioact = 'start';
        szCutoff = '[4.2 6]';
        sztstBioact = 'start';
        sztstCutoff = '[4.2 6]';
        sortgoodtobad = 1;
end

set(Mdat.hgui.bioact, 'String', szBioact);
set(Mdat.hgui.bins, 'String', szBin);
set(Mdat.hgui.exclude, 'String', szExcl);
set(Mdat.hgui.tstbioact, 'String', sztstBioact);
set(Mdat.hgui.tstbins, 'String', sztstBin);
set(Mdat.hgui.tstexclude, 'String', sztstExcl);
set(Mdat.hgui.cutoff, 'String', szCutoff);
set(Mdat.hgui.sortgoodtobad, 'Value', sortgoodtobad);

% --- Executes on button press in Run.
function qsarrun(hObject, eventdata, handles)
% hObject    handle to Cluster_job (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Hint: get(hObject,'Value') returns toggle state of Cluster_job

global Mdat userdata;

% Update the userdata info from the GUI handles
userdata = updateinfo(Mdat,userdata);
% Import the data and prepare according to user defined filter parameters
% [CmpNames, VarNames, Xmat, Ymat, Ynames, Bins,Maskmat] = prepdata(userdata);

[CmpNames, VarNames, Xmat, Ymat, Ynames, Bins,Maskmat] = ...
    CTC_dataimport(userdata.trfile, userdata.tstfile);

switch userdata.validation
case 'None'
    ModelLimit = size(Maskmat, 2);
case {'Leave-One-Out','Leave-Some-Out'}      
    ModelLimit = 1;
end;

tic;
if userdata.modelflag  % Create a model   
    for nModel=1:ModelLimit
        [bestgenome,CrossMat] = qsarmodel(userdata,VarNames,CmpNames,Xmat,Ymat,Bins,Maskmat,nModel);
        % Best descriptors are found. Create the model for final statistics
        if nModel ==1
            % Open the model file
            fmodel = fopen(userdata.modelfile, 'w');
            dt = fix(clock());
            dt = sprintf('%d-%d-%d %d:%d:%d', dt(1), dt(2), dt(3), dt(4), dt(5), dt(6));
            fprintf(fmodel, '%s\t%s\n', char(strcat(userdata.method, ' model')), dt);

            % Check if the output file name is valid
            try
                fout = fopen(userdata.outputfile, 'w');
                dt = fix(clock());
                dt = sprintf('%d-%d-%d %d:%d:%d', dt(1), dt(2), dt(3), dt(4), dt(5), dt(6));                
                fprintf(fout, '%s\t%s\n', char(strcat(userdata.method, ' results')), dt);
            catch
                errordlg('Please enter a valid output file name!','I/O Error');
                return;
            end
        end;
        qsarresult(userdata,bestgenome,VarNames,CmpNames,Xmat,Ymat,Bins,CrossMat,nModel,fmodel,fout)  
    end % for nModel
    fclose(fmodel);
    fclose(fout);
end;
if userdata.predictflag  % Predict bioactivity/Class
% XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
% The following block is designed for using the built model(s) to
% predict the specify external data
% XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
  
    % Read file according to the specified options
   [PrdCmpNames, PrdVarNames, PrdX, PrdY, PrdYname, PrdBins] = qsarimport(userdata.tstfile, 'bioact', ...
                                                     userdata.tstbioact, 'binning', userdata.tstbin, ...
                                                    'cutoffs', userdata.cutoff, 'ex', userdata.tstexclude);
  if isempty(PrdY)
      PrdY = zeros(length(PrdCmpNames),1);
  end;
  if isempty(PrdBins)
      PrdBins = ones(length(PrdCmpNames),1);
  end;
  
   for nModel=1:ModelLimit
        
        Results = qsarpredict(userdata,VarNames,CmpNames,Xmat,Ymat,Bins,...
                                       PrdCmpNames, PrdVarNames, PrdX, PrdY, PrdBins,Maskmat,nModel);
        % Best descriptors are found. Create the model for final statistics
        %qsarresult(userdata,bestgenome,VarNames,CmpNames,Xmat,Ymat,Bins,CrossMat,nModel,fmodel,fout)  
    end % for nModel    
end;
toc;


% --- Executes on button press in Send to Cluster.
function qsarscript(hObject, eventdata, handles)
% Generate a script file to launch jobs on daugter nodes
% ------------------------------------------------------
global Mdat userdata;

% Update the userdata info from the GUI handles
userdata = updateinfo(Mdat,userdata);
% Create the parameter file to load back on compute nodes
Default = userdata.modelfile;
Pathloc = findstr(Default,filesep);
Currentpath = pwd;
Currentpath = strcat(Currentpath,filesep);
Dotloc = findstr(Default,'.');
if (isempty(Pathloc))
	if (isempty(Dotloc))
        if userdata.modelflag
            Defaultfile = [Currentpath strcat(userdata.modelfile,'.job')];
        else
            Defaultfile = [Currentpath strcat('Prd_', userdata.modelfile) '.job'];
        end;    
    else
        if userdata.modelflag
            Defaultfile = [Currentpath strcat( userdata.modelfile(1:Dotloc(end)-1), '.job')];
        else
            Defaultfile = [Currentpath strcat('Prd_', userdata.modelfile(1:Dotloc(end)-1), '.job')];
        end;  
    end;    
else
    Default = Default(1:Pathloc(end-1));
    if (isempty(Dotloc))
        if userdata.modelflag
            Defaultfile = [Default strcat(userdata.modelfile(Pathloc(end)+1:end), '.job')];
        else
            Defaultfile = [Default strcat('Prd_', userdata.modelfile(Pathloc(end)+1:end), '.job')];
        end;
    else
        if userdata.modelflag
            Defaultfile = [Default strcat(userdata.modelfile(Pathloc(end)+1:Dotloc(end)-1),'.job')];
        else
            Defaultfile = [Default strcat('Prd_', userdata.modelfile(Pathloc(end)+1:Dotloc(end)-1),'.job')];
        end; 
    end
end    

% Ask user to select the output file to write
[scriptfile, pname] = uiputfile(Defaultfile,'Save the Executable Script File', 50, 50);
if (scriptfile == 0) | (~ischar(scriptfile))  
   szStr = 'Please enter a valid script file name!';
   errordlg(szStr,'File Error');
   return;
else 
    userdata.scriptfile = strcat(pname,scriptfile);
end;
Defaultfile = [pname strcat('Prms_',scriptfile(1:end-4),'.mat')];

% Ask user to select the output file to write
[parameterfile, pname] = uiputfile(Defaultfile,...
                      'Save the Parameters to a Mat File', 50, 50);
userdata.Paramfile = strcat(pname,parameterfile);
if (parameterfile~=0)
  save(userdata.Paramfile,'userdata');
else
   szStr = 'Please enter a valid Parameter file name!'
   errordlg(szStr,'File Error');
   return;  
end;
%Now we have all the info needed, create the job script
createscript(userdata);

% --------------------------------------------------------------------
function closemqsar(obj,ev,mFig)
% --------------------------------------------------------------------
global Mdat;

%closemqsar = questdlg('Close mqsar?','Exit dialog','Yes'); 
closemqsar = 'Yes';

if strcmp(closemqsar,'Yes') 
  if isfield(Mdat,'hsetgui') 
      if isfield(Mdat.hsetgui,'sFig')
        delete(Mdat.hsetgui.sFig);
      end;  
  end 
  if isfield(Mdat,'hMsetgui')      
      if isfield(Mdat.hMsetgui,'sMFig')
        delete(Mdat.hMsetgui.sMFig);
      end;  
  end 
  if isfield(Mdat,'hVsetgui') 
      if isfield(Mdat.hVsetgui,'sVFig')
        delete(Mdat.hVsetgui.sVFig);
      end;  
  end
  if isfield(Mdat,'hAgui') 
      if isfield(Mdat.hAgui,'aFig')
        delete(Mdat.hAgui.aFig);
      end;  
  end 
  if isstruct(Mdat)
      delete(Mdat.hgui.mFig);
  end  
  clear all;
end

% --------------------------------------------------------------------
function qsarpartition(obj,ev,mFig)
% --------------------------------------------------------------------

% Read file according the specify options
% NEED to add a function to parition files into training and test sets. 

% --------------------------------------------------------------------
function updatemethod(obj,ev)
% --------------------------------------------------------------------
global Mdat;
global userdata;

% Modeling
userdata.method = get(Mdat.hgui.method, 'String');
userdata.method = deblank(userdata.method(get(Mdat.hgui.method, 'Value'),:));

if ~isempty(Mdat.hMsetgui) 
    if strcmp(get(Mdat.hMsetgui.sMFig,'Visible'),'on')
        delete(Mdat.hMsetgui.sMFig);
    end;   
    Mdat= rmfield(Mdat,'hMsetgui');
    Mdat.hMsetgui = [];
    pos = get(Mdat.hgui.mFig,'Position');
    qsarsetup(pos);
    modelsetup(pos);      
    varselsetup(pos);
end

% --------------------------------------------------------------------
function updatevarsel(obj,ev)
% --------------------------------------------------------------------
global Mdat;
global userdata;

% Modeling
userdata.varselect = get(Mdat.hgui.varsel, 'String');
userdata.varselect = deblank(userdata.varselect(get(Mdat.hgui.varsel, 'Value'),:));

if ~isempty(Mdat.hVsetgui)  
    if strcmp(get(Mdat.hVsetgui.sVFig,'Visible'),'on')
        delete(Mdat.hVsetgui.sVFig);
    end;   
    Mdat= rmfield(Mdat,'hVsetgui');
    Mdat.hVsetgui = [];
    pos = get(Mdat.hgui.mFig,'Position');
    qsarsetup(pos);
    modelsetup(pos);      
    varselsetup(pos);  
end

% --------------------------------------------------------------------
function userdata = updateinfo(Mdat,userdata)
% --- Executes on button press in Run.
% --------------------------------------------------------------------

userdata.trfile = get(Mdat.hgui.trfile, 'String');
userdata.tstfile = get(Mdat.hgui.tstfile, 'String');
userdata.partitionfile = get(Mdat.hgui.partitionfile, 'String');
userdata.modelfile = get(Mdat.hgui.modelfile, 'String');
userdata.outputfile = get(Mdat.hgui.outputfile, 'String');
userdata.bin = str2num(get(Mdat.hgui.bins, 'String'));      % define the binning column
userdata.exclude = str2num(get(Mdat.hgui.exclude, 'String'));   % define the exclusion columns
userdata.tstbin = str2num(get(Mdat.hgui.tstbins, 'String'));      % define the test data binning column
userdata.tstexclude = str2num(get(Mdat.hgui.tstexclude, 'String'));   % define the test data exclusion columns
userdata.cutoff = str2num(get(Mdat.hgui.cutoff, 'String')); % define the cutoff arguments       
userdata.modelflag = get(Mdat.hgui.chkmodel, 'Value');
userdata.predictflag = get(Mdat.hgui.chkpredict, 'Value');

userdata.sortgoodtobad = get(Mdat.hgui.sortgoodtobad, 'Value');

% define the bioact column
szStr = get(Mdat.hgui.bioact, 'String');
if strcmpi(szStr, 'start')
    userdata.bioact = 1;
elseif strcmpi(szStr, 'end')
    userdata.bioact = 'end';
else
    userdata.bioact = str2num(szStr);
end

% define the test data bioact column
szStr = get(Mdat.hgui.tstbioact, 'String');
if strcmpi(szStr, 'start')
    userdata.tstbioact = 1;
elseif strcmpi(szStr, 'end')
    userdata.tstbioact = 'end';
else
    userdata.tstbioact = str2num(szStr);
end

% Data I/O related settings
userdata.corrfilt = str2num(get(Mdat.hsetgui.corrfilt, 'String')); % define the cutoff arguments 
userdata.stdfilt = str2num(get(Mdat.hsetgui.stdfilt, 'String')); 
% scaling method
scaling = get(Mdat.hsetgui.scaling, 'String');
userdata.scaling = deblank(scaling(get(Mdat.hsetgui.scaling, 'Value'),:)); 

% selected metric
metric = get(Mdat.hsetgui.metric, 'String');
userdata.metric = deblank(scaling(get(Mdat.hsetgui.metric, 'Value'),:)); 

% validation method
validation = get(Mdat.hsetgui.validation, 'String');
userdata.validation = deblank(validation(get(Mdat.hsetgui.validation, 'Value'),:));

% optimization method
optimize = get(Mdat.hsetgui.optimize, 'String');
userdata.optimize = deblank(optimize(get(Mdat.hsetgui.optimize, 'Value'),:));

userdata.percentout = str2num(get(Mdat.hsetgui.percentout, 'String'));
userdata.nfold = str2num(get(Mdat.hsetgui.nfold, 'String'));
% Descriptor Size limit
DescLimit = get(Mdat.hsetgui.DescLimit, 'String');
userdata.DescLimit = deblank(DescLimit(get(Mdat.hsetgui.DescLimit, 'Value'),:));

userdata.Descriptors = str2num(get(Mdat.hsetgui.DescNum, 'String'));      % define the binning column

% Variable Selection method
varsel = get(Mdat.hgui.varsel, 'String');
userdata.varselect = deblank(varsel(get(Mdat.hgui.varsel, 'Value'),:));
switch userdata.varselect
case 'None'
    
case 'GA'
    userdata.MinMutate = str2num(get(Mdat.hVsetgui.minmutate, 'String'));
    userdata.MaxMutate = str2num(get(Mdat.hVsetgui.maxmutate, 'String'));
    userdata.SubsetPopulation = str2num(get(Mdat.hVsetgui.subno,'String'));
    userdata.MinCrossOver = str2num(get(Mdat.hVsetgui.mincross,'String'));
    userdata.MaxCrossOver = str2num(get(Mdat.hVsetgui.maxcross,'String'));
    userdata.Pmutation = str2num(get(Mdat.hVsetgui.pmutate,'String'));
    userdata.Pcross = str2num(get(Mdat.hVsetgui.pcross,'String'));
    userdata.SubsetPopulation = str2num(get(Mdat.hVsetgui.subpop,'String'));
    userdata.Subsets = str2num(get(Mdat.hVsetgui.subno,'String'));
    userdata.Generation = str2num(get(Mdat.hVsetgui.generations,'String'));
    userdata.PoolDur = str2num(get(Mdat.hVsetgui.pooldur,'String'));
    userdata.PoolFreq = str2num(get(Mdat.hVsetgui.poolfreq,'String'));
    userdata.Elites = str2num(get(Mdat.hVsetgui.elite,'String'));
    userdata.GeneLife = str2num(get(Mdat.hVsetgui.GeneLife,'String'));
    % Temporary variable until code transfer is complete
    userdata.MoveTarget = 0;
    userdata.EliteDuration = 10;
    
case 'SA'    
    userdata.StartT = str2num(get(Mdat.hVsetgui.tstart, 'String'));
    userdata.EndT = str2num(get(Mdat.hVsetgui.tstop, 'String'));
    userdata.Droprate = str2num(get(Mdat.hVsetgui.droprate, 'String'));
    userdata.Cycle = str2num(get(Mdat.hVsetgui.cycle, 'String'));
    userdata.Mutate = str2num(get(Mdat.hVsetgui.mutation, 'String'));
case 'Hill Climbing'
    userdata.HGeneration = str2num(get(Mdat.hVsetgui.generations,'String'));
    userdata.Hcycle = str2num(get(Mdat.hVsetgui.Hcycle,'String'));
    userdata.Mutate = str2num(get(Mdat.hVsetgui.mutation, 'String'));
case 'Stepwise'    
    
end;

% Modeling method
method = get(Mdat.hgui.method, 'String');
userdata.method = deblank(method(get(Mdat.hgui.method, 'Value'),:));
switch userdata.method
case {'CM','FCM','kNN','MDS','MLR','PCR','SVM'}

case 'NNET'
    nntype = get(Mdat.hMsetgui.nntype, 'String');
    userdata.nntype = deblank(nntype(get(Mdat.hMsetgui.nntype, 'Value'),:));
    nntraining = get(Mdat.hMsetgui.nntraining, 'String');
    userdata.nntraining = deblank(nntraining(get(Mdat.hMsetgui.nntraining, 'Value'),:));
    userdata.hiddenstart = str2num(get(Mdat.hMsetgui.hiddenstart, 'String'));
    userdata.hiddenstop = str2num(get(Mdat.hMsetgui.hiddenstop, 'String'));
    userdata.hiddenstart2 = str2num(get(Mdat.hMsetgui.hiddenstart2, 'String'));
    userdata.hiddenstop2 = str2num(get(Mdat.hMsetgui.hiddenstop2, 'String'));
    nnfunction = get(Mdat.hMsetgui.nnfunction, 'String');
    userdata.nnfunction = deblank(nnfunction(get(Mdat.hMsetgui.nnfunction, 'Value'),:));
    nnmethod = get(Mdat.hMsetgui.nnmethod, 'String');
    userdata.nnmethod = deblank(nnmethod(get(Mdat.hMsetgui.nnmethod, 'Value'),:));
    userdata.epoch = str2num(get(Mdat.hMsetgui.epoch, 'String'));
case 'SOM'
    somtype = get(Mdat.hMsetgui.somtype, 'String');
    userdata.somtype = deblank(somtype(get(Mdat.hMsetgui.somtype, 'Value'),:)); 
    sommap = get(Mdat.hMsetgui.sommap, 'String');
    userdata.sommap = deblank(sommap(get(Mdat.hMsetgui.sommap, 'Value'),:));
    neighbor = get(Mdat.hMsetgui.neighbor, 'String');
    userdata.neighbor = deblank(neighbor(get(Mdat.hMsetgui.neighbor, 'Value'),:));
    shape = get(Mdat.hMsetgui.shape, 'String');
    userdata.shape = deblank(shape(get(Mdat.hMsetgui.shape, 'Value'),:));
end   

% Temporary variables 
userdata.ClassBins = length(userdata.cutoff) +1;
userdata.Cluster = 1;
userdata.ComponNum = 3;
userdata.Val = 1;
userdata.Nonlinear = 0;
userdata.Square = 0;
userdata.Exp = 0;
userdata.Inverse = 0;
userdata.Log = 0;
userdata.Fin = 4.0;
userdata.Fout = 4.0;
userdata.ShuffleFlag  = 0;