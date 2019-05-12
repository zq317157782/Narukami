function x=radical_inverse(base,num)
   inv_num=0;
   inv_base=1;
    while num!=0
        digit=mod(num,base);
        num=floor(num/base);
        inv_base=inv_base/base;
        inv_num=inv_num+digit*inv_base;
    end
    x=inv_num; 
endfunction