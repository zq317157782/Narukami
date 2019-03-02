#include "gtest/gtest.h"
#include "core/math.h"
using namespace narukami;
TEST(Math,rcp){
    EXPECT_FLOAT_EQ(rcp(2.0f),0.5f);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
