function plot_integration_error_gaussion_func(t_size,t_step)
    true_value =0.78539816339;
    a_size = t_size/t_step;

    x = 1:t_step:t_size;
    data = zeros(7,a_size);
    ux=zeros(7,1);
    uy=zeros(7,1);

    load sobol_02_matrix;


    for idx=1:a_size
        N=x(idx);
        sample_sum=zeros(7,1);
        px=permutate1D(N);
        py=permutate1D(N);
        scramble = randi(2^32);
        for u=1:N
            [ux(1),uy(1)]=halton_seq(u);
            [ux(2),uy(2)]=r2_seq(u);
            [ux(3),uy(3)]=hammersley_set(u,N);
            [ux(4),uy(4)]=hypercube(u,N,px,py);
            ux(5)=rand();
            uy(5)=rand();
            [ux(6),uy(6)]=sobol_02_seq(u,sobol_02_matrix);
            [ux(7),uy(7)]=scramble_sobol_02_seq(u,sobol_02_matrix,scramble);
            sample_sum = sample_sum.+gaussion_func(ux,uy);
        end
        estimate_value=sample_sum./N;
        error = estimate_value.-true_value;
        data(:,idx)=error.*error;
    end

    semilogy(x,data(1,:),"-",x,data(2,:),"-",x,data(3,:),".",x,data(4,:),".",x,data(5,:),"ro",x,data(6,:),'d',x,data(7,:),'d');
    legend('halton','r2','hammersley','hypercube','random','0-2','scramble 0-2');
    xlabel("sample number");
    ylabel("squared error");
    title(["gaussion function"]);

    %  %xlim([0 x(a_size)])
    %  %ylim([0.0 0.5])
    print -dpng gaussion_function.png
end