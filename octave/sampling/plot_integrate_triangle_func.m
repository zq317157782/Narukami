function plot_integrate_triangle_func()
    true_value = 0.5;
    a_size = 50;
    x = zeros(a_size);
    halton_y = zeros(a_size);
    r2_y = zeros(a_size);
    stratified_y = zeros(a_size);
    hammersley_y = zeros(a_size);
    random_y = zeros(a_size);
    #设置样本数
    x(1)=2;
    for idx=2:a_size
        x(idx)=idx*idx;
    end

    

    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            ux=rand();
            uy=rand();
            sample_sum = sample_sum+triangle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        random_y(idx)=abs(estimate_value-true_value);
    end


    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=halton_seq(u);
            sample_sum = sample_sum+triangle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        halton_y(idx)=abs(estimate_value-true_value);
    end

    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=r2_seq(u);
            sample_sum = sample_sum+triangle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        r2_y(idx)=abs(estimate_value-true_value);
    end

    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=stratified(u,sqrt(N),sqrt(N));
            sample_sum = sample_sum+triangle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        stratified_y(idx)=abs(estimate_value-true_value);
    end


    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=hammersley_set(u,N);
            sample_sum = sample_sum+triangle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        hammersley_y(idx)=abs(estimate_value-true_value);
    end



    #plot(x,halton_y,"-",x,r2_y,"r.",x,stratified_y,"-",x,hammersley_y,"g.");
    
    plot(x,random_y,"ro")

    hold on
    plot(x,halton_y,"b-")

    plot(x,r2_y,"r-")

    plot(x,stratified_y,"go")

    plot(x,hammersley_y,"g-")
    hold off

    xlabel("sample number");
    ylabel("integration error");
    title(["triangle function"]);
    legend({'random','halton','r2','stratified','hammersley'})
    xlim([0 x(a_size)])
    ylim([0.0 0.05])
    print -dpng triangle_function.png
end