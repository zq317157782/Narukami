function y=reverse_bits_uint32(x)
    x = uint32(x);
    x=bitor(bitshift(x,16,32),bitshift(x,-16,32)); # (x<<16)|(x>>16)
    x=bitor(bitshift(bitand(x,0x00ff00ff),8,32),bitshift(bitand(x,0xff00ff00),-8,32));#((x&0x00ff00ff)<<8)|((x&0xff00ff00)>>8)
    x=bitor(bitshift(bitand(x,0x0f0f0f0f),4,32),bitshift(bitand(x,0xf0f0f0f0),-4,32));#((x&0x0f0f0f0f)<<4)|((x&0xf0f0f0f0)>>4)
    x=bitor(bitshift(bitand(x,0x33333333),2,32),bitshift(bitand(x,0xcccccccc),-2,32));#((x&0x33333333)<<2)|((x&0xcccccccc)>>2)
    x=bitor(bitshift(bitand(x,0x55555555),1,32),bitshift(bitand(x,0xaaaaaaaa),-1,32));#((x&0x55555555)<<1)|((x&0xaaaaaaaa)>>1)
    y=x;
end