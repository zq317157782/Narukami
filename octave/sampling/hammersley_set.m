function [x,y]=hammersley_set(idx,N)
    x=idx/N;
    y=radical_inverse(2,idx);
endfunction