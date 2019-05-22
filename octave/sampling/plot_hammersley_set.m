function plot_hammersley_set(N)
    x = zeros(N,1);
    y = zeros(N,1);
    for idx=1:N
        [x(idx),y(idx)]=hammersley_set(idx,N);
    end

    plot(x,y,".");
    xlabel("x");
    ylabel("y");
    title(["hammersley_" num2str(N)]);
    print -dpng hammersley.png
end
