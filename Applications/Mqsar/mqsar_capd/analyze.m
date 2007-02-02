function analyze(varargin)
% QSAR tool's analysis panel
%
% pos is the position of the main QSAR gui. 

global Mdat;
global userdata;
pos = get(Mdat.hgui.mFig,'Position');
if ~isfield(Mdat.hAgui,'aFig')
    aFig = figure('Name','Statistical Analysis',...
                  'NumberTitle','off',...
                  'Units','pixels',...
                  'Position',[pos(1),pos(2)-178,pos(3),145],...
                  'ToolBar','none',...  
                  'MenuBar','none',... 
                  'Resize','off',...
                  'DockControls','off');
    set(aFig,'CloseRequestFcn',{@closesetup,aFig});
    setupPanel = uipanel(aFig,'Units','pixels',...
                     'ForegroundColor','black','Position',[0,0,pos(3),145]); 
                 
    %-------------------------                 
    % PREPROCESS PANEL
    %-------------------------   
    StatPanel = uipanel('Title','Statistics','BorderType','line',...
                     'BorderType','line','Units','pixels',...
                     'ForegroundColor','black','Position',[1,1,pos(3)-150,144]);  
                 
    method = get(Mdat.hgui.method, 'String');
    method = deblank(method(get(Mdat.hgui.method, 'Value'),:));  
    switch method
    case {'CM','FCM','kNN','MDS','SOM','SVM'}
        chkprd = 0;
    otherwise
        chkprd = 1;
    end;    
            
    handles.chi2 = uicontrol('Parent', StatPanel,'Style','radiobutton',...
                        'Units','pixels',...
                        'Value',0,'String','Chi2',...
                        'Position',[2,105,45,20]);
    handles.q2 = uicontrol('Parent', StatPanel,'Style','radiobutton',...
                        'Units','pixels',...
                        'Value',chkprd,'String','q2',...
                        'Position',[50,105,40,20]);                
    handles.r2 = uicontrol('Parent', StatPanel,'Style','radiobutton',...
                        'Units','pixels',...
                        'Value',chkprd,'String','r2',...
                        'Position',[95,105,40,20]);  
    handles.cmat = uicontrol('Parent', StatPanel,'Style','radiobutton',...
                        'Units','pixels',...
                        'Value',1,'String','Confusion Matrix',...
                        'Position',[140,105,120,20]);    
                    
    handles.stats = uicontrol('Parent', StatPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Results',...
                        'Position',[2,80,50,20],'Callback',{@selectfile,'stats'});
    handles.statfile = uicontrol('Parent', StatPanel,'Style', 'edit', 'BackgroundColor','white',...
                             'String', '', 'Position', [55 80 pos(3)-210 20]); 
                         
    txtplot = uicontrol('Parent', StatPanel,'Style', 'text', ...
                         'String', 'Plot', 'Position',[1 30 35 20]);
    txtact = uicontrol('Parent', StatPanel,'Style', 'text', ...
                             'String', 'Bioactivity', 'Position',[38 55 125 20]);                     
    handles.plotactivity = uicontrol('Parent', StatPanel,'Style', 'popupmenu', 'BackgroundColor','white',...
                             'String', 'Training vs Actual|Test vs Actual|Validation vs Actual|All', ...
                             'Position', [43 35 125 20],...
                             'Callback',{@plotresult,'activity'});  
    txtbin = uicontrol('Parent', StatPanel,'Style', 'text', ...
                             'String', 'Classification', 'Position',[170 55 125 20]); 
    handles.plotbin = uicontrol('Parent', StatPanel,'Style', 'popupmenu', 'BackgroundColor','white',...
                             'String', 'Training vs Actual|Test vs Actual|Validation vs Actual|All',...
                             'Position', [170 35 125 20],...
                             'Callback',{@plotresult,'bin'});  
    handles.calcstats = uicontrol('Parent', StatPanel,'Style','pushbutton',...
                        'Units','pixels',...
                        'String','Calculate Stats',...
                        'Position',[80,5,140,20],'Callback',@calcstats);                     
                         
    %-------------------------                 
    % PREPROCESS PANEL
    %-------------------------   
    DescPanel = uipanel('Title','Descriptors','BorderType','line',...
                     'BorderType','line','Units','pixels',...
                     'ForegroundColor','black','Position',[pos(3)-151,1,150,144]);                           
                             
    handles.aFig=aFig;
    Mdat.hAgui=handles;            
else
    set(Mdat.hAgui.aFig,'Position',[pos(1),pos(2)-178,pos(3),145]);
    set(Mdat.hAgui.aFig,'Visible','on');
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
case 'stats'
    [FileName, FilePath] = uigetfile({'*.txt;*.dat','Results File (*.txt, *.dat)'; ...
                      '*.*', 'All Files (*.*)'}, 'Pick Statistical Results File');
    set(Mdat.hAgui.statfile,'String',strcat(FilePath,FileName));
end;    


% --------------------------------------------------------------------
function calcstats(obj,ev)
% --------------------------------------------------------------------
global Mdat;

trfile = get(Mdat.hgui.trfile, 'String');
tstfile = get(Mdat.hgui.tstfile, 'String');
partitionfile = get(Mdat.hgui.partitionfile, 'String');
modelfile = get(Mdat.hgui.modelfile, 'String');
outputfile = get(Mdat.hgui.outputfile, 'String');

statfile = get(Mdat.hAgui.statfile, 'String');
if isempty(statfile)
   statfile = strcat(outputfile(1:end-3),'txt'); 
   set(Mdat.hAgui.statfile,'String',statfile);
end;
    
cutoff = str2num(get(Mdat.hgui.cutoff, 'String')); % define the cutoff arguments  

bin = str2num(get(Mdat.hgui.bins, 'String'));      % define the binning column
exclude = str2num(get(Mdat.hgui.exclude, 'String'));   % define the exclusion columns
tstbin = str2num(get(Mdat.hgui.tstbins, 'String'));      % define the test data binning column
tstexclude = str2num(get(Mdat.hgui.tstexclude, 'String'));   % define the test data exclusion columns
sortgoodtobad = get(Mdat.hgui.sortgoodtobad, 'Value');

% define the bioact column
szStr = get(Mdat.hgui.bioact, 'String');
if strcmpi(szStr, 'start')
    bioact = 1;
elseif strcmpi(szStr, 'end')
    bioact = 'end';
else
    bioact = str2num(szStr);
end
% define the test data bioact column
szStr = get(Mdat.hgui.tstbioact, 'String');
if strcmpi(szStr, 'start')
   tstbioact = 1;
elseif strcmpi(szStr, 'end')
   tstbioact = 'end';
else
   tstbioact = str2num(szStr);
end

binnum = length(cutoff)+1;

if ~isempty(outputfile)
    ftext = textread(outputfile,'%s','delimiter','\n');
    i=1;
    ind = 1;
    while i < length(ftext)
          if ~isempty(strfind(char(ftext(i)),'Training Set'))    
             Trbeg(ind) = i+2;   
          end;
          if ~isempty(strfind(char(ftext(i)),'Model')) & ind ~= 1
                Tstend(ind-1) = i-2; 
          end; 
          if ~isempty(strfind(char(ftext(i)),'Test Set'))    
             Trend(ind) = i-2; 
             Tstbeg(ind) = i+2;
             ind = ind +1; 
          end;
         i = i+1;
    end;      
    Tstend(end+1) = length(ftext);
else
    return;
end;   

Trdat = [];
Tstdat = [];
for kk=1:length(Trend)
    Trdat = [Trdat ; ftext(Trbeg(kk):Trend(kk))];
    Tstdat = [Tstdat ; ftext(Tstbeg(kk):Tstend(kk))];
end;

Trdat = str2num(char(Trdat));
Tstdat = str2num(char(Tstdat));

Trmols = cellstr(num2str(Trdat(:,1)));
Tstmols = cellstr(num2str(Tstdat(:,1)));

for i=1:length(Trmols)
   tmp= isspace(Trmols{i});
   if tmp(1) ==1
       beg = find(tmp==0);
       nstr = char(Trmols{i});
       Trmols{i} = nstr(beg(1):end);
   end;    
end;
for i=1:length(Tstmols)
   tmp= isspace(Tstmols{i});
   if tmp(1) ==1
       beg = find(tmp==0);
       nstr = char(Tstmols{i});
       Tstmols{i} = nstr(beg(1):end);
   end;    
end;

if ~isempty(trfile)
    Desc = dlmread(trfile,'\t',1,1);
    % Size of the numeric Data
    [Height,Width] = size(Desc);
    % Add one to  width for column names
    Width = Width+2;
    Bufsize = (Height+1)*(Width+1);
    if Bufsize < 30000
        Bufsize = 30000;
    end;    

    % Read the molecule names and ignore the rest of the file
    rownames  = textread(trfile,'%s%*[^\n]','whitespace','\t','bufsize',Bufsize);
    rownames(1) = [];  
else
    error('Stats can not be calculated without a training dataset');
    return;
end;    

if ~isempty(tstfile)
    Desc2 = dlmread(tstfile,'\t',1,1);
    % Size of the numeric Data
    [Height,Width] = size(Desc2);
    % Add one to  width for column names
    Width = Width+2;
    Bufsize = (Height+1)*(Width+1);
    if Bufsize < 30000
        Bufsize = 30000;
    end;    

    % Read the molecule names and ignore the rest of the file
    rownames2  = textread(tstfile,'%s%*[^\n]','whitespace','\t','bufsize',Bufsize);
    rownames2(1) = [];  
else
    Desc2 = [];
    rownames2 = [];
end; 

for k=1:length(rownames)
    ind =1;
    Test(k).result = [];
    Training(k).result = [];
    for j=1:length(Trmols)
        if strcmp(char(Trmols(j,:)),char(rownames{k}))
            Training(k).result(ind) = Trdat(j,3);
            ind = ind + 1;
        end;
    end;
    ind2 =1;
    for j=1:length(Tstmols)
        if strcmp(char(Tstmols(j,:)),char(rownames{k}))
            Test(k).result(ind2) = Tstdat(j,3);
            ind2 = ind2 + 1;
        end;
    end; 
end;    

  
for k=1:length(rownames2)
    ind =1;
    for j=1:length(Tstmols)
        if strcmp(char(Tstmols(j,:)),char(rownames2{k}))
            Predict(k).result(ind) = Tstdat(j,3);
            ind = ind + 1;
        end;
    end;
end;    


TrPrd =-999*ones(length(rownames),1);
TstPrd = -999*ones(length(rownames),1);
ValPrd = -999*ones(length(rownames2),1);

for i=1:length(TrPrd)
    if ~isempty(Training(i).result)
        TrPrd(i) = mean(Training(i).result);
    end
    if ~isempty(Test(i).result)
        TstPrd(i) = mean(Test(i).result);
    end
end;

for i=1:length(ValPrd)
    if ~isempty(Predict(i).result)
        ValPrd(i) = mean(Predict(i).result);
    end
end;

for i=1:length(TrPrd)
    nBin = length(cutoff) + 1;
    for nN=1:length(cutoff)
        if TrPrd(i) < cutoff(nN)
            nBin = nN;
            break;
        end
    end
    Trbins(i) = nBin;
end

for i=1:length(TstPrd)
    nBin = length(cutoff) + 1;
    for nN=1:length(cutoff)
        if TstPrd(i) < cutoff(nN)
            nBin = nN;
            break;
        end
    end
    Tstbins(i) = nBin;
end

for i=1:length(ValPrd)
    nBin = length(cutoff) + 1;
    for nN=1:length(cutoff)
        if ValPrd(i) < cutoff(nN)
            nBin = nN;
            break;
        end
    end
    Valbins(i) = nBin;
end

colnames = {'Names', 'log10(1/IC50)' 'Training' 'LSO Test' 'Actual Bin' 'Training Bin' 'LSO Test Bin'};

OffTst = find(TstPrd == -999);
IndTst = 1:length(TstPrd);
IndTst(OffTst) = [];



Datain = [Desc(:,bioact) TrPrd TstPrd Desc(:,bin) Trbins' Tstbins'];
TrCor = sum(Trbins'==Desc(:,bin))/length(Trbins)*100;
TstCor = sum(Tstbins(IndTst)'==Desc(IndTst,bin))/length(IndTst)*100;
if ~isempty(Desc2)
    IndVal = 1:length(ValPrd);
    IndVal(OffVal) = [];
    OffVal = find(Desc2(:,bioact) == -999);
    Datain2 = [Desc2(:,tstbioact) ValPrd Desc2(:,tstbin) Valbins'];
    PrdCor = sum(Valbins(IndVal)'==Desc2(IndVal,tstbin))/length(Valbins)*100;
else
    PrdCor = [];
    Valbins = [];
    Datain2 = [];
%     OffVal = [];
    IndVal = [];
end;

% confusion matrix
for ii=1:binnum 
    for jj=1:binnum
        confusion_tr(ii,jj) = sum((Trbins' == jj).*(Desc(:,bin) == ii));
        confusion_tst(ii,jj) = sum((Tstbins(IndTst)' == jj).*(Desc(IndTst,bin) == ii));
        if ~isempty(Desc2)
           confusion_val(ii,jj) = sum((Valbins(IndVal)' == jj).*(Desc2(IndVal,tstbin) == ii));
        else
           confusion_val = [];
        end;   
    end;
end;  

fid = fopen(statfile,'w');

fprintf(fid,'%s\t','Descriptors: ');

fprintf(fid,'%s\n',char(ftext(4)));

fprintf(fid,'%s\t%f\n','Training %Correct',TrCor);
fprintf(fid,'%s\t%f\n','LSO Test %Correct',TstCor);
fprintf(fid,'%s\t%f\n','Validation %Correct',PrdCor);
fprintf(fid,'\n');

StrActtr = [{'Trainig'}];
StrObstr = [{'Training'}];
for i=1:binnum
   StrActtr = [StrActtr ;{strcat('A',num2str(i))}];
   StrObstr = [StrObstr ; {strcat('O',num2str(i))}];
end;

StrActtst = StrActtr;
StrActtst(1) = {'Test'};
StrObstst = StrObstr;
StrObstst(1) = {'Test'};

StrActVal = StrActtr;
StrActVal(1) = {'Validation'};
StrObsVal = StrObstr;
StrObsVal(1) = {'Validation'};

Obs = [StrObstr;{'   '};StrObstst]; 

for i=1:length(Obs)-1
 fprintf(fid,'%s\t',char(Obs(i)));
end;
fprintf(fid,'%s\n',char(Obs(end)));

for i=1:binnum
    fprintf(fid,'%s\t',char(StrActtr(i+1)));
    fprintf(fid,'%d\t',confusion_tr(i,:));
    fprintf(fid,'%s\t','   ');
    fprintf(fid,'%s\t',char(StrActtst(i+1)));
    fprintf(fid,'%d\t',confusion_tst(i,:));
    fprintf(fid,'\n');
end;
fprintf(fid,'\n');


if ~isempty(Desc2)
    for i=1:length(StrObsVal)-1
     fprintf(fid,'%s\t',char(StrObsVal(i)));
    end;
    fprintf(fid,'%s\n',char(StrObsVal(end)));
    for i=1:binnum
        fprintf(fid,'%s\t',char(StrActVal(i+1)));
        fprintf(fid,'%d\t',confusion_val(i,:));
        fprintf(fid,'\n');
    end;
    fprintf(fid,'\n');
end;

for i=1:length(colnames)-1
 fprintf(fid,'%s\t',char(colnames(i)));
end;
fprintf(fid,'%s\n',char(colnames(end)));

for i=1:length(rownames)
 fprintf(fid,'%s\t',char(rownames(i)));
 fprintf(fid,'%f\t',Datain(i,:));
 fprintf(fid,'\n');
end;
fprintf(fid,'\n');
fprintf(fid,'\n');

if ~isempty(Desc2)
    fprintf(fid,'%s\n','Predictions');
    colnames = {'Names', 'Actual' 'Predicted' 'Actual Bin' 'Prediction'};

    for i=1:length(colnames)-1
     fprintf(fid,'%s\t',char(colnames(i)));
    end;
    fprintf(fid,'%s\n',char(colnames(end)));

    for i=1:length(rownames2)
     fprintf(fid,'%s\t',char(rownames2(i)));
     fprintf(fid,'%f\t',Datain2(i,:));
     fprintf(fid,'\n');
    end;
end;
fclose(fid);

