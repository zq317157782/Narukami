function y=radical_inverse_base2(num)
    y=cast(reverse_bits_uint32(num),"single")*(2^-32);
end