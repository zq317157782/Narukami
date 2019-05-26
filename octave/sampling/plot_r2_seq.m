function plot_r2_seq(N)
    x = zeros(N,1);
    y = zeros(N,1);
    for idx=1:N
        [x(idx),y(idx)]=r2_seq(idx);
    end

    plot(x,y,".");
    grid on
    xlabel("x");
    ylabel("y");
    title(["r2_" num2str(N)]);
    print -dpng r2.png
end
