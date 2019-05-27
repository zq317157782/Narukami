function v=multiply_generator_matrix(x,C)
    #C col major matrix
    v=0;
    for idx=1:32 
        if (bitget(x,idx) == 1)
            v=bitxor(uint32(v),C(idx));
        end
    end
end