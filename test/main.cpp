#include "gtest/gtest.h"
#include "core/math/math.h"
using namespace narukami;
TEST(math,rcp){
    EXPECT_FLOAT_EQ(rcp(2.0f),0.5f);
    EXPECT_FLOAT_EQ(rcp(3.0f),1.0f/3.0f);
    EXPECT_FLOAT_EQ(rcp(5.0f),1.0f/5.0f);
    EXPECT_FLOAT_EQ(rcp(7.0f),1.0f/7.0f);
}

TEST(math,isnan){
    float zero=0.0f;
    float x=0.0f/zero;
    EXPECT_TRUE(isnan(x));

    x=1.0f/zero;
    EXPECT_FALSE(isnan(x));
}

TEST(math,isinf){
    float zero=0.0f;
    float x=0.0f/zero;
    EXPECT_FALSE(isinf(x));

    x=1.0f/zero;
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

TEST(math,lerp){
    float a=lerp(0,1,0.5f);
    EXPECT_FLOAT_EQ(a,0.5f);
}

#include "core/math/vector2.h"
#include "core/math/vector3.h"
// TEST(type,int_div_zero){
//     int a= 1/0;
//     EXPECT_EQ(isnan(a),false);
// }

TEST(vector3f,add){
    Vector3f v1(1,2,3);
    Vector3f v2(4,5,6);
    auto v3=v1+v2;
    EXPECT_EQ(v3,Vector3f(5,7,9));
}

TEST(vector2f,add){
    Vector2f v1(1,2);
    Vector2f v2(4,5);
    auto v3=v1+v2;
    EXPECT_EQ(v3,Vector2f(5,7));
}

TEST(vector3f,sub){
    Vector3f v1(1,2,3);
    Vector3f v2(4,5,6);
    auto v3=v1-v2;
    EXPECT_EQ(v3,Vector3f(-3,-3,-3));
}

TEST(vector2f,sub){
    Vector2f v1(1,2);
    Vector2f v2(4,5);
    auto v3=v1-v2;
    EXPECT_EQ(v3,Vector2f(-3,-3));
}


TEST(vector3f,mul){
    Vector3f v1(1,2,3);
    Vector3f v2(4,5,6);
    auto v3=v1*v2;
    EXPECT_EQ(v3,Vector3f(4,10,18));
}

TEST(vector2f,mul){
    Vector2f v1(1,2);
    Vector2f v2(4,5);
    auto v3=v1*v2;
    EXPECT_EQ(v3,Vector2f(4,10));
}

TEST(vector3f,mul2){
    Vector3f v1(1,2,3);
    float f=2; 
    auto v2=v1*f;
    EXPECT_EQ(v2,Vector3f(2,4,6));
}
TEST(vector2f,mul2){
    Vector2f v1(1,2);
    float f=2; 
    auto v2=v1*f;
    EXPECT_EQ(v2,Vector2f(2,4));
}


TEST(vector3f,div){
    Vector3f v1(1,2,3);
    float f=2; 
    auto v2=v1/f;
    EXPECT_EQ(v2,Vector3f(0.5f,1.0f,1.5f));
}

TEST(vector2f,div){
    Vector2f v1(1,2);
    float f=2; 
    auto v2=v1/f;
    EXPECT_EQ(v2,Vector2f(0.5f,1.0f));
}

TEST(vector3f,equal){
    Vector3f v1(1);
    Vector3f v2(2);
    Vector3f v3(2);
    EXPECT_TRUE(v1!=v3);
    EXPECT_TRUE(v2==v3);
}

TEST(vector2f,equal){
    Vector2f v1(1);
    Vector2f v2(2);
    Vector2f v3(2);
    EXPECT_TRUE(v1!=v3);
    EXPECT_TRUE(v2==v3);
}

TEST(vector3f,cross){
    Vector3f v1(1,0,0);
    Vector3f v2(0,1,0);
    auto v3=cross(v1,v2);
    Vector3f v(0,0,1);
    EXPECT_EQ(v3,v);
}


TEST(vector3f,normalize){
    Vector3f v1(5,0,0);
    auto v2=normalize(v1);
    EXPECT_EQ(v2,Vector3f(1,0,0));
}

TEST(vector2f,normalize){
    Vector2f v1(5,0);
    auto v2=normalize(v1);
    EXPECT_EQ(v2,Vector2f(1,0));
}


TEST(vector3f,rcp){
    Vector3f v1(2,2,2);
    auto v2=rcp(v1);
    EXPECT_LE(v2.x,0.50001);
    EXPECT_GE(v2.x,0.49999);
}

TEST(vector2f,rcp){
    Vector2f v1(2,2);
    auto v2=rcp(v1);
    EXPECT_LE(v2.x,0.50001);
    EXPECT_GE(v2.x,0.49999);
}

TEST(vector3f,subscript){
    Vector3f v1(0);
    v1[0]=1;
    EXPECT_FLOAT_EQ(v1[0],1);
}

TEST(vector2f,subscript){
    Vector2f v1(0);
    v1[0]=1;
    EXPECT_FLOAT_EQ(v1[0],1);
}

TEST(vector2f,sqrt){
    Vector2f v1(9);
    auto v2=sqrt(v1);
    EXPECT_EQ(v2,Vector2f(3,3));
}

TEST(vector2f,Vector3fToVector2f){
    Vector3f v1(1,2,3);
    Vector2f v2(v1);
    EXPECT_EQ(v2,Vector2f(1,2));
}

TEST(vector3i,dot){
    Vector3i v1(1,2,3);
    Vector3i v2(1,2,3);
    int a=dot(v1,v2);
    EXPECT_EQ(a,14);
}

TEST(vector3i,length){
    Vector3i v1(1,2,3);
    float a=length(v1);
    EXPECT_FLOAT_EQ(a,3.7416575);
}





int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
