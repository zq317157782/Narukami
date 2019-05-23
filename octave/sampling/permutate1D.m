function x=permutate1D(N)
    for idx=1:N
        x(idx)=idx;
    end
    for idx=1:N
       idx2 = randi(N);
       temp=x(idx2);
       x(idx2)=x(idx);
       x(idx)=temp;
    end
end