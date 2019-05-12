function [x,y]=stratified_sampling(N)
    x = zeros(N*N,1);
    y = zeros(N*N,1);
    for idx=0:(N*N-1)
        x(idx+1)=(mod(idx,N)+rand())/N;
        y(idx+1)=(floor(idx/N)+rand())/N;
    endfor
endfunction