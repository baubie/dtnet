function dtpst(filename, pop, trials, duration)

    buffer = 10;
    basefilename = filename;
    cspikes = [];    
    for trial = 1:trials      
        
        filename = strcat(basefilename,num2str(trial),'-',num2str(duration));
        % Read in dtnet data
        if exist(filename) == 0
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

        fclose(fid);
        
        popsize = pop_sizes(pop);
        % 1 ms bin widths 
        for i = 1:popsize
          nspikes = spikes{pop,i};
          for s = 1:length(nspikes)
              cspikes(end+1) = nspikes(s)-buffer;
          end
        end        
    end
    

    figure;
    x = -buffer:1:60;
    n = histc(cspikes,x);
    bar(x+0.5,n,'k'); 
    ylabel('# Of Spikes');
    xlabel('Time (ms)');
