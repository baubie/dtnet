function axes_h=showdt(filename, type, varargin)

    buffer = 10;

    % Read in dtnet data
    if exist(filename,'file') == 0
       fprintf('Error: %s not found.\n', filename);
       return;
    end    
    fid = fopen(filename, 'r');

    % Determine the number of steps in each trace
    steps = fread(fid,1,'uint32');

    % Determine the number of populations
    pops = fread(fid,1,'uint32');

    % Read in time series
    t = fread(fid,steps,'float32');

    % Read in voltage traces
    V = {};
    spikes = {};
    pop_names = cell(1,pops);
    pop_sizes = zeros(1,pops);
    for p=1:pops
        popsize = fread(fid,1,'uint32');
        pop_sizes(p) = popsize;
        namesize = fread(fid,1,'uint32');
        pop_names{p} = fread(fid,namesize,'*char')';
        for i = 1:popsize
            V{p,i} = fread(fid,steps,'float32');
            numspikes = fread(fid,1,'uint32');
            spikes{p,i} = fread(fid,numspikes,'float32');
        end
    end

    filter = 1:pops;
    if (length(varargin) == 1) 
        filter = varargin{1};
    end
    axes_h = zeros(1, length(filter));
    fclose(fid);
    
    figure('Position', [1 100 700 800]);
    switch lower(type)
        case 'voltage'
            pcount = 0;
            for p=filter
                pcount = pcount + 1;
                axes_h(pcount) = subplot(length(filter),1,pcount);
                for n=1:pop_sizes(p)
                    plot(t,V{p,n},'-');
                    hold all;
                end
               % title(pop_names{p}); 
                axis([t(1),t(end),-100,-20]);
             
            end
           
        case 'stackedvoltage'
            for p=filter
                for n=1:pop_sizes(p)
                    plot(t,V{p,n});
                    hold all;
                end
            end
           % title(pop_names{p}); 
            axis([-10,t(end),-100,-20]);
            xlabel('Time From Stimulus Onset (ms)');
            ylabel('Voltage (mV)');

        case 'spikes'           
            pcount = 0;
            for p=filter
                pcount = pcount + 1;
                axes_h(pcount) = subplot(length(filter),1,pcount);
                for n=1:pop_sizes(p)
                    s = spikes{p,n};
                    plot(s-buffer, ones(length(s))*n-0.5, 'k.');
                    hold on;
                end
                %title(pop_names{p}); 
                axis([t(1),t(end),0,pop_sizes(p)]);
                set(gca,'YTick',[]);
            end
            xlabel('Time From Stimulus Onset (ms)');
    end
