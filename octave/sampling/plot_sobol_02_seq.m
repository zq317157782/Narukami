function plot_sobol_02_seq(N)
    x = zeros(N,1);
    y = zeros(N,1);
    C=load("sobol_matrix.txt");
    for idx=1:N
        [x(idx),y(idx)]=sobol_02_seq(idx,C);
    end

    plot(x,y,".");
    grid on
    xlabel("x");
    ylabel("y");
    title(["sobol_02_" num2str(N)]);
    print -dpng sobol_02.png
end
