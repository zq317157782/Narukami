function plot_integrate_step_func()
    true_value = 0.5;
    x = zeros(100);
    halton_y = zeros(100);
    r2_y = zeros(100);
    stratified_y = zeros(100);
    hammersley_y = zeros(100);
    #设置样本数
    x(1)=2;
    for idx=2:100
        x(idx)=x(idx-1) + 1;
    end


    for idx=1:100
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=halton_seq(u);
            sample_sum = sample_sum+step_func(ux);
        end
        estimate_value=sample_sum/N;
        halton_y(idx)=abs(estimate_value-true_value);
    end

    for idx=1:100
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=r2_seq(u);
            sample_sum = sample_sum+step_func(ux);
        end
        estimate_value=sample_sum/N;
        r2_y(idx)=abs(estimate_value-true_value);
    end

    for idx=1:100
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=stratified(u,10,10);
            sample_sum = sample_sum+step_func(ux);
        end
        estimate_value=sample_sum/N;
        stratified_y(idx)=abs(estimate_value-true_value);
    end


    for idx=1:100
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=hammersley_set(u,100);
            sample_sum = sample_sum+step_func(ux);
        end
        estimate_value=sample_sum/N;
        hammersley_y(idx)=abs(estimate_value-true_value);
    end


    #plot(x,halton_y,"-",x,r2_y,"r.",x,stratified_y,"-",x,hammersley_y,"g.");
    plot(x,halton_y,"-")
    hold on
    plot(x,r2_y,"r.")

    plot(x,stratified_y,"-")

    plot(x,hammersley_y,"g.")
    hold off

    xlabel("sample number");
    ylabel("integration error");
    title(["step function"]);
    legend({'halton','r2','stratified','hammersley'})
    xlim([0 100])
    ylim([0.0 0.2])
    print -dpng step_function.png
end