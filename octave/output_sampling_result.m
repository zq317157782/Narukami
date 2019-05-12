[x,y]=stratified_sampling(50);
plot(x,y,".");
xlabel("x");
ylabel("y");
title("stratified 50x50");
print -dpdf stratified_sampling.pdf

[x,y]=halton_sampling(2500);
plot(x,y,".");
xlabel("x");
ylabel("y");
title("halton 2500");
print -dpdf halton_sampling.pdf


