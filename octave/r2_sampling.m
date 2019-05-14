function [x,y]=r2_sampling(size)
    g = 1.32471795724474602596;
    a1 = 1.0/g;
    a2 = 1.0/(g*g);
    x = zeros(size,1);
    y = zeros(size,1);
    for idx=1:(size)
        x(idx)=mod(0.5+a1*idx,1);
        y(idx)=mod(0.5+a2*idx,1);
    endfor
endfunction