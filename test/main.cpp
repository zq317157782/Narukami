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

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
