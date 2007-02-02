function varselreset(obj,ev,varargin)
% QSAR tool's general parameter setup panel
%
% It takes the iniital setup values from initial setup function and 
% resets the values
global Mdat;
global userdata;
userdata = initialsetup(userdata);
varsel = get(Mdat.hgui.varsel, 'String');
varsel = deblank(varsel(get(Mdat.hgui.varsel, 'Value'),:));
switch varsel
case 'None'
    
case 'GA'
    set(Mdat.hVsetgui.minmutate, 'String',num2str(userdata.MinMutate));
    set(Mdat.hVsetgui.maxmutate, 'String',num2str(userdata.MaxMutate));
    set(Mdat.hVsetgui.mincross, 'String',num2str(userdata.MinCrossOver));   
    set(Mdat.hVsetgui.maxcross, 'String',num2str(userdata.MaxCrossOver));
    set(Mdat.hVsetgui.pmutate, 'String',num2str(userdata.Pmutation));
    set(Mdat.hVsetgui.pcross, 'String',num2str(userdata.Pcross));
    set(Mdat.hVsetgui.subpop, 'String',num2str(userdata.SubsetPopulation));
    set(Mdat.hVsetgui.subno, 'String',num2str(userdata.Subsets));
    set(Mdat.hVsetgui.generations, 'String',num2str(userdata.Generation));
    set(Mdat.hVsetgui.elite, 'String',num2str(userdata.Elites));
    set(Mdat.hVsetgui.pooldur, 'String',num2str(userdata.PoolDur));
    set(Mdat.hVsetgui.poolfreq, 'String',num2str(userdata.PoolFreq));
case 'Hill Climbing'
    set(Mdat.hVsetgui.generations, 'String',num2str(userdata.HGeneration));
    set(Mdat.hVsetgui.mutation, 'String',num2str(userdata.Mutate));
case 'SA'     
    set(Mdat.hVsetgui.tstart, 'String',num2str(userdata.StartT));
    set(Mdat.hVsetgui.tstop, 'String',num2str(userdata.EndT));
    set(Mdat.hVsetgui.droprate, 'String',num2str(userdata.Droprate));
    set(Mdat.hVsetgui.cycle, 'String',num2str(userdata.Cycle));   
    set(Mdat.hVsetgui.mutation, 'String',num2str(userdata.Mutate));
case 'Stepwise'    
    
end;     
