function v=multiply_generator_matrix(x,C)
    #C col major matrix
    v=0;
    for idx=1:32 
        if (bitget(x,idx) == 1)
            v=bitxor(v,uint32(reverse_bits_uint32(C(idx))));
        end
    end
end