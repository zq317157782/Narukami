function plot_integrate_circle_func()
    true_value = pi*0.25;
    x = zeros(400);
    halton_y = zeros(400);
    r2_y = zeros(400);
    stratified_y = zeros(400);
    hammersley_y = zeros(400);
    #设置样本数
    x(1)=2;
    for idx=2:400
        x(idx)=x(idx-1) + 1;
    end


    for idx=1:400
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=halton_seq(u);
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        halton_y(idx)=abs(estimate_value-true_value);
    end

    for idx=1:400
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=r2_seq(u);
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        r2_y(idx)=abs(estimate_value-true_value);
    end

    for idx=1:400
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=stratified(u,20,20);
            sample_sum = sample_sum+circle_func(ux,uy);
        end
        estimate_value=sample_sum/N;
        stratified_y(idx)=abs(estimate_value-true_value);
    end


    for idx=1:400
        N=x(idx);
        sample_sum=0;
        for u=1:N
            [ux,uy]=hammersley_set(u,400);
            sample_sum = sample_sum+circle_func(ux,uy);
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
    title(["circle function"]);
    legend({'halton','r2','stratified','hammersley'})
    xlim([0 400])
    ylim([0.0 0.2])
    print -dpng circle_function.png
end