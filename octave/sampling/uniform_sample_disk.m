function [x,y]=uniform_sample_disk(u,v)
    radius = sqrt(u);
    theta = 2.0 * pi * v;
    x = radius * cos(theta);
    y = radius * sin(theta);
end