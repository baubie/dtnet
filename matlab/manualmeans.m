
data = [0,0,0.87,1.53,1.87,1.87,2,1.33,1.13,0.73,0.53,0.33,0.33,0.4,0.33,0.27,0,0.2,0.33,0.07,0.07,0.07,0.07,0,0.07;
0,0.13,0.67,1.87,2.73,2.8,3.6,3.87,3.6,3.73,3.47,3.13,3.07,2.53,2.13,1.67,1.73,1.4,0.93,1.2,1,0.67,0.8,0.53,0.53];


    h=plot(1:25,data(1,1:end));
axis([0 25 0 4.5]);    
whitebg('w');
duration=25;
    
hold all;
        set(gca,'TickDir', 'out');
        set(gca,'XMinorTick','on');
        set(gca,'YMinorTick','off');
        set(gca,'FontSize',6);

          fillCol = [0 0 0];
           set(h,'MarkerFaceColor', [0 0 0]);
        set(h,'MarkerSize', 4);
    h=plot(1:25,data(2,1:end));        
           set(h,'MarkerFaceColor', [1 1 1]);
        set(h,'MarkerSize', 4);
         width = 5.4;
    height = 4.2;
    xmargin = 0.6;
    ymargin = 0.9;    
    %ylim('auto');
    YtickLabels = get(gca, 'YTickLabel');
    set(gca, 'YTickLabelMode', 'manual');
    set(gca, 'YTickMode', 'manual');
    set(gca, 'xlim', [0 duration+1]);
    set(gca, 'YTickLabel', get(gca, 'YTick'));
    set(gca, 'Box', 'off');
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

    h=legend('+10 dB', '+20 dB', 'Location', 'NorthEast');        
    set(h,'units','centimeters');        
    set(h,'Position', [3.2 2.5 2 0.8]);
        
    legend('boxoff');

    xlabel('Stimulus Duration (ms)', 'FontSize', 7);
    
    print -r600 -painters -noui -deps2 -f1 manmeans.eps