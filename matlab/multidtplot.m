function multidtplot(name,num,duration,pop)

for i=1:num
    dtspikes([name,num2str(i),'-'], duration, pop, (i-1)/(2*num));
end
