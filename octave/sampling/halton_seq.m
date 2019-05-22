function [x,y]=halton_seq(idx)
    x=radical_inverse(2,idx);
    y=radical_inverse(3,idx);
end