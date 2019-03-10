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

TEST(math,deg2rad){
    float deg=180;
    float rad=deg2rad(deg);
    EXPECT_FLOAT_EQ(rad,3.1415927f);
}

TEST(math,rad2deg){
    float rad=3.1415927f;
    float deg=rad2deg(rad);
    EXPECT_FLOAT_EQ(deg,180);
}

#include "core/vector.h"
TEST(type,int_div_zero){
    int a= 1/0;
    EXPECT_EQ(isnan(a),false);
}

TEST(vector3f,add){
    Vector3f v1(1,2,3);
    Vector3f v2(4,5,6);
    auto v3=v1+v2;
    EXPECT_EQ(v3,Vector3f(5,7,9));
}

TEST(vector3f,sub){
    Vector3f v1(1,2,3);
    Vector3f v2(4,5,6);
    auto v3=v1-v2;
    EXPECT_EQ(v3,Vector3f(-3,-3,-3));
}


TEST(vector3f,mul){
    Vector3f v1(1,2,3);
    Vector3f v2(4,5,6);
    auto v3=v1*v2;
    EXPECT_EQ(v3,Vector3f(4,10,18));
}

TEST(vector3f,mul2){
    Vector3f v1(1,2,3);
    float f=2; 
    auto v2=v1*f;
    EXPECT_EQ(v2,Vector3f(2,4,6));
}

TEST(vector3f,div){
    Vector3f v1(1,2,3);
    float f=2; 
    auto v2=v1/f;
    EXPECT_EQ(v2,Vector3f(0.5f,1.0f,1.5f));
}

TEST(vector3f,equal){
    Vector3f v1(1);
    Vector3f v2(2);
    Vector3f v3(2);
    EXPECT_TRUE(v1!=v3);
    EXPECT_TRUE(v2==v3);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
