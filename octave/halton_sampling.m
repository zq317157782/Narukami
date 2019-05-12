function [x,y]=halton_sampling(size)
    x = zeros(size,1);
    y = zeros(size,1);
    for idx=1:(size)
        x(idx)=radical_inverse(2,idx);
        y(idx)=radical_inverse(3,idx);
    endfor
endfunction