function plot_stratified(N,M)
    x = zeros(N*M,1);
    y = zeros(N*M,1);
    for idx=1:N*M
        [x(idx),y(idx)]=stratified(idx,N,M);
    end

    plot(x,y,".");
    xlabel("x");
    ylabel("y");
    title(["stratified_" num2str(N) "x" num2str(M)]);
    print -dpng stratified.png
end
