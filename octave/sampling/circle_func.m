function z=circle_func(x,y)
    x = x .- 0.5;
    y = y .- 0.5;
    z=sqrt(x.*x.+y.*y)<=0.5;
end