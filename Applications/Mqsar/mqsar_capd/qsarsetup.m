function qsarsetup(varargin)
% QSAR toolbox setup main rotuine
% general setup deals with method independent parameters
% modelsetup deals with the QSAR modeling parameters
% varsetup deals with variable selection parameters

global Mdat;
global userdata;

pos = get(Mdat.hgui.mFig,'Position');
generalsetup(pos);
modelsetup(pos);
varselsetup(pos);
