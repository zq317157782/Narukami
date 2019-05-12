function [x,y]=hammersley_sampling(size)
    x = zeros(size,1);
    y = zeros(size,1);
    for idx=1:(size)
        x(idx)=idx/size;
        y(idx)=radical_inverse(2,idx);
    endfor
endfunction