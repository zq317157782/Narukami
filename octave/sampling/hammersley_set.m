function [x,y]=hammersley_set(idx,N)
    x=idx/N;
    y=radical_inverse_base2(idx);
endfunction