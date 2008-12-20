function varargout = dtplot(varargin)
%DTPLOT M-file for dtplot.fig
%      DTPLOT, by itself, creates a new DTPLOT or raises the existing
%      singleton*.
%
%      H = DTPLOT returns the handle to a new DTPLOT or the handle to
%      the existing singleton*.
%
%      DTPLOT('Property','Value',...) creates a new DTPLOT using the
%      given property value pairs. Unrecognized properties are passed via
%      varargin to dtplot_OpeningFcn.  This calling syntax produces a
%      warning when there is an existing singleton*.
%
%      DTPLOT('CALLBACK') and DTPLOT('CALLBACK',hObject,...) call the
%      local function named CALLBACK in DTPLOT.M with the given input
%      arguments.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help dtplot

% Last Modified by GUIDE v2.5 10-Sep-2008 11:45:05

% Begin initialization code - DO NOT EDIT
gui_Singleton = 0;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @dtplot_OpeningFcn, ...
                   'gui_OutputFcn',  @dtplot_OutputFcn, ...
                   'gui_LayoutFcn',  [], ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
   gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end


% End initialization code - DO NOT EDIT


function dtplot_OpeningFcn(hObject, eventdata, handles, varargin)

% Choose default command line output for dtplot
handles.output = hObject;

% Setup the handler for plot selection changes
set(handles.panelSelection,'SelectionChangeFcn',@panelSelection_SelectionChangeFcn);

% Update handles structure
guidata(hObject, handles);


function panelSelection_SelectionChangeFcn(hObject, eventdata)
 
%retrieve GUI data, i.e. the handles structure
handles = guidata(hObject); 

switch get(eventdata.NewValue,'Tag')   % Get Tag of selected object
    case 'rbMSD'
      set(handles.duration,'Enable','on');

    case 'rbSR'
      set(handles.duration,'Enable','on');

    case 'rbSTV'
      set(handles.duration,'Enable','on'); 

    case 'rbSTS'
      set(handles.duration,'Enable','on'); 
end
%updates the handles structure
guidata(hObject, handles);


function varargout = dtplot_OutputFcn(hObject, eventdata, handles)

% Get default command line output from handles structure
varargout{1} = handles.output;


function filename_CreateFcn(hObject, eventdata, handles)

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

    populateFilenames(hObject);



function populateFilenames(ui)
filenames = '';
files = dir('.');
[sorted_names,sorted_index] = sortrows({files.name}');
uniqueNames = {}; %cell(length(sorted_names),1);
for i = 1:length(sorted_names)
    f = sorted_names(i);
    f = f{1};
    name = regexpi(f,'[a-z]+','match');
    if length(name) == 1
        name = name{1};
        if length(name) > 2
            uniqueNames{length(uniqueNames)+1} = name;
        end
    end
end

uniqueNames = unique(uniqueNames);

for i = 1:length(uniqueNames)
    filenames = strcat(filenames,uniqueNames{i},'|');
end

% Remove last |
filenames = filenames(1:end-1);

% Populate the Trial Set (filename) dropdown menu
if (strcmp(filenames,'')==1)
    filenames = 'No Files Found';
    set(ui,'Enable','inactive');   
end
set(ui,'String',filenames);

function trials_Callback(hObject, eventdata, handles)


function filename_Callback(hObject, eventdata, handles)



function trials_CreateFcn(hObject, eventdata, handles)

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function duration_Callback(hObject, eventdata, handles)


function duration_CreateFcn(hObject, eventdata, handles)

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end

function pbGenerate_Callback(hObject, eventdata, handles)

% Determine where we are and do what's right.
filesSelected = get(handles.filename,'Value');

whitebg('w');
if get(handles.rbMSD,'Value')
    set(0,'DefaultAxesColorOrder',[0 0 0]);
    if get(handles.rbBicuculline, 'Value')
        set(0,'DefaultAxesLineStyleOrder',{'o-','s-'});        
    else
        set(0,'DefaultAxesLineStyleOrder',{'o-','s-','^-','d-'});        
    end
elseif get(handles.rbSTV,'Value')
    colours = 0:0.1:0.8;
    colours = [colours;colours;colours]';
    set(0,'DefaultAxesColorOrder',colours);
end

haveFigure = 0;
count = 0;
for fi = filesSelected
    count = count + 1;
    filename = get(handles.filename,'String');
    filename = strtrim(filename(fi,:));
    filename = strcat(filename,'/',filename);
    ti = get(handles.trials,'Value');
    trials = get(handles.trials,'String');
    trials = str2double(trials{ti});
    di = get(handles.duration,'Value');
    duration = get(handles.duration,'String');
    duration = str2double(duration{di});
    population = get(handles.population,'String');
    population = str2num(population); %#ok<ST2NM>

    if get(handles.rbMSD,'Value')
        if (haveFigure == 0)
            figure;
            haveFigure = 1;
        end
        hold all;
        set(gca,'TickDir', 'out');
        set(gca,'XMinorTick','on');
        set(gca,'YMinorTick','off');
        set(gca,'FontSize',6);
        h=dthist(filename, 1:duration, 1:trials, population(end), 1);
        if(mod(count,2) == 1)
            fillCol = [0 0 0];
        else
            fillCol = [1 1 1];
        end
        
        set(h,'MarkerFaceColor', fillCol);
        set(h,'MarkerSize', 4);
    elseif get(handles.rbSR,'Value')
        if (haveFigure == 0)
            figure;
            haveFigure = 1;
        end
        hold all;
        set(gca,'TickDir', 'out');
        set(gca,'XMinorTick','on');
        set(gca,'YMinorTick','off');
        set(gca, 'YTick', 1:1:duration+1);
        set(gca, 'YTickLabelMode', 'manual');        
        YtickLabels = [];
        for i=1:duration
            YtickLabels = strcat(YtickLabels,num2str(i),'|');
        end
        YtickLabels = strcat(YtickLabels,' ');
        set(gca, 'YTickLabel', YtickLabels);        
        multidtplot(filename,trials,1:duration,population(end));   
        set(gca,'FontSize',7);        
    elseif get(handles.rbSTV,'Value')
        axes_h = showdt(strcat(filename,num2str(trials),'-',num2str(duration)),'voltage',population);
    elseif get(handles.rbSTS,'Value')
        axes_h = showdt(strcat(filename,num2str(trials),'-',num2str(duration)),'spikes',population);
    elseif get(handles.rbPST,'Value')
        dtpst(filename,population,trials,duration);
    end
end

if (strcmp(filename(1:2),'LP'))
    titles = {'Standard Input', 'Steeper Rate Level Function Input', 'Adapting Sustained Inhibition', 'Tonic Excitation', 'Duration Tuned Neuron'};
elseif (strcmp(filename(1:2),'SP'))
    titles = {'Background Noise', 'Input', 'Sustained Inhibition', 'Onset', 'Delayed Onset', 'Duration Tuned Neuron'};
else
    titles = {'Input', 'Sustained Inhibition', 'Onset', 'Offset', 'Delayed Onset', 'Duration Tuned Neuron'};
end

if get(handles.rbMSD,'Value')
    width = 5.4;
    height = 4.2;
    xmargin = 0.6;
    ymargin = 0.9;    
    ylim('auto');
    YtickLabels = get(gca, 'YTickLabel');
    set(gca, 'YTickLabelMode', 'manual');
    set(gca, 'YTickMode', 'manual');
    set(gca, 'xlim', [0 duration+1]);
    set(gca, 'YTickLabel', get(gca, 'YTick'));
    YtickLabels = get(gca, 'YTickLabel');
    for i=1:length(YtickLabels)
       lab = str2double(YtickLabels(i,1:end));
       if (ceil(lab)-lab ~= 0)
          YtickLabels(i,1:end) = ' ';
       end
    end        
    set(gcf, 'Units', 'centimeters');
    set(gcf, 'PaperUnits', 'centimeters');
    set(gcf, 'PaperSize', [width height]);
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf, 'PaperPosition', [0 0 width height]);
    set(gcf, 'Position', [0 0 width height]);
    set(gcf, 'color', 'white');
    set(gca, 'Units', 'centimeters');    
    set(gca, 'YTickLabel', YtickLabels);
    set(gca, 'Position', [xmargin ymargin width-xmargin-0.1 height-ymargin]);    
    set(gca, 'TickDir', 'out');        
    set(gca, 'TickLength', [0.03, 0.02]);    
    if get(handles.rbRightLegend,'Value')
        %legend('350 Hz', '400 Hz', '450 Hz', '500 Hz', 'Location', 'NorthEast');        
        legend('350 Hz', '500 Hz', 'Location', 'NorthEast');        
    elseif get(handles.rbLeftLegend, 'Value')
        h=legend('E:350 Hz/I:150 Hz', 'E:400 Hz/I:250 Hz', 'E:450 Hz/I:375 Hz', 'E:500 Hz/I:525 Hz','Location', 'NorthWest');
        set(h,'units','centimeters');        
        set(h,'Position', [0.6 2.8 2 1.5]);
    elseif get(handles.rbBicuculline, 'Value')
        h=legend('Inhibition Present', 'Inhibition Removed');
        set(h,'units','centimeters');        
        set(h,'Position', [2.2 1.5 2 1.5]);
    end
    legend('boxoff');

    xlabel('Stimulus Duration (ms)', 'FontSize', 7);
    % ylabel('Spikes per Stimulus');
    
    print -r600 -painters -noui -deps2 -f1 means.eps
elseif get(handles.rbSR,'Value')  
    set(gca,'ylim', [0 duration+1]);
    set(gca,'xlim', [0 40]);
    width = 5.4;
    height = 4.7;
    xmargin = 0.5;
    ymargin = 0.9;
    set(gcf, 'Units', 'centimeters');
    set(gcf, 'PaperUnits', 'centimeters');
    set(gcf, 'PaperSize', [width height]);
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf, 'PaperPosition', [0 0 width height]);
    set(gcf, 'Position', [0 0 width height]);
    set(gcf, 'color', 'white');   
    set(gca, 'Units', 'centimeters');
    set(gca, 'Position', [xmargin ymargin width-xmargin-0.1 height-ymargin]);
    set(gca, 'TickDir', 'out');        
    set(gca, 'TickLength', [0.03, 0.01]);    
    set(gca, 'FontSize', 6);

%    ylabel('Input Signal Duration');
    xlabel('Time (ms)', 'FontSize', 7);
    
    ytl = cell(1,duration);
    for i=1:duration
        ytl{i} = i;
    end
    my_yticklabels([1:1:duration], ytl, 'FontSize', 4);
    
    print -r600 -painters -noui -deps2 -f1 raster.eps
elseif get(handles.rbSTV,'Value')
    width = 8;
    row_height = 1.2;
    height = row_height*length(population)+length(population)+0.5;
    xmargin = 1.2;
    ymargin = 0.8;
    set(gcf, 'Units', 'centimeters');
    set(gcf, 'PaperUnits', 'centimeters');
    set(gcf, 'PaperSize', [width height]);
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf, 'PaperPosition', [0 0 width height]);
    set(gcf, 'Position', [0 0 width height]);
    set(gcf, 'color', 'white');   

    set (axes_h, 'Units', 'centimeters');
    c = 0;
    y = row_height+0.5;
    for h = axes_h
        c = c + 1;
        set(h, 'Position', [xmargin height-y width-xmargin row_height]);
        set(h,'FontSize',7); 
        set(h, 'Box', 'off');
        set(h, 'XMinorTick', 'on');
        set(h, 'TickDir', 'out');        
        set(h, 'TickLength', [0.02, 0.02]);
        title(h, titles{population(c)}, 'HorizontalAlignment', 'left', 'FontSize', 10, 'Units', 'centimeters', 'Position', [0.2 row_height]);
        y = y + row_height+1;
        ylabel(h, 'Voltage (mV)');        
    end
    xlabel('Time From Stimulus Onset (ms)');    
    print -r600 -painters -noui -deps2 -f1 voltage.eps
elseif get(handles.rbSTS,'Value')
    width = 7.3;
    row_height = 1.2;
    height = row_height*length(population)+length(population)+0.5;
    xmargin = 0.5;
    ymargin = 0.8;
    set(gcf, 'Units', 'centimeters');
    set(gcf, 'PaperUnits', 'centimeters');
    set(gcf, 'PaperSize', [width height]);
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf, 'PaperPosition', [0 0 width height]);
    set(gcf, 'Position', [0 0 width height]);
    set(gcf, 'color', 'white');   

    set (axes_h, 'Units', 'centimeters');
    c = 0;
    y = row_height+0.5;
    for h = axes_h
        c = c + 1;
        set(h, 'Position', [xmargin height-y width-xmargin row_height]);
        set(h,'FontSize',7); 
        set(h, 'Box', 'off');
        set(h, 'XMinorTick', 'on');
        set(h, 'TickDir', 'out');        
        set(h, 'TickLength', [0.02, 0.02]);        
        title(h, titles{population(c)}, 'HorizontalAlignment', 'left', 'FontSize', 10, 'Units', 'centimeters', 'Position', [0.2 row_height]);
        y = y + row_height+1;       
    end
    xlabel('Time From Stimulus Onset (ms)');    
    print -r600 -painters -noui -deps2 -f1 voltage.eps    
elseif get(handles.rbPST,'Value')    
    width = 5.4;
    height = 4.2;
    xmargin = 1.6;
    ymargin = 1;    
    set(gca, 'xlim', [-10 30]);
    set(gcf, 'Units', 'centimeters');
    set(gcf, 'PaperUnits', 'centimeters');
    set(gcf, 'PaperSize', [width height]);
    set(gcf, 'PaperPositionMode', 'manual');
    set(gcf, 'PaperPosition', [0 0 width height]);
    set(gcf, 'Position', [0 0 width height]);
    set(gcf, 'color', 'white');
    set(gca, 'Units', 'centimeters');    
    set(gca, 'Position', [xmargin ymargin width-xmargin-0.1 height-ymargin]);    
    set(gca, 'TickDir', 'out');        
    set(gca, 'YMinorTick', 'on');
    set(gca, 'FontSize', 7);
    set(gca, 'XMinorTick', 'on');
    set(gca, 'TickLength', [0.03, 0.01]);    
    set(gca, 'Box', 'off');
    xlabel('Time (ms)', 'FontSize', 7);
    ylabel('Number of Spikes');
    
    print -r600 -painters -noui -deps2 -f1 pst.eps
end

hold off;

function pbRefresh_Callback(hObject, eventdata, handles)
    populateFilenames(handles.filename);


function tbHold_Callback(hObject, eventdata, handles)


function rbPST_Callback(hObject, eventdata, handles)


function population_Callback(hObject, eventdata, handles)


function population_CreateFcn(hObject, eventdata, handles)

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


function rbRightLegend_Callback(hObject, eventdata, handles)


function rbLeftLegend_Callback(hObject, eventdata, handles)


