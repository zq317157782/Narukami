function plot_halton_seq(N)
    x = zeros(N,1);
    y = zeros(N,1);
    for idx=1:N
        [x(idx),y(idx)]=halton_seq(idx);
    end
    
    plot(x,y,".");
    grid on
    xlabel("x");
    ylabel("y");
    title(["halton_" num2str(N)]);
    xlim([0 1])
    ylim([0 1])
    print -dpng halton.png
end
