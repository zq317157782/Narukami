function [x,y]=r2_seq(idx)
    idx = idx - 1;
    g = 1.32471795724474602596;
    a1 = 1.0/g;
    a2 = 1.0/(g*g);
  
    x=mod(0.5+a1*idx,1);
    y=mod(0.5+a2*idx,1);
endfunction