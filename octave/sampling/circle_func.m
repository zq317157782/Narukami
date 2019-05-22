function z=circle_func(x,y)
    x = x - 0.5;
    y = y - 0.5;
    if (sqrt(x*x+y*y)<=0.5)
        z=1;
    else
        z=0;
    end
end