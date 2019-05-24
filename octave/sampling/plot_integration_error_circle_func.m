function plot_integration_error_circle_func(t_size,t_step)
    
    true_value = pi*0.25;
    a_size = t_size/t_step;

    x = 1:t_step:t_size;
    halton_y = zeros(a_size,1);
    r2_y = zeros(a_size,1);
    stratified_y = zeros(a_size,1);
    hammersley_y = zeros(a_size,1);
    hypercube_y = zeros(a_size,1);
    random_y = zeros(a_size,1);

    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=halton_seq(u);
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        halton_y(idx)=abs(estimate_value-true_value);
    end


    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=r2_seq(u);
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        r2_y(idx)=abs(estimate_value-true_value);
    end

    % for idx=1:a_size
    %     N=x(idx);
    %     sample_sum=0;
    %     for u=1:N
    %         [ux,uy]=stratified(u,sqrt(N),sqrt(N));
    %         sample_sum = sample_sum+circle_func(ux,uy);
    %     end
    %     estimate_value=sample_sum/N;
    %     stratified_y(idx)=abs(estimate_value-true_value);
    % end


    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=hammersley_set(u,N);
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        hammersley_y(idx)=abs(estimate_value-true_value);
    end

    

    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        px=permutate1D(N);
        py=permutate1D(N);
        for u=1:N
            [ux,uy]=hypercube(u,N,px,py);
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        hypercube_y(idx)=abs(estimate_value-true_value);
    end


    

    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            ux=rand();
            uy=rand();
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        random_y(idx)=abs(estimate_value-true_value);
    end


    loglog(x,random_y,"ro",x,halton_y,"--",x,r2_y,":",x,hammersley_y,".",x,hypercube_y,"-");
    legend('random','halton','r2','hammersley','hypercube')

    xlabel("sample number");
    ylabel("integration error");
    title(["circle function"]);

    xlim([0 x(a_size)])
    ylim([0.0 0.5])
    print -dpng circle_function.png
end