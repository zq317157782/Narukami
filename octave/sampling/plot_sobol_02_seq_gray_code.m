function plot_sobol_02_seq_gray_code(N)
    x = zeros(N,1);
    y = zeros(N,1);
    xb =0;
    yb =0;
    load sobol_02_matrix;
    for idx=1:N
        [x(idx),y(idx),xb,yb]=sobol_02_seq_gray_code(idx,sobol_02_matrix,xb,yb);
    end

    plot(x,y,".");
    grid on
    xlabel("x");
    ylabel("y");
    title(["sobol_02_gray_code_" num2str(N)]);
    print -dpng sobol_02_gray_code.png
end
