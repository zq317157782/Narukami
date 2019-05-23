function plot_hypercube(N)
    x = zeros(N,1);
    y = zeros(N,1);
    px=permutate1D(N);
    py=permutate1D(N);
    for idx=1:N
        [x(idx),y(idx)]=hypercube(idx,N,px,py);
    end

    plot(x,y,".");
    xlabel("x");
    ylabel("y");
    title(["hypercube_" num2str(N)]);
    xlim([0 1])
    ylim([0 1])
    print -dpng hypercube.png
end
