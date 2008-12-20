function dtspikes(filename, filerange, pop, offset)

count = 0;
sigSpikes = cell(length(pop),length(filerange));
for append = filerange
    count = count + 1;

    % Read in dtnet data
    newName = [filename,num2str(append)];    
    if exist(newName) == 0
       fprintf('Error: %s not found.\n', newName);
       return;
    end
    fid = fopen(newName, 'r');

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
    for p=pop
        sigSpikes{p, count} = spikes{pop,1};
    end
    fclose(fid);
end

plotcount = 0;
for p=pop
    plotcount = plotcount + 1;
    count = 0;
    for sigLen = filerange
        count = count + 1;
        spikes = sigSpikes{p,count}-10;  % Sound starts at 10 so that's why we put this here.
        plot(spikes, ones(1,length(spikes))*sigLen+offset, 'MarkerSize', 5, 'LineStyle', 'none', 'Marker', '.', 'Color', 'k');
        hold on;
        signal = [0,sigLen];
        plot(signal, ones(1,2)*sigLen, 'Color', 'k', 'LineWidth', 0.3, 'Marker', 'none');
        hold on;
    end
    axis([-0.5 max(t) filerange(1)-1 filerange(end)+1]);
end
