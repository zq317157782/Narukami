#include "gtest/gtest.h"
#include "core/math.h"
using namespace narukami;
TEST(math,rcp){
    EXPECT_FLOAT_EQ(rcp(2.0f),0.5f);
    EXPECT_FLOAT_EQ(rcp(3.0f),1.0f/3.0f);
    EXPECT_FLOAT_EQ(rcp(5.0f),1.0f/5.0f);
    EXPECT_FLOAT_EQ(rcp(7.0f),1.0f/7.0f);
}

TEST(math,isnan){
    float x=0.0f/0.0f;
    EXPECT_TRUE(isnan(x));

    x=1.0f/0.0f;
    EXPECT_FALSE(isnan(x));
}

TEST(math,isinf){
    float x=0.0f/0.0f;
    EXPECT_FALSE(isinf(x));

    x=1.0f/0.0f;
    EXPECT_TRUE(isinf(x));
}

TEST(math,cast_i2f){
    int x=1<<23;
    float y=cast_i2f(x);
    EXPECT_EQ(cast_f2i(y),x);
}

TEST(math,min){
    float x=10;
    float y=20;
    float z=30;
    float w=5;

    EXPECT_FLOAT_EQ(min(x,y),10);
    EXPECT_FLOAT_EQ(min(x,y,z),10);
    EXPECT_FLOAT_EQ(min(x,y,z,w),5);
}

TEST(math,max){
    float x=10;
    float y=20;
    float z=30;
    float w=5;

    EXPECT_FLOAT_EQ(max(x,y),20);
    EXPECT_FLOAT_EQ(max(x,y,z),30);
    EXPECT_FLOAT_EQ(max(x,y,z,w),30);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
