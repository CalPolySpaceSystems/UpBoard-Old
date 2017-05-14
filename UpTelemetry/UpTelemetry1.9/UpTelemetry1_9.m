%% Introduction
%{


%}

%% Pre start housekeeping 
delete(instrfindall);
%close all;
fclose all;
%panels = '';

%% Form Properties

% Name of this figure, including version
figureName = 'UpTelemetry 1.9';

defaultSize = [35 5 230 55];
minSize = [150 30];
upTelemetry = figure(   'DockControls','off',...
                        'Menubar','none',... 
                        'Name',figureName,...
                        'NumberTitle','off',...
                        'Units','characters',...
                        'Resize','On',... Change Later?
                        'SizeChangedFcn',@sizeChanged,...
                        'Interruptible','on',...
                        'BusyAction','queue',...Figure out how to handle interrupts, queue {@sizeChanged,upTelemetry,panels}
                        'IntegerHandle','off',...
                        'Position',defaultSize,...
                        'Visible','off'); % Don't show yet
                    
%% UI Bar

tbFile = uimenu('Label','File');
    uimenu(tbFile,'Label','Save Configuration...');
    uimenu(tbFile,'Label','Load Configuration...');
    uimenu(tbFile,'Label','Quit','Callback','close(upTelemetry);','Separator','on','Accelerator','Q');
        
tbTools = uimenu('Label','Edit');
    %uimenu(tbTools,'Label','Reset COM Port...');
    uimenu(tbTools,'Label','Serial Settings...');
    uimenu(tbTools,'Label','Parsing Settings...');
    uimenu(tbTools,'Label','KML Converter...');

tbHelp = uimenu('Label','Help');
    uimenu(tbHelp,'Label','Readme');
    uimenu(tbHelp,'Label','About');

%% Serial Monitor Elements
global formElements

formElements.serialCtrl = uipanel('Title','Serial Controls',...
                            'Units','Characters',...
                            'Position',[2 45 70 9]);
     
    txtPort = uicontrol(formElements.serialCtrl,'Style','text',...
                                                'String','Port',...
                                                'HorizontalAlignment','left',...
                                                'Position',[10 70 30 20],...
                                                'Units','Points');
                                            
    txtBaud = uicontrol(formElements.serialCtrl,'Style','text',...
                                                'String','Baud',...
                                                'HorizontalAlignment','left',...
                                                'Position',[10 43 30 20],...
                                                'Units','Points');
                                            
    btnSettings = uicontrol(formElements.serialCtrl,'Style','pushbutton',...
                                                'String','Settings...',...
                                                'HorizontalAlignment','Center',...
                                                'Position',[10 12 105 25],...
                                                'Units','Points');
                                            
    tbxPort = uicontrol(formElements.serialCtrl,'Style','edit',...
                                                'String','',...
                                                'HorizontalAlignment','left',...
                                                'Position',[45 74 70 20],...
                                                'Units','Points');
                                            
    ppmBaud = uicontrol(formElements.serialCtrl,'Style','popupmenu',...
                                                'String',[300,9600],...
                                                'HorizontalAlignment','left',...
                                                'Position',[45 45 70 20],...
                                                'Units','Points');
                                            
    btnToggleSerial = uicontrol(formElements.serialCtrl,'Style','pushbutton',...
                                                'String','Open Serial Port',...
                                                'HorizontalAlignment','Center',...
                                                'Position',[130 72 105 25],...
                                                'Units','Points');
                                            
    btnClearSerial = uicontrol(formElements.serialCtrl,'Style','pushbutton',...
                                                'String','Clear Monitor',...
                                                'HorizontalAlignment','Center',...
                                                'Position',[130 42 105 25],...
                                                'Units','Points');
                                            
    cbxShowStrings = uicontrol(formElements.serialCtrl,'Style','checkbox',...
                                                'String','Display Strings',...
                                                'HorizontalAlignment','left',...
                                                'Position',[250 42 105 25],...
                                                'Units','Points');
                                                                                     
    btnRecSerial = uicontrol(formElements.serialCtrl,'Style','pushbutton',...
                                                'String','Record Serial',...
                                                'HorizontalAlignment','Center',...
                                                'Position',[130 12 105 25],...
                                                'Units','Points');                                        
                                            
formElements.serialMon = uipanel( 'Title','Serial Monitor',...
                            'Units','Characters',...
                            'Position',[2 1 140 43]);

%% GPS Monitor Elements

formElements.gpsCtrl = uipanel(   'Title','GPS Controls',...
                            'Units','Characters',...
                            'Position',[144 45 80 9]);


formElements.gpsMon = uipanel(    'Title','GPS Monitor',...
                            'Units','Characters',...
                            'Position',[144 1 80 43]);


%% Final Preparations before opening form

% Get eachUIpanel offset to effectively anchor them to the top left corner

getElementOffset(formElements.serialCtrl,defaultSize);
getElementOffset(formElements.serialMon,defaultSize);
getElementOffset(formElements.gpsCtrl,defaultSize);
getElementOffset(formElements.gpsMon,defaultSize);

set(upTelemetry,'Visible','on');
%% Figure Management Functions
  
function getElementOffset(targetElement,formPosition)
    elementPosition = get(targetElement,'Position');
    
    % Define an offset as being from the top right corner
    offset(1) = formPosition(3) - elementPosition(1);
    offset(2) = formPosition(4) - elementPosition(2);
    set(targetElement,'UserData',offset);

end

function sizeChanged(parentFigure,~)
    global formElements
    
    %Get new figure size
    figPosition = get(parentFigure,'Position');
    
    % Anchor serial controls
    currentPosition = get(formElements.serialCtrl,'Position');
    offset = get(formElements.serialCtrl,'UserData');
    elementPosition(1) = currentPosition(1);
    elementPosition(2) = figPosition(4) - offset(2);     
    elementPosition(3) = currentPosition(3);
    elementPosition(4) = currentPosition(4);
    set(formElements.serialCtrl,'Position',elementPosition);

    % Set new offsets
    set(formElements.serialCtrl,'UserData',[figPosition(3) - elementPosition(1),figPosition(4) - elementPosition(2)]);

    % Scale Serial Monitor
    currentPosition = get(formElements.serialMon,'Position');
    offset = get(formElements.serialMon,'UserData');
    elementPosition(1) = currentPosition(1);
    elementPosition(2) = currentPosition(2);  
    elementPosition(3) = currentPosition(3) + figPosition(3) - (currentPosition(1) + offset(1));
    elementPosition(4) = currentPosition(4) + figPosition(4) - (currentPosition(2) + offset(2));
    set(formElements.serialMon,'Position',elementPosition);
    
    % Set new offsets
    set(formElements.serialMon,'UserData',[figPosition(3) - elementPosition(1),figPosition(4) - elementPosition(2)]);
    
    
    % Shift GPS Controls
    currentPosition = get(formElements.gpsCtrl,'Position');
    offset = get(formElements.gpsCtrl,'UserData');
    elementPosition(1) = figPosition(3) - (currentPosition(3)+2);  
    elementPosition(2) = figPosition(4) - offset(2);  
    elementPosition(3) = currentPosition(3);
    elementPosition(4) = currentPosition(4);
    set(formElements.gpsCtrl,'Position',elementPosition);
    
    % Set new offsets
    set(formElements.gpsCtrl,'UserData',[figPosition(3) - elementPosition(1),figPosition(4) - elementPosition(2)]);
    
    
    % Scale GPS Monitor
    currentPosition = get(formElements.gpsMon,'Position');
    offset = get(formElements.gpsMon,'UserData');
    elementPosition(1) = figPosition(3) - (currentPosition(3)+2);   
    elementPosition(2) = currentPosition(2);  
    elementPosition(3) = currentPosition(3);
    elementPosition(4) = currentPosition(4) + figPosition(4) - currentPosition(2) - offset(2);
    set(formElements.gpsMon,'Position',elementPosition);  
    
    % Set new offsets
    set(formElements.gpsMon,'UserData',[figPosition(3) - elementPosition(1),figPosition(4) - elementPosition(2)]);
    
    % Constrain minimum size
    set(parentFigure, 'Position', max([0 0 160 50], figPosition));

end

%% Serial Monitor Callbacks

%% GPS Monitor Callbacks

%% Assorted Functions

%% External Forms