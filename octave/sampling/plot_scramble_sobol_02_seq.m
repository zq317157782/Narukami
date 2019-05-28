function plot_scramble_sobol_02_seq(N)
    x = zeros(N,1);
    y = zeros(N,1);
    scramble = randi(2^32);
    load sobol_02_matrix;
    for idx=1:N
        [x(idx),y(idx)]=scramble_sobol_02_seq(idx,sobol_02_matrix,scramble);
    end

    plot(x,y,".");
    grid on
    xlabel("x");
    ylabel("y");
    title(["scramble_sobol_02_" num2str(N)]);
    print -dpng scramble_sobol_02.png
end
