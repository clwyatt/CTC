function modelsetup(pos)
% QSAR tool's general parameter setup panel
%
% pos is the position of the main QSAR gui. 

global Mdat;
global userdata;

if ~isfield(Mdat.hMsetgui,'sMFig')

sMFig = figure('Name','Modeling Setup',...
              'NumberTitle','off',...
              'Units','pixels',...
              'Position',[pos(1)+pos(3)+6,pos(2)+pos(4)-218,350,100],...
              'ToolBar','none',...  
              'MenuBar','none',... 
              'Resize','off',...
              'DockControls','off');
set(sMFig,'CloseRequestFcn',{@closesetup,sMFig});


modelPanel = uipanel('Units','pixels',...
                 'ForegroundColor','black','Position',[0,0,350,100]); 
             
% Modeling method
method = get(Mdat.hgui.method, 'String');
method = deblank(method(get(Mdat.hgui.method, 'Value'),:));
switch method
case {'CM','FCM','kNN','MDS','MLR','PCR','SVM'}
    txtnone= uicontrol('Style', 'text', ...
                     'String', 'There is no adjustable parameter', 'Position', [5 20 200 20]);
case 'NNET' 
    txttype= uicontrol('Style', 'text', ...
                             'String', 'Type', 'Position', [1 70 50 20]);    
    handles.nntype = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','Feed Forward|Cascade FF|RBF|Regression|LVQ|Competitive',...
                            'BackgroundColor','white',...
                            'Position',[55,75,110,20]);                    
                            
    txttraining= uicontrol('Style', 'text', ...
                             'String', 'Training', 'Position', [175 70 50 20]);                     
    handles.nntraining = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','Default|Leven. Marq|Grad Descent|GDes Moment|GDes Adapt Lrn|GDes Mom ALrn',...
                            'BackgroundColor','white',...
                            'Position',[230,75,120,20]);
    txtmethod= uicontrol('Style', 'text', ...
                             'String', 'Method', 'Position', [1 35 50 20]);    
    handles.nnmethod = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','Predict|Classify',...
                            'BackgroundColor','white',...
                            'Position',[55,40,70,20]);       
    txttransfunc= uicontrol('Style', 'text', ...
                             'String', 'Neuron Func', 'Position', [1 5 50 30]); %175 35                    
    handles.nnfunction = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','Default|tansig|logsig|purelin',...
                            'BackgroundColor','white',...
                            'Position',[55,10,70,20]);  %230,40                   
                            
    txthidden= uicontrol('Style', 'text', ...
                            'String', 'Hidden1', 'Position', [130 35 50 20]);                     
    handles.hiddenstart = uicontrol('Style', 'edit', 'Units','pixels',...
                             'BackgroundColor','white',...
                            'String',num2str(userdata.hiddenstart),'Position',[182,40,20,20]);  
    txtto= uicontrol('Style', 'text', ...
                            'String', 'to', 'Position', [207 35 25 20]);  
    handles.hiddenstop = uicontrol('Style', 'edit', 'Units','pixels',...
                            'BackgroundColor','white',...
                            'String',num2str(userdata.hiddenstop),'Position',[235,40,20,20]);    
    txthidden2= uicontrol('Style', 'text', ...
                            'String', 'Hidden2', 'Position', [130 5 50 20]);                     
    handles.hiddenstart2 = uicontrol('Style', 'edit', 'Units','pixels',...
                             'BackgroundColor','white',...
                            'String',num2str(userdata.hiddenstart),'Position',[182,10,20,20]);  
    txtto2= uicontrol('Style', 'text', ...
                            'String', 'to', 'Position', [207 5 25 20]);  
    handles.hiddenstop2 = uicontrol('Style', 'edit', 'Units','pixels',...
                            'BackgroundColor','white',...
                            'String',num2str(userdata.hiddenstop),'Position',[235,10,20,20]);                        
    
    txtepoch= uicontrol('Style', 'text', ...
                            'String', 'Epoch', 'Position', [265 35 45 20]);                     
    handles.epoch = uicontrol('Style', 'edit', 'Units','pixels',...
                            'BackgroundColor','white',...
                            'String',num2str(userdata.epoch),'Position',[315,40,30,20]);                         
case 'SOM'
    txttype= uicontrol('Style', 'text', ...
                             'String', 'Type', 'Position', [1 70 50 20]);    
    handles.somtype = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','Supervised|Unsupervised',...
                            'BackgroundColor','white',...
                            'Position',[55,75,100,20]);
    txtsommap= uicontrol('Style', 'text', ...
                             'String', 'Map', 'Position', [160 70 50 20]);    
    handles.sommap = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','big|normal|small',...
                            'BackgroundColor','white',...
                            'Position',[215,75,80,20]);
    txtneighbor= uicontrol('Style', 'text', ...
                             'String', 'Neighbor', 'Position', [1 40 60 20]);    
    handles.neighbor = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','gaussian|cutgauss|ep|bubble',...
                            'BackgroundColor','white',...
                            'Position',[65,45,90,20]);   
    txtshape= uicontrol('Style', 'text', ...
                             'String', 'Shape', 'Position', [160 40 50 20]);    
    handles.shape = uicontrol('Style', 'popupmenu', 'Units','pixels',...
                            'String','sheet|cyl|toroid',...
                            'BackgroundColor','white',...
                            'Position',[215,45,80,20]);                    
end   
%-------------------------                 
% PREPROCESS PANEL
%-------------------------   

handles.sreset = uicontrol(sMFig,'Style', 'pushbutton', ...
     'String', 'Reset', 'Position', [300 1 45 25], ...
     'Callback',@modelreset);   
  
handles.sMFig=sMFig;
Mdat.hMsetgui=handles;

else
    set(Mdat.hMsetgui.sMFig,'Position',[pos(1)+pos(3)+6,pos(2)+pos(4)-218,350,100]);
    set(Mdat.hMsetgui.sMFig,'Visible','on');
end  
