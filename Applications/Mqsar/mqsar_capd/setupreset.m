function setupreset(obj,ev,varargin)
% QSAR tool's general parameter setup panel
%
% It takes the iniital setup values from initial setup function and 
% resets the values
global Mdat;
global userdata;

userdata = initialsetup(userdata);

set(Mdat.hsetgui.corrfilt, 'String',userdata.corrfilt);
set(Mdat.hsetgui.stdfilt, 'String',userdata.stdfilt);

set(Mdat.hsetgui.scaling, 'Value',1);
% metric
set(Mdat.hsetgui.metric, 'Value',1);
% validation
set(Mdat.hsetgui.validation, 'Value',1);
% DescLimit
set(Mdat.hsetgui.DescLimit, 'Value',1);

set(Mdat.hsetgui.DescNum, 'String',userdata.DescNum);
