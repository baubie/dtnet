function dtformat(mode)

switch(mode)
    case 'MSD'
        legend('250 Hz', '300 Hz', '350 Hz', '400 Hz');
        axis([0, 25, 0, 100]);
        axis 'auto y';
        
        h = findobj(gcf, 'Type', 'line');
        
        markers = ['^','v','d','o','s'];
        
        for i = 8:12
           set(h(i), 'Marker', markers(i-7)); 
           set(h(i), 'MarkerEdgeColor', 'k');
           set(h(i), 'MarkerFaceColor', 'k');
           set(h(i), 'MarkerSize', 6);
           set(h(i), 'Color', 'k');
        end
end
