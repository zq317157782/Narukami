function [x,y]=hypercube(idx,N,px,py)
    x=(px(idx)+rand())/N;
    y=(py(idx)+rand())/N;
end