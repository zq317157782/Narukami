function [x,y]=scramble_sobol_02_seq(idx,C,scramble)
    x=multiply_generator_matrix(idx,C(1,:));
    y=multiply_generator_matrix(idx,C(2,:));

    x=cast(bitxor(reverse_bits_uint32(uint32(x)),scramble),"single")*(2^-32);
    y=cast(bitxor(reverse_bits_uint32(uint32(y)),scramble),"single")*(2^-32);
end