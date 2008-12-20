function h=dthist(filename, trialrange, runrange, pop, smooth)


spikenum = cell(size(pop));

runcount = 0;
for runappend = runrange
    count = 0;
    runcount = runcount + 1;
    for append = trialrange
        count = count + 1;

        % Read in dtnet data
        newName = [filename,num2str(runappend),'-',num2str(append)];
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
        numspikes = zeros(1,pops);
        for p=1:pops
            popsize = fread(fid,1,'uint32');
            pop_sizes(p) = popsize;
            namesize = fread(fid,1,'uint32');
            pop_names{p} = fread(fid,namesize,'*char')';
            for i = 1:popsize
                V{p,i} = fread(fid,steps,'float32');
                tmp = fread(fid,1,'uint32');
                numspikes(p) = numspikes(p) + tmp;
                spikes{p,i} = fread(fid,tmp,'float32');
            end
        end

        for j = pop
            num = numspikes(j); 
            if (runcount > 1)
                spikenum{j}(count)  = (spikenum{j}(count)*(runcount-1) + num) / runcount;
            else
                spikenum{j}(count) = num;
            end
        end

        fclose(fid);
    end
end

count = 0;
for i=pop
    count = count + 1;

    x = spikenum{i};
    m = smooth;
    n = length(spikenum{i});

    z = [0 cumsum(x)];
    y = ( z(m+1:n+1) - z(1:n-m+1) ) / m;
    h=plot(trialrange(1:end-m+1),y);
end
axis([0 trialrange(end-m+1) 0 max(y)+1]);
