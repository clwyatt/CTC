function modelreset(obj,ev,varargin)
% QSAR tool's general parameter setup panel
%
% It takes the iniital setup values from initial setup function and 
% resets the values
global Mdat;
global userdata;

% Modeling method
method = get(Mdat.hgui.method, 'String');
method = deblank(method(get(Mdat.hgui.method, 'Value'),:));
switch method
case {'CM','FCM','kNN','MDS','MLR','PCR','SVM'}
    
case 'NNET'
    set(Mdat.hMsetgui.nntype, 'Value',1);
    set(Mdat.hMsetgui.nntraining, 'Value',1);
    set(Mdat.hMsetgui.nnfunction, 'Value',1);
    set(Mdat.hMsetgui.nnmethod, 'Value',1);
    set(Mdat.hMsetgui.hiddenstart, 'String',3);
    set(Mdat.hMsetgui.hiddenstop, 'String',7);
    set(Mdat.hMsetgui.hiddenstart2, 'String',3);
    set(Mdat.hMsetgui.hiddenstop2, 'String',7);
    set(Mdat.hMsetgui.nnfunction, 'Value',1);
    set(Mdat.hMsetgui.epoch, 'String',100);
    
case 'SOM'  
    set(Mdat.hMsetgui.somtype, 'Value',1);  
end;    
