function varargout = partition(varargin)
% PARTITION M-file for partition.fig
%      PARTITION, by itself, creates a new PARTITION or raises the existing
%      singleton*.
%
%      H = PARTITION returns the handle to a new PARTITION or the handle to
%      the existing singleton*.
%
%      PARTITION('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in PARTITION.M with the given input arguments.
%
%      PARTITION('Property','Value',...) creates a new PARTITION or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before partition_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to partition_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Yun-De Xiao, March 3, 2003

% Last Modified by GUIDE v2.5 03-Mar-2003 17:27:14

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @partition_OpeningFcn, ...
                   'gui_OutputFcn',  @partition_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin & isstr(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT

% Set up global variables for user data
global userdata;

% --- Executes just before partition is made visible.
function partition_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to partition (see VARARGIN)

% Choose default command line output for partition
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes partition wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = partition_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in btnCmpAll.
function btnCmpAll_Callback(hObject, eventdata, handles)
global partit_ud;
index_selected = (1: length(partit_ud.cmpnames));
set(handles.lstCmpName, 'Value', index_selected);
InfUpdate(handles);

% --- Executes on button press in btnCmpInvert.
function btnCmpInvert_Callback(hObject, eventdata, handles)
global partit_ud;
index_new = (1: length(partit_ud.cmpnames));
index_selected = get(handles.lstCmpName, 'Value'); %Current selected compounds
index_new(index_selected) = []; %After inverse, selected compounds
set(handles.lstCmpName, 'Value', index_new);
InfUpdate(handles);

% --- Executes on button press in btnVarAll.
function btnVarAll_Callback(hObject, eventdata, handles)
global partit_ud;
index_selected = (1: length(partit_ud.xnames));
set(handles.lstVarName, 'Value', index_selected);
InfUpdate(handles);


% --- Executes on button press in btnVarInvert.
function btnVarInvert_Callback(hObject, eventdata, handles)
global partit_ud;
index_new = (1: length(partit_ud.xnames));
index_selected = get(handles.lstVarName, 'Value'); %Current selected variables
index_new(index_selected) = []; %After inverse, selected variables
set(handles.lstVarName, 'Value', index_new);
InfUpdate(handles);

% --- Executes during object creation, after setting all properties.
function lstCmpName_CreateFcn(hObject, eventdata, handles)
% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in lstCmpName.
function lstCmpName_Callback(hObject, eventdata, handles)
InfUpdate(handles);
% Hints: contents = get(hObject,'String') returns lstCmpName contents as cell array
%        contents{get(hObject,'Value')} returns selected item from lstCmpName


% --- Executes during object creation, after setting all properties.
function lstVarName_CreateFcn(hObject, eventdata, handles)
% Hint: listbox controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in lstVarName.
function lstVarName_Callback(hObject, eventdata, handles)
InfUpdate(handles);
% Hints: contents = get(hObject,'String') returns lstVarName contents as cell array
%        contents{get(hObject,'Value')} returns selected item from lstVarName


% --- Executes on button press in popVarName.
function popVarName_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of popVarName
global partit_ud;
if isempty(partit_ud)
	return;
end
index = get(handles.popVarName, 'Value');
set(handles.edtMin, 'String', num2str(min(partit_ud.x(:, index))));
set(handles.edtMax, 'String', num2str(max(partit_ud.x(:, index))));
set(handles.edtMean, 'String', num2str(mean(partit_ud.x(:, index))));
set(handles.edtStd, 'String', num2str(std(partit_ud.x(:, index))));

% --- Executes during object creation, after setting all properties.
function edtMin_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtMin_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtMin as text
%        str2double(get(hObject,'String')) returns contents of edtMin as a double


% --- Executes during object creation, after setting all properties.
function edtMax_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtMax_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtMax as text
%        str2double(get(hObject,'String')) returns contents of edtMax as a double

% --- Executes during object creation, after setting all properties.
function edtMean_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtMean_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtMean as text
%        str2double(get(hObject,'String')) returns contents of edtMean as a double

% --- Executes during object creation, after setting all properties.
function edtStd_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtStd_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtStd as text
%        str2double(get(hObject,'String')) returns contents of edtStd as a double


% --- Executes on button press in chkMask.
function chkMask_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of chkMask


% --- Executes on button press in chkTrTe.
function chkTrTe_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of chkTrTe


% --- Executes on button press in chkSelVar.
function chkSelVar_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of chkSelVar


% --- Executes on button press in btnQuit.
function btnQuit_Callback(hObject, eventdata, handles)
close;


% --- Executes on button press in radPercentage.
function radPercentage_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of radPercentage
PercentageNumCheck(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edtPercentage_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtPercentage_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtPercentage as text
%        str2double(get(hObject,'String')) returns contents of edtPercentage as a double
global partit_ud;
percentage = str2num(get(hObject, 'String'));
if isempty(percentage) || percentage < 0 || percentage > 100
    disp('Please enter numeric 0-100');
    return;
end
number = percentage * length(partit_ud.cmpnames);
set(handles.edtNumber, 'String', num2str(round(number/100)));
PercentageNumCheck(handles.radPercentage, handles);

% --- Executes on button press in radNumber.
function radNumber_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of radNumber
PercentageNumCheck(hObject, handles);

% --- Executes during object creation, after setting all properties.
function edtNumber_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtNumber_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtNumber as text
%        str2double(get(hObject,'String')) returns contents of edtNumber as a double
global partit_ud;
number = str2num(get(hObject, 'String'));
totalNum = length(partit_ud.cmpnames);
if isempty(number) || number < 0 || number > totalNum
    disp(['Please enter numeric 0-', num2str(totalNum)]);
    return;
end
percentage = round(number/totalNum * 1000)/10;
set(handles.edtPercentage, 'String', num2str(percentage));
PercentageNumCheck(handles.radNumber, handles);

% --- Executes on button press in checkbox6.
function checkbox6_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of checkbox6


% --- Executes during object creation, after setting all properties.
function edit8_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit8_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edit8 as text
%        str2double(get(hObject,'String')) returns contents of edit8 as a double


% --- Executes during object creation, after setting all properties.
function edtSets_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtSets_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtSets as text
%        str2double(get(hObject,'String')) returns contents of edtSets as a double


% --- Executes on button press in checkbox7.
function checkbox7_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of checkbox7


% --- Executes on button press in checkbox8.
function checkbox8_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of checkbox8


% --- Executes on button press in checkbox9.
function checkbox9_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of checkbox9


% --- Executes on button press in checkbox10.
function checkbox10_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of checkbox10


% --- Executes during object creation, after setting all properties.
function edtMask_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtMask_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtMask as text
%        str2double(get(hObject,'String')) returns contents of edtMask as a double
str = get(hObject, 'String');
if ischar(str) || ~isempty(str)
    set(handles.chkMask, 'Value', 1);
end

% --- Executes during object creation, after setting all properties.
function edtTrTe_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtTrTe_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtTrTe as text
%        str2double(get(hObject,'String')) returns contents of edtTrTe as a double
str = get(hObject, 'String');
if ischar(str) || ~isempty(str)
    set(handles.chkTrTe, 'Value', 1);
end


% --- Executes on button press in radRand.
function radRand_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of radRand
DistributionCheck(hObject, handles);

% --- Executes on button press in radBio.
function radBio_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of radBio
DistributionCheck(hObject, handles);

% --- Executes on button press in radChemo.
function radChemo_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of radChemo
DistributionCheck(hObject, handles);

% --- Executes on button press in radMix.
function radMix_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of radMix
DistributionCheck(hObject, handles);

% --- Executes on button press in btnName.
function btnName_Callback(hObject, eventdata, handles)
% Get the specify file name
[pname] = GetFileName;
set(handles.edtName, 'String', pname);

% --- Executes during object creation, after setting all properties.
function edtName_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtName_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtName as text
%        str2double(get(hObject,'String')) returns contents of edtName as a double


% --- Executes on button press in btnLoad.
function btnLoad_Callback(hObject, eventdata, handles)
global partit_ud;
tname = get(handles.edtName, 'String');
% Check whether the selected file is a valid one.
if ischar(tname) & (~isempty(tname))
    [partit_ud.cmpnames, partit_ud.xnames partit_ud.x] = dataimport(tname);
    szTitle = partit_ud.cmpnames(1);
    partit_ud.cmpnames(1) = [];
    partit_ud.xnames(1) = [];
    tStd = std(partit_ud.x);
    index = find(tStd < 1.0e-6);
    partit_ud.x(:, index) = [];
    partit_ud.xnames(index) = [];
    partit_ud.y = [];
    set(handles.lstCmpName, 'String', partit_ud.cmpnames);
    set(handles.lstVarName, 'String', partit_ud.xnames);
    set(handles.popVarName, 'String', partit_ud.xnames);
    InfUpdate(handles);  % Update the information
    number = str2num(get(handles.edtPercentage, 'String')) * length(partit_ud.cmpnames);
    set(handles.edtNumber, 'String', num2str(floor(number/100)));
    set(handles.btnRun, 'Enable', 'on');
end
    
% --- Executes on button press in btnProperty.
function btnProperty_Callback(hObject, eventdata, handles)
global partit_ud;
index = get(handles.lstVarName, 'Value');
if length(index) ~= 1
    disp('select one and only one variable');
    return;
end
partit_ud.y = partit_ud.x(:, index);
partit_ud.x(:, index) = [];
partit_ud.xnames(index) = [];
set(handles.lstVarName, 'String', partit_ud.xnames);
set(handles.popVarName, 'String', partit_ud.xnames);
InfUpdate(handles);

% ----------------------------------------------------
% Get the valid file name
function [pname] = GetFileName()
% Ask user to select the file by popping up a load file window
[FileName, FilePath] = uigetfile({'*.dat; *.txt', 'Data Files (*.dat, *.txt}'; ...
    '*.mod', 'Model Files (*.mod)'; '*.out', 'Output Files (*.out)'; ...
    '*.*', 'All Files (*.*)';}, 'Pick a file');

%Check whether the selected file name is a valid one.
if ischar(FileName) & (~isempty(FileName));
    % cat the user selectd filename with its path
    pname = strcat(FilePath, FileName);
else
    pname = [];
end

% -----------------------------------------------------
% update the information
function InfUpdate(handles)
% get the total number of compunds and variables
global partit_ud;
[nCmpNum nVarNum] = size(partit_ud.x);
m = size(get(handles.lstCmpName, 'Value'), 2);
n = size(get(handles.lstVarName, 'Value'), 2);
str = ['compounds: ' num2str(nCmpNum) ' (' num2str(m) ')    Variables: ' ...
        num2str(nVarNum) ' (' num2str(n) ')'];
set(handles.txtInf, 'String', str);

% --- Executes on button press in btnRun.
function btnRun_Callback(hObject, eventdata, handles)
global partit_ud;

% Get mask file name if checked and check if it is a valid name
if get(handles.chkMask, 'Value') == 1
    mskName = get(handles.edtMask, 'String');
    if ~ischar(mskName) || isempty(mskName)
        disp('Please enter a valid mask file name or uncheck mask file');
        return;
    end
end
% Get Training and test file base name if checked and check if it is a
% valid name
if get(handles.chkTrTe, 'Value') == 1
    trteBase = get(handles.edtTrTe, 'String');
    if ~ischar(trteBase) || isempty(trteBase)
        disp('Please enter a valid training/test base name or uncheck it');
        return;
    end
end

% At least one kind of output file is checked
if get(handles.chkMask, 'Value') == 0 && get(handles.chkTrTe, 'Value') == 0
    disp('Please check at least one output file!');
    return;
end

% Get the global parameter
xdata = partit_ud.x;
ydata = partit_ud.y;
xnames = partit_ud.xnames;
cmpnames = partit_ud.cmpnames;
[nCmpNum, nVarNum] = size(xdata);

% If Biodistibution or Mix distribution is checked, the biologic property
% matrix can not be empty
if get(handles.radBio, 'Value') == 1 || get(handles.radMix, 'Value') == 1
    if isempty(ydata)
        disp('Please set property or check Random or ChemoDistribution!');
        return;
    end
end    

% Get the set number
nSets = str2num(get(handles.edtSets, 'String'));
if isempty(nSets) || nSets < 1 || nSets > nCmpNum
    disp(['Please enter a valid sets number: 1 - ', num2str(nCmpNum)]);
    return;
end

% Get the number of test set
nNumber = str2num(get(handles.edtNumber, 'String'));

% If only use the selected variables
if get(handles.chkSelVar, 'Value') == 1
    xdata = xdata(:, get(handles.lstVarName, 'Value'));
    xnames = xnames(get(handles.lstVarName, 'Value'));
    [nCmpNum, nVarNum] = size(xdata);
end

% Define the group number which the data set will be splited
nGroup = 4;
if nCmpNum <= 4
    disp('This program can not partition the data set with less five members');
    return;
elseif nCmpNum > 250
    nGroup = 10;
elseif nCmpNum > 150
    nGroup = 9;
elseif nCmpNum > 100
    nGroup = 8;
elseif nCmpNum > 80
    nGroup = 7;
elseif nCmpNum > 60
    nGroup = 6;
elseif nCmpNum > 30
    nGroup = 5;
else
    nGroup = 4;
end

% Generate the index accoding to the specify property
% Random - randomly, BioRang - based on property distibution, ChemoDistibution -
% based on the first principal component in chemical descriptor space
Ix = [];        % Index of the compounds
if get(handles.radRand, 'Value') == 1
    Ix = (1:nCmpNum).';
elseif get(handles.radBio, 'Value') == 1
    [B Ix] = sort(ydata);
elseif get(handles.radChemo, 'Value') == 1 || get(handles.radMix, 'Value') == 1
    % Standardize the data by dividing each column by its standard
    % deviation
    stdr = std(xdata);
    sr = xdata./stdr(ones(nCmpNum,1),:);
    [pcs, newdata] = princomp(sr);
    [B Ix] = sort(newdata(:, 1));   % Sort only the first principal component
    if get(handles.radMix, 'Value') == 1
        [B Jx] = sort(Ix);
        [B Ix] = sort(ydata);
        nTmpGroup = round(nGroup/2);
        nTmpEachGroup = floor(nCmpNum/nTmpGroup);
        nTmpLeft = mod(nCmpNum, nTmpGroup);
        nTmpBg = 1;
        for nTmp=1:nTmpGroup
            nTmpEnd = nTmpBg + nTmpEachGroup - 1;
            if nTmp < nTmpLeft
                nTmpEnd = nTmpEnd + 1;
            end
            nTmpId = Jx(Ix(nTmpBg:nTmpEnd));
            [b tmpJx] = sort(nTmpId);
            tmpFinal = Ix(nTmpBg:nTmpEnd);
            tmpFinal = tmpFinal(tmpJx);
            Ix(nTmpBg:nTmpEnd) = tmpFinal;
            nTmpBg = nTmpEnd + 1;
        end
    end
end

% Generate training and test sets
nTotalLeft = mod(nCmpNum, nGroup);  % the fraction of total member
nTotalGroupEach = floor(nCmpNum/nGroup); % the average number for each group
nTestLeft = mod(nNumber, nGroup);  % the fraction of test set
nTestGroupEach = floor(nNumber/nGroup); % the average number for each group in test set

setmask = zeros(nCmpNum, nSets);    % 1 means in test set and 0 means in training set

for nSet = 1:nSets
    index = [];
    for i=1:nGroup
        nTotalOffset = 0;
        if i<=nTotalLeft
            nTotalOffset = 1;
        end
        nTestOffset = 0;
        if i<=nTestLeft
            nTestOffset = 1;
        end
        % generate rand number
        randNums = randperm(nTotalGroupEach + nTotalOffset);
        randNums = sort(randNums(1:nTestGroupEach + nTestOffset));
        if i <= nTotalLeft
            randNums = randNums + ((i - 1) * nTotalGroupEach + i - 1);
        else
            randNums = randNums + ((i - 1) * nTotalGroupEach + nTotalLeft);
        end
        index = [index, randNums];
    end
    AdjustIndex = Ix(index);
    setmask(AdjustIndex, nSet) = 1;
end

% Write to the mask file if it is checked
if get(handles.chkMask, 'Value') == 1
    fMask = fopen(mskName, 'w');
    fprintf(fMask, '%s', 'Name');
    for nSet=1:nSets
        fprintf(fMask, '\t%s', char(['P', num2str(nSet)]));
    end
    fprintf(fMask, '\n');
    for nCmp=1:nCmpNum
        fprintf(fMask, '%s', char(cmpnames(nCmp)));
        for nSet=1:nSets
            fprintf(fMask, '\t%6d', setmask(nCmp, nSet));
        end
        fprintf(fMask, '\n');
    end
    fclose(fMask);
end

% Write to the training/test set files if it is checked
if get(handles.chkTrTe, 'Value') == 1
    for nSet=1:nSets
        % training set file name
        szTrName = [trteBase, '_tr_', num2str(nSet), '.dat'];
        % test set file name
        szTeName = [trteBase, '_te_', num2str(nSet), '.dat'];
        fTrain = fopen(szTrName, 'w');
        fTest = fopen(szTeName, 'w');
        fprintf(fTrain, '%s', 'Name');
        fprintf(fTest, '%s', 'Name');
        if ~isempty(ydata)
            fprintf(fTrain, '\t%s', 'Activity');
            fprintf(fTest, '\t%s', 'Activity');
        end
        for nVar=1:nVarNum
            fprintf(fTrain, '\t%s', char(xnames(nVar)));
            fprintf(fTest, '\t%s', char(xnames(nVar)));
        end
        fprintf(fTrain, '\n');
        fprintf(fTest, '\n');
        for nCmp=1:nCmpNum
            if setmask(nCmp, nSet) == 0
                fprintf(fTrain, '%s', char(cmpnames(nCmp)));
                if ~isempty(ydata)
                    fprintf(fTrain, '\t%12e', ydata(nCmp));
                end
                for nVar=1:nVarNum
                    fprintf(fTrain, '\t%12e', xdata(nCmp, nVar));
                end
                fprintf(fTrain, '\n');
            else
                fprintf(fTest, '%s', char(cmpnames(nCmp)));
                if ~isempty(ydata)
                    fprintf(fTest, '\t%12e', ydata(nCmp));
                end
                for nVar=1:nVarNum
                    fprintf(fTest, '\t%12e', xdata(nCmp, nVar));
                end
                fprintf(fTest, '\n');
            end
        end
        fclose(fTrain);
        fclose(fTest);
    end
end


% ------------------------------------------------
function PercentageNumCheck(hObject, handles)
% hObject   handle that be checked
set(handles.radPercentage, 'Value', 0);
set(handles.radNumber, 'Value', 0);
set(hObject, 'Value', 1);

% --- Executes on button press in any distribution radio button
% and check the pressed butten and set the other buttons to unchecked
function DistributionCheck(hObject, handles)
% hObject   handle taht be checked
set(handles.radRand, 'Value', 0);
set(handles.radBio, 'Value', 0);
set(handles.radChemo, 'Value', 0);
set(handles.radMix, 'Value', 0);
set(hObject, 'Value', 1);