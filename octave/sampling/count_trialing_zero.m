function x=count_trialing_zero(idx)
     count = 0;
     while (bitget(idx,1)==0) 
        count = count + 1;
        idx = bitshift(idx,-1);
     end
     x=count;
end