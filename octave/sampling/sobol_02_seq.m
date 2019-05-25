function [x,y]=sobol_02_seq(idx)
    C=load("sobol_matrix.txt");
    x=multiply_generator_matrix(idx,C(1,:));
    y=multiply_generator_matrix(idx,C(2,:));

    x=cast(reverse_bits_uint32(uint32(x)),"single")*(2^-32);
    y=cast(reverse_bits_uint32(uint32(y)),"single")*(2^-32);
end