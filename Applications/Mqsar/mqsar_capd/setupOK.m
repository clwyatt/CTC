function setupOK(obj,ev,varargin)
% QSAR tool's general parameter setup panel
%

global Mdat;
global userdata;

userdata.corrfilt = str2num(get(Mdat.hsetgui.corrfilt, 'String'));
userdata.stdfilt = str2num(get(Mdat.hsetgui.stdfilt, 'String'));
% scaling
userdata.scaling = get(Mdat.hsetgui.scaling, 'String');
userdata.scaling = deblank(userdata.scaling(get(Mdat.hsetgui.scaling, 'Value'),:));
% metric
userdata.metric = get(Mdat.hsetgui.metric, 'String');
userdata.metric = deblank(userdata.metric(get(Mdat.hsetgui.metric, 'Value'),:));
% validation
userdata.validation = get(Mdat.hsetgui.validation, 'String');
userdata.validation = deblank(userdata.validation(get(Mdat.hsetgui.validation, 'Value'),:));
% DescLimit
userdata.DescLimit = get(Mdat.hsetgui.DescLimit, 'String');
userdata.DescLimit = deblank(userdata.DescLimit(get(Mdat.hsetgui.DescLimit, 'Value'),:));

userdata.DescNum = str2num(get(Mdat.hsetgui.DescNum, 'String'));

% Modeling method
method = get(Mdat.hgui.method, 'String');
method = deblank(method(get(Mdat.hgui.method, 'Value'),:));
switch method
case {'CM','FCM','kNN','MDS','MLR','PCR','SVM'}
    
case 'NNET'
    userdata.nntype = get(Mdat.hMsetgui.nntype, 'String');
    userdata.nntype = deblank(userdata.nntype(get(Mdat.hMsetgui.nntype, 'Value'),:));
    
    userdata.nntraining = get(Mdat.hMsetgui.nntraining, 'String');
    userdata.nntraining = deblank(userdata.nntraining(get(Mdat.hMsetgui.nntraining, 'Value'),:));
    
    userdata.hiddenstart = str2num(get(Mdat.hMsetgui.hiddenstart, 'String'));
    userdata.hiddenstop = str2num(get(Mdat.hMsetgui.hiddenstop, 'String'));

    userdata.nnfunction = get(Mdat.hMsetgui.nnfunction, 'String');
    userdata.nnfunction = deblank(userdata.nnfunction(get(Mdat.hMsetgui.nnfunction, 'Value'),:));
    
    userdata.epoch = str2num(get(Mdat.hMsetgui.epoch, 'String'));
    
case 'SOM'  
    userdata.somtype = get(Mdat.hMsetgui.somtype, 'String');
    userdata.somtype = deblank(userdata.somtype(get(Mdat.hMsetgui.somtype, 'Value'),:));
end;    
    
% Variable Selection method
varsel = get(Mdat.hgui.varsel, 'String');
varsel = deblank(varsel(get(Mdat.hgui.varsel, 'Value'),:));
switch varsel
case 'None'
    
case 'GA'
    userdata.minmutate = str2num(get(Mdat.hVsetgui.minmutate, 'String'));
    userdata.maxmutate = str2num(get(Mdat.hVsetgui.maxmutate, 'String'));
    userdata.mincross = str2num(get(Mdat.hVsetgui.mincross, 'String'));
    userdata.maxcross = str2num(get(Mdat.hVsetgui.maxcross, 'String'));
    userdata.pmutate = str2num(get(Mdat.hVsetgui.pmutate, 'String'));
    userdata.pcross = str2num(get(Mdat.hVsetgui.pcross, 'String'));
    userdata.subpop = str2num(get(Mdat.hVsetgui.subpop, 'String'));
    userdata.subno = str2num(get(Mdat.hVsetgui.subno, 'String'));
    userdata.generations = str2num(get(Mdat.hVsetgui.generations, 'String'));
    userdata.elite = str2num(get(Mdat.hVsetgui.elite, 'String'));
    userdata.pooldur = str2num(get(Mdat.hVsetgui.pooldur, 'String'));
    userdata.poolfreq = str2num(get(Mdat.hVsetgui.poolfreq, 'String'));

case 'Hill Climbing'

case 'SA'     
    userdata.tstart = str2num(get(Mdat.hVsetgui.tstart, 'String'));
    userdata.tstop = str2num(get(Mdat.hVsetgui.tstop, 'String'));
    userdata.droprate = str2num(get(Mdat.hVsetgui.droprate, 'String'));
    userdata.cycle = str2num(get(Mdat.hVsetgui.cycle, 'String'));
    userdata.mutation = str2num(get(Mdat.hVsetgui.mutation, 'String'));
case 'Stepwise'    
    
end;     

set(Mdat.hVsetgui.sVFig,'Visible','off')
set(Mdat.hMsetgui.sMFig,'Visible','off')
set(Mdat.hsetgui.sFig,'Visible','off')
    