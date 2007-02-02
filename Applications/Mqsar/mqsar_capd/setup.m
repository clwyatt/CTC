function varargout = setup(varargin)
% SETUP M-file for setup.fig
%      SETUP, by itself, creates a new SETUP or raises the existing
%      singleton*.
%
%      H = SETUP returns the handle to a new SETUP or the handle to
%      the existing singleton*.
%
%      SETUP('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in SETUP.M with the given input arguments.
%
%      SETUP('Property','Value',...) creates a new SETUP or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before setup_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to setup_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES


% Yun-De Xiao, March 25, 2003
% Last Modified by GUIDE v2.5 15-Jun-2004 14:28:09

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @setup_OpeningFcn, ...
                   'gui_OutputFcn',  @setup_OutputFcn, ...
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
global userdata; 

% --- Executes just before setup is made visible.
function setup_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% varargin   command line arguments to setup (see VARARGIN)
setup_initial(handles);
% Choose default command line output for setup
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes setup wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = setup_OutputFcn(hObject, eventdata, handles)
% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in btnOK.
function btnOK_Callback(hObject, eventdata, handles)
global userdata;
userdata.Scaling = get(handles.popScaling, 'Value');
userdata.ComponNum = str2num(get(handles.edtComponNum, 'String'));
userdata.Val = get(handles.popVal, 'Value');
userdata.Nonlinear = get(handles.chkNonlinear, 'Value');
userdata.Square = get(handles.chkSquare, 'Value');
userdata.Exp = get(handles.chkExp, 'Value');
userdata.Inverse = get(handles.chkInverse, 'Value');
userdata.Log = get(handles.chkLog, 'Value');
userdata.Fin = str2num(get(handles.edtFin, 'String'));
userdata.Fout = str2num(get(handles.edtFout, 'String'));
userdata.ChkCluster = get(handles.chkCluster, 'Value');
userdata.Cluster = get(handles.popCluster, 'Value');
userdata.Dist = get(handles.popDist, 'Value');
%userdata.Population = str2num(get(handles.edtSubsetPopulation, 'String'));
%userdata.Generation = str2num(get(handles.edtGeneration, 'String'));

% Ersin Bayram 
userdata.Descriptors = str2num(get(handles.edtDescriptors, 'String'));
userdata.SubsetPopulation = str2num(get(handles.edtSubsetPopulation, 'String'));
userdata.Generation = str2num(get(handles.edtGeneration, 'String'));
userdata.Subsets = str2num(get(handles.edtSubsets, 'String'));
userdata.PoolFreq = str2num(get(handles.edtPoolFreq, 'String'));
userdata.PoolDur = str2num(get(handles.edtPoolDur, 'String'));
userdata.MaxMutate = str2num(get(handles.edtMaxMutate, 'String'));
userdata.MinMutate = str2num(get(handles.edtMinMutate, 'String'));
userdata.MaxCrossOver = str2num(get(handles.edtMaxCrossOver, 'String'));
userdata.MinCrossOver = str2num(get(handles.edtMinCrossOver, 'String'));
userdata.Elites = str2num(get(handles.edtElites, 'String'));
userdata.Pmutation = str2num(get(handles.edtPmutation, 'String'));
userdata.Pcross = str2num(get(handles.edtPcross, 'String'));
userdata.MoveTarget = get(handles.chkMoveTarget, 'Value');
userdata.EliteDuration = str2num(get(handles.edtEliteDuration, 'String'));
userdata.CorrFiltThreshold = str2num(get(handles.edtCorrFilter, 'String'));
userdata.StdFiltThreshold = str2num(get(handles.edtStdFilter, 'String'));
userdata.ClassBins = str2num(get(handles.edtClassBins, 'String'));

userdata.StartT = str2num(get(handles.edtStartT, 'String'));
userdata.EndT = str2num(get(handles.edtEndT, 'String'));
userdata.Droprate = str2num(get(handles.edtDroprate, 'String'));
userdata.Cycle = str2num(get(handles.edtCycle, 'String'));
userdata.Mutate = str2num(get(handles.edtMutate, 'String'));
userdata.ChkFixLen = get(handles.chkFixLen, 'Value');
userdata.FixLen = str2num(get(handles.edtFixLen, 'String'));
userdata.Norm = get(handles.chkNorm, 'Value');
close;

% --- Executes on button press in btnCancel.
function btnCancel_Callback(hObject, eventdata, handles)
close;


% --- Executes during object creation, after setting all properties.
function popScaling_CreateFcn(hObject, eventdata, handles)
% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popScaling.
function popScaling_Callback(hObject, eventdata, handles)
% Hints: contents = get(hObject,'String') returns popScaling contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popScaling


% --- Executes during object creation, after setting all properties.
function edtComponNum_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtComponNum_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtComponNum as text
%        str2double(get(hObject,'String')) returns contents of edtComponNum as a double


% --- Executes during object creation, after setting all properties.
function popVal_CreateFcn(hObject, eventdata, handles)
% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popVal.
function popVal_Callback(hObject, eventdata, handles)
% Hints: contents = get(hObject,'String') returns popVal contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popVal


% --- Executes on button press in chkNonlinear.
function chkNonlinear_Callback(hObject, eventdata, handles)
if get(handles.chkNonlinear, 'Value') 
    nValue = 1;
else 
    nValue = 0;
end
set(handles.chkSquare, 'Value', nValue);
set(handles.chkExp, 'Value', nValue);
set(handles.chkInverse, 'Value', nValue);
set(handles.chkLog, 'Value', nValue);
% Hint: get(hObject,'Value') returns toggle state of chkNonlinear

% --- Executes on button press in chkSquare.
function chkSquare_Callback(hObject, eventdata, handles)
check_nonlinear(handles);
% Hint: get(hObject,'Value') returns toggle state of chkSquare

% --- Executes on button press in chkExp.
function chkExp_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of chkExp
check_nonlinear(handles);

% --- Executes on button press in chkInverse.
function chkInverse_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of chkInverse
check_nonlinear(handles);

% --- Executes on button press in chkLog.
function chkLog_Callback(hObject, eventdata, handles)
check_nonlinear(handles);
% Hint: get(hObject,'Value') returns toggle state of chkLog


% --- Executes during object creation, after setting all properties.
function edtFin_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtFin_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtFin as text
%        str2double(get(hObject,'String')) returns contents of edtFin as a double


% --- Executes during object creation, after setting all properties.
function edtFout_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtFout_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtFout as text
%        str2double(get(hObject,'String')) returns contents of edtFout as a double


% --- Executes on button press in chkFixLen.
function chkFixLen_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of chkFixLen


% --- Executes during object creation, after setting all properties.
function edtFixLen_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtFixLen_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtFixLen as text
%        str2double(get(hObject,'String')) returns contents of edtFixLen as a double


% --- Executes on button press in chkNorm.
function chkNorm_Callback(hObject, eventdata, handles)
% Hint: get(hObject,'Value') returns toggle state of chkNorm


% --- Executes during object creation, after setting all properties.
function popCluster_CreateFcn(hObject, eventdata, handles)
% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popCluster.
function popCluster_Callback(hObject, eventdata, handles)
% Hints: contents = get(hObject,'String') returns popCluster contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popCluster


% --- Executes during object creation, after setting all properties.
function popDist_CreateFcn(hObject, eventdata, handles)
% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popDist.
function popDist_Callback(hObject, eventdata, handles)
% Hints: contents = get(hObject,'String') returns popDist contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popDist


% --- Executes during object creation, after setting all properties.
function edtSubsetPopulation_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtSubsetPopulation_Callback(hObject, eventdata, handles)
% Hints: get(hObject,'String') returns contents of edtSubsetPopulation as text
%        str2double(get(hObject,'String')) returns contents of edtSubsetPopulation as a double


% --- Executes during object creation, after setting all properties.
function edtGeneration_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtGeneration_Callback(hObject, eventdata, handles)

% --- Executes on button press in btnDefault.
function btnDefault_Callback(hObject, eventdata, handles)
set(handles.popScaling, 'Value', 1);
set(handles.edtComponNum, 'String', 3);
set(handles.popVal, 'Value', 1);
set(handles.chkNonlinear, 'Value', 0);
set(handles.chkSquare, 'Value', 0);
set(handles.chkExp, 'Value', 0);
set(handles.chkInverse, 'Value', 0);
set(handles.chkLog, 'Value', 0);
set(handles.edtFin, 'String', '4.0');
set(handles.edtFout, 'String', '4.0');
set(handles.chkCluster, 'Value', 0);
set(handles.popCluster, 'Value', 1);
set(handles.popDist, 'Value', 1);


%set(handles.edtSubsetPopulation, 'String', 100);
%set(handles.edtGeneration, 'String', 500);

% Ersin Bayram
set(handles.edtDecriptors, 'String', 30);
set(handles.edtSubsetPopulation, 'String', 60);
set(handles.edtGeneration, 'String', 600);
set(handles.edtSubsets, 'String', 1);
set(handles.edtPoolFreq, 'String', 0);
set(handles.edtPoolDur, 'String', 600);
set(handles.edtMaxMutate, 'String', 10);
set(handles.edtMinMutate, 'String', 2);
set(handles.edtMaxCrossOver, 'String',10);
set(handles.edtMinCrossOver, 'String', 0);
set(handles.edtElites, 'String', 10);
set(handles.edtPmutation, 'String', 0.1);
set(handles.edtPcross, 'String', 0.7);
set(handles.chkMoveTarget, 'Value', 0);
set(handles.edtEliteDuration, 'String' , 10);

set(handles.edtClassBins, 'String', 3);
set(handles.edtCorrFilter, 'String', 1.0);
set(handles.edtStdFilter, 'String' , 0.0);

set(handles.edtStartT, 'String', 100);
set(handles.edtEndT, 'String', 1.0e-6);
set(handles.edtDroprate, 'String', 0.9);
set(handles.edtCycle, 'String', 20);
set(handles.edtMutate, 'String', 2);
set(handles.chkFixLen, 'Value', 0);
set(handles.edtFixLen, 'String', 10);
set(handles.chkNorm, 'Value', 0);

% --- Executes during object creation, after setting all properties.
function edtStartT_CreateFcn(hObject, eventdata, handles)
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtStartT_Callback(hObject, eventdata, handles)
% --- Executes during object creation, after setting all properties.

function edtEndT_CreateFcn(hObject, eventdata, handles)
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtEndT_Callback(hObject, eventdata, handles)
% --- Executes during object creation, after setting all properties.
Tend = str2double(get(hObject,'String'));

if Tend < 0 
    errordlg('Final Temperature must be > 0');
end;    


function edtDroprate_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtDroprate_Callback(hObject, eventdata, handles)

Tstep = str2double(get(hObject,'String'));
% Verify drop rate
if Tstep >= 1
	disp('the temperature drop rate should be between 0-1');
	return;
end

% --- Executes during object creation, after setting all properties.
function edtCycle_CreateFcn(hObject, eventdata, handles)
% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtCycle_Callback(hObject, eventdata, handles)

% --- Executes during object creation, after setting all properties.
function edtMutate_CreateFcn(hObject, eventdata, handles)
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtMutate_Callback(hObject, eventdata, handles)
% --- if any nonlinear parts is checked

function check_nonlinear(handles)
if get(handles.chkSquare, 'Value') || get(handles.chkExp, 'Value') || ...
        get(handles.chkInverse, 'Value') || get(handles.chkLog, 'Value')
    set(handles.chkNonlinear, 'Value', 1);
else
    set(handles.chkNonlinear, 'Value', 0);
end

% --- Initialize all parameters using global arguments
function setup_initial(handles)
global userdata;
set(handles.popScaling, 'Value', userdata.Scaling);
set(handles.edtComponNum, 'String', userdata.ComponNum);
set(handles.popVal, 'Value', userdata.Val);
set(handles.chkNonlinear, 'Value', userdata.Nonlinear);
set(handles.chkSquare, 'Value', userdata.Square);
set(handles.chkExp, 'Value', userdata.Exp);
set(handles.chkInverse, 'Value', userdata.Inverse);
set(handles.chkLog, 'Value', userdata.Log);
set(handles.edtFin, 'String', userdata.Fin);
set(handles.edtFout, 'String', userdata.Fout);
set(handles.chkCluster, 'Value', userdata.ChkCluster);
set(handles.popCluster, 'Value', userdata.Cluster);
set(handles.popDist, 'Value', userdata.Dist);

%set(handles.edtSubsetPopulation, 'String', userdata.Population);
%set(handles.edtGeneration, 'String', userdata.Generation);

% Ersin Bayram
set(handles.edtDescriptors, 'String', userdata.Descriptors);
set(handles.edtSubsetPopulation, 'String', userdata.SubsetPopulation);
set(handles.edtGeneration, 'String', userdata.Generation);
set(handles.edtSubsets, 'String', userdata.Subsets);
set(handles.edtPoolFreq, 'String', userdata.PoolFreq);
set(handles.edtPoolDur, 'String', userdata.PoolDur);
set(handles.edtMaxMutate, 'String', userdata.MaxMutate);
set(handles.edtMinMutate, 'String', userdata.MinMutate);
set(handles.edtMaxCrossOver, 'String', userdata.MaxCrossOver);
set(handles.edtMinCrossOver, 'String', userdata.MinCrossOver);
set(handles.edtElites, 'String', userdata.Elites);
set(handles.edtPmutation, 'String', userdata.Pmutation);
set(handles.edtPcross, 'String', userdata.Pcross);
set(handles.chkMoveTarget, 'Value', userdata.MoveTarget);
set(handles.edtEliteDuration, 'String' ,userdata.EliteDuration);

set(handles.edtClassBins, 'String', userdata.ClassBins);
set(handles.edtCorrFilter, 'String', userdata.CorrFiltThreshold);
set(handles.edtStdFilter, 'String' ,userdata.StdFiltThreshold);
set(handles.chkFixInitial, 'Value', userdata.chkFixInitial);


set(handles.edtStartT, 'String', userdata.StartT);
set(handles.edtEndT, 'String', userdata.EndT);
set(handles.edtDroprate, 'String', userdata.Droprate);
set(handles.edtCycle, 'String', userdata.Cycle);
set(handles.edtMutate, 'String', userdata.Mutate);
set(handles.chkFixLen, 'Value', userdata.ChkFixLen);
set(handles.edtFixLen, 'String', userdata.FixLen);
set(handles.chkNorm, 'Value', userdata.Norm);


% --- Executes during object creation, after setting all properties.
function editSubsets_CreateFcn(hObject, eventdata, handles)
% hObject    handle to editSubsets (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function editSubsets_Callback(hObject, eventdata, handles)
% hObject    handle to editSubsets (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of editSubsets as text
%        str2double(get(hObject,'String')) returns contents of editSubsets as a double


% --- Executes during object creation, after setting all properties.
function edtSubsets_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtSubsets (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtSubsets_Callback(hObject, eventdata, handles)
% hObject    handle to edtSubsets (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtSubsets as text
%        str2double(get(hObject,'String')) returns contents of edtSubsets as a double


% --- Executes during object creation, after setting all properties.
function edtPoolFreq_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtPoolFreq (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtPoolFreq_Callback(hObject, eventdata, handles)
% hObject    handle to edtPoolFreq (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtPoolFreq as text
%        str2double(get(hObject,'String')) returns contents of edtPoolFreq as a double


% --- Executes during object creation, after setting all properties.
function edtPoolDur_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtPoolDur (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtPoolDur_Callback(hObject, eventdata, handles)
% hObject    handle to edtPoolDur (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtPoolDur as text
%        str2double(get(hObject,'String')) returns contents of edtPoolDur as a double


% --- Executes during object creation, after setting all properties.
function edtMaxMutate_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtMaxMutate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtMaxMutate_Callback(hObject, eventdata, handles)
% hObject    handle to edtMaxMutate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtMaxMutate as text
%        str2double(get(hObject,'String')) returns contents of edtMaxMutate as a double


% --- Executes during object creation, after setting all properties.
function edtMinMutate_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtMinMutate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


function edtMinMutate_Callback(hObject, eventdata, handles)
% hObject    handle to edtMinMutate (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtMinMutate as text
%        str2double(get(hObject,'String')) returns contents of edtMinMutate as a double


% --- Executes during object creation, after setting all properties.
function edtMaxCrossOver_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtMaxCrossOver (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtMaxCrossOver_Callback(hObject, eventdata, handles)
% hObject    handle to edtMaxCrossOver (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtMaxCrossOver as text
%        str2double(get(hObject,'String')) returns contents of edtMaxCrossOver as a double


% --- Executes during object creation, after setting all properties.
function edit19_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit19 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit19_Callback(hObject, eventdata, handles)
% hObject    handle to edit19 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit19 as text
%        str2double(get(hObject,'String')) returns contents of edit19 as a double


% --- Executes during object creation, after setting all properties.
function edtMinCrossOver_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtMinCrossOver (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtMinCrossOver_Callback(hObject, eventdata, handles)
% hObject    handle to edtMinCrossOver (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtMinCrossOver as text
%        str2double(get(hObject,'String')) returns contents of edtMinCrossOver as a double


% --- Executes during object creation, after setting all properties.
function edtElites_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtElites (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtElites_Callback(hObject, eventdata, handles)
% hObject    handle to edtElites (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtElites as text
%        str2double(get(hObject,'String')) returns contents of edtElites as a double


% --- Executes during object creation, after setting all properties.
function edtPmutation_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtPmutation (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtPmutation_Callback(hObject, eventdata, handles)
% hObject    handle to edtPmutation (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtPmutation as text
%        str2double(get(hObject,'String')) returns contents of edtPmutation as a double


% --- Executes during object creation, after setting all properties.
function edtPcross_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtPcross (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtPcross_Callback(hObject, eventdata, handles)
% hObject    handle to edtPcross (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtPcross as text
%        str2double(get(hObject,'String')) returns contents of edtPcross as a double


% --- Executes during object creation, after setting all properties.
function edtDescriptors_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtDescriptors (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtDescriptors_Callback(hObject, eventdata, handles)
% hObject    handle to edtDescriptors (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtDescriptors as text
%        str2double(get(hObject,'String')) returns contents of edtDescriptors as a double


% --- Executes on button press in chkMoveTarget.
function chkMoveTarget_Callback(hObject, eventdata, handles)
% hObject    handle to chkMoveTarget (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of chkMoveTarget


% --- Executes during object creation, after setting all properties.
function edtEliteDuration_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtEliteDuration (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtEliteDuration_Callback(hObject, eventdata, handles)
% hObject    handle to edtEliteDuration (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtEliteDuration as text
%        str2double(get(hObject,'String')) returns contents of edtEliteDuration as a double


% --- Executes during object creation, after setting all properties.
function edtCorrFilter_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtCorrFilter (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtCorrFilter_Callback(hObject, eventdata, handles)
% hObject    handle to edtCorrFilter (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtCorrFilter as text
%        str2double(get(hObject,'String')) returns contents of edtCorrFilter as a double

CorrThres = str2double(get(hObject,'String'));

if CorrThres < 0 | CorrThres > 1
    errordlg('Please enter a number between 0.0 and 1.0');
end;    

% --- Executes during object creation, after setting all properties.
function edtStdFilter_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtStdFilter (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edtStdFilter_Callback(hObject, eventdata, handles)
% hObject    handle to edtStdFilter (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtStdFilter as text
%        str2double(get(hObject,'String')) returns contents of edtStdFilter as a double

StdThres = str2double(get(hObject,'String'));

if StdThres < 0 | StdThres > 1
    errordlg('Please enter a number between 0.0 and 1.0');
end;    


% --- Executes during object creation, after setting all properties.
function edtClassBins_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edtClassBins (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edtClassBins_Callback(hObject, eventdata, handles)
% hObject    handle to edtClassBins (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edtClassBins as text
%        str2double(get(hObject,'String')) returns contents of edtClassBins as a double


% --- Executes on button press in chkFixInitial.
function chkFixInitial_Callback(hObject, eventdata, handles)
% hObject    handle to chkFixInitial (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of chkFixInitial


