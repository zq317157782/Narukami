function [x,y,xb,yb]=sobol_02_seq_gray_code(idx,C,xbits,ybits)

    col=count_trialing_zero(idx)+1;
    xb=bitxor(xbits,C(1,col));
    yb=bitxor(ybits,C(2,col));

    x=cast(reverse_bits_uint32(xb),"single")*(2^-32);
    y=cast(reverse_bits_uint32(yb),"single")*(2^-32);
end