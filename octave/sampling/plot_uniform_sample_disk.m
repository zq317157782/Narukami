function plot_uniform_sample_disk(N,M)
    x = zeros(N*M,1);
    y = zeros(N*M,1);
    for idx=1:N*M
        [u,v] = stratified(idx,N,M);
        [x(idx),y(idx)]=uniform_sample_disk(u,v);
    end
    
    plot(x,y,".");
    xlabel("x");
    ylabel("y");
    title(["uniform_sample_disk_" num2str(N) "x" num2str(M)]);
    hold on
    th = 0:pi/50:2*pi;
    xunit = cos(th);
    yunit = sin(th);
    h = plot(xunit, yunit);
    hold off
    print -dpng uniform_sample_disk.png
end