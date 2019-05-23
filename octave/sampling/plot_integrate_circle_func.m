function plot_integrate_circle_func()
    true_value = pi*0.25;
    a_size = 50;
    x = zeros(a_size);
    halton_y = zeros(a_size);
    r2_y = zeros(a_size);
    stratified_y = zeros(a_size);
    hammersley_y = zeros(a_size);
    hypercube_y = zeros(a_size);
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

    for idx=1:a_size
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=stratified(u,sqrt(N),sqrt(N));
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        stratified_y(idx)=abs(estimate_value-true_value);
    end


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


    #plot(x,halton_y,"-",x,r2_y,"r.",x,stratified_y,"-",x,hammersley_y,"g.");
    plot(x,halton_y,"b-")
    hold on
    plot(x,r2_y,"r-")

    plot(x,stratified_y,"go")

    plot(x,hammersley_y,"g-")

    plot(x,hypercube_y,"-")

    plot(x,random_y,"ro")
    hold off

    xlabel("sample number");
    ylabel("integration error");
    title(["circle function"]);
    legend({'halton','r2','stratified','hammersley','hypercube','random'})
    xlim([0 x(a_size)])
    ylim([0.0 0.3])
    print -dpng circle_function.png
end