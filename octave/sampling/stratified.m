function [x,y]=stratified(idx,N,M)
    idx = idx - 1;
    x=(mod(idx,N)+rand())/N;
    y=(floor(idx/N)+rand())/M;
endfunction