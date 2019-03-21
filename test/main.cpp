#include "gtest/gtest.h"
#include "core/math/euclid.h"

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


// TEST(type,int_div_zero){
//     int a= 1/0;
//     EXPECT_EQ(isnan(a),false);
// }

// TEST(vector3f,add){
//     Vector3f v1(1,2,3);
//     Vector3f v2(4,5,6);
//     auto v3=v1+v2;
//     EXPECT_EQ(v3,Vector3f(5,7,9));
// }



// TEST(vector3f,sub){
//     Vector3f v1(1,2,3);
//     Vector3f v2(4,5,6);
//     auto v3=v1-v2;
//     EXPECT_EQ(v3,Vector3f(-3,-3,-3));
// }




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
TEST(vector3f,rcp){
    Vector3f v1(2,2,2);
    auto v2=rcp(v1);
    EXPECT_LE(v2.x,0.50001);
    EXPECT_GE(v2.x,0.49999);
}
TEST(vector3f,subscript){
    Vector3f v1(0);
    v1[0]=1;
    EXPECT_FLOAT_EQ(v1[0],1);
}


TEST(vector3f,positive_and_negative){
    Vector3f v1(-1);
    auto v2= -v1;
    EXPECT_EQ(v2,Vector3f(1.0f));
}

TEST(vector3f,rsqrt){
    Vector3f v1(1,4,16);
    auto v2=rsqrt(v1);
    EXPECT_FLOAT_EQ(v2.x,1);
    EXPECT_FLOAT_EQ(v2.y,0.5f);
    EXPECT_FLOAT_EQ(v2.z,0.25f);
}


TEST(vector2f,add){
    Vector2f v1(1,2);
    Vector2f v2(4,5);
    auto v3=v1+v2;
    EXPECT_EQ(v3,Vector2f(5,7));
}
TEST(vector2f,sub){
    Vector2f v1(1,2);
    Vector2f v2(4,5);
    auto v3=v1-v2;
    EXPECT_EQ(v3,Vector2f(-3,-3));
}
TEST(vector2f,mul){
    Vector2f v1(1,2);
    Vector2f v2(4,5);
    auto v3=v1*v2;
    EXPECT_EQ(v3,Vector2f(4,10));
}
TEST(vector2f,mul2){
    Vector2f v1(1,2);
    float f=2; 
    auto v2=v1*f;
    EXPECT_EQ(v2,Vector2f(2,4));
}

TEST(vector2f,div){
    Vector2f v1(1,2);
    float f=2; 
    auto v2=v1/f;
    EXPECT_EQ(v2,Vector2f(0.5f,1.0f));
}
TEST(vector2f,equal){
    Vector2f v1(1);
    Vector2f v2(2);
    Vector2f v3(2);
    EXPECT_TRUE(v1!=v3);
    EXPECT_TRUE(v2==v3);
}
TEST(vector2f,normalize){
    Vector2f v1(5,0);
    auto v2=normalize(v1);
    EXPECT_EQ(v2,Vector2f(1,0));
}



TEST(vector2f,rcp){
    Vector2f v1(2,2);
    auto v2=rcp(v1);
    EXPECT_LE(v2.x,0.50001);
    EXPECT_GE(v2.x,0.49999);
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

TEST(ssevector3f,size){
    SSEVector3f v1;
    auto len=sizeof(v1);
    EXPECT_EQ(len,16);
}

TEST(ssevector3f,copy){
    Vector3f v1(1,1,1);
    SSEVector3f v2(v1);
    EXPECT_EQ(v2.x,1);
    EXPECT_EQ(v2.y,1);
    EXPECT_EQ(v2.z,1);
}


TEST(ssevector3f,eq){
    SSEVector3f v1(1);
    SSEVector3f v2(1);
    SSEVector3f v3(2);
    EXPECT_TRUE(v1==v2);
    EXPECT_TRUE(v1!=v3);
}


TEST(ssevector3f,negetive){
     SSEVector3f v1(1);
     SSEVector3f v2(-1);
     EXPECT_EQ(-v1,v2);
}

TEST(ssevector3f,abs){
     SSEVector3f v1(-1);
     SSEVector3f v2(1);
     EXPECT_EQ(abs(v1),v2);
}


TEST(ssevector3f,sign){
     SSEVector3f v1(-5);
     EXPECT_EQ(sign(v1),SSEVector3f(-1.0f));
     SSEVector3f v2(5);
     EXPECT_EQ(sign(v2),SSEVector3f(1.0f));
}

// TEST(ssevector3f,add){
//      SSEVector3f v1(-5);
//      SSEVector3f v2(5);
//      auto v3=v1+v2;
//      EXPECT_EQ(v3,SSEVector3f(0.0F));
// }

TEST(ssevector3f,sub){
     SSEVector3f v1(-5);
     SSEVector3f v2(5);
     auto v3=v1*v2;
     EXPECT_EQ(v3,SSEVector3f(-25.0F));
}


// TEST(ssevector3f,mul){
//      SSEVector3f v1(-5);
//      SSEVector3f v2(5);
//      auto v3=v1-v2;
//      EXPECT_EQ(v3,SSEVector3f(-10.0F));
// }

TEST(ssevector3f,div){
     SSEVector3f v1(-5);
     float div=5; 
     auto v3=v1/div;
     EXPECT_EQ(v3,SSEVector3f(-1.0F));
}

TEST(ssevector3f,cross){
    SSEVector3f v1(1,0,0);
    SSEVector3f v2(0,1,0);
    auto v3=cross(v1,v2);
    SSEVector3f v(0,0,1);
    EXPECT_EQ(v3,v);
}

TEST(ssevector3f,sum){
    SSEVector3f v1(1,2,3);
    float a=sum(v1);
    EXPECT_EQ(a,6);
}


TEST(ssevector3f,dot){
    SSEVector3f v1(1,2,3);
    SSEVector3f v2(1,2,3);
    int a=dot(v1,v2);
    EXPECT_EQ(a,14);
}

TEST(ssevector3f,rcp){
    SSEVector3f v1(2,2,2);
    auto v2=rcp(v1);
    EXPECT_FLOAT_EQ(v2[0],0.5f);
    EXPECT_FLOAT_EQ(v2[1],0.5f);
    EXPECT_FLOAT_EQ(v2[2],0.5f);
}


TEST(ssevector3f,length){
    SSEVector3f v1(1,0,0);
    float l=length(v1);
    EXPECT_FLOAT_EQ(l,1.0f);
}

TEST(ssevector3f,normalize){
    SSEVector3f v1(5,0,0);
    auto v2=normalize(v1);
    EXPECT_EQ(v2,SSEVector3f(1,0,0));
}

TEST(ssevector3f,rsqrt){
    SSEVector3f v1(1,4,16);
    auto v2=rsqrt(v1);
    EXPECT_FLOAT_EQ(v2.x,1);
    EXPECT_FLOAT_EQ(v2.y,0.5f);
    EXPECT_FLOAT_EQ(v2.z,0.25f);
}

// TEST(sse,_sse_blendv_ps){
//     __m128 a =_mm_set1_ps(5.0f);
//     __m128 b =_mm_set1_ps(6.0f);
//     __m128 mask = _mm_set1_ps(cast_i2f(0xFFFFFFFF));
//     __m128 ret=_sse_blendv_ps(a,b,mask);

//     EXPECT_FLOAT_EQ(_mm_cvtss_f32(ret),6.0F);
// }

TEST(sse,sum){
    __m128 a =_mm_set_ps(1,2,3,4);
    EXPECT_FLOAT_EQ(sum(a),10);
}



TEST(float4,eq){
    float4 a(1,1,1,1);
    float4 b(1,1,1,1);
    float4 c(1,1,1,2);
    EXPECT_TRUE(a==b);
    EXPECT_TRUE(a!=c);
}


TEST(float4,positive_and_negative){
     float4 a(1,1,1,1);
     EXPECT_EQ(-a,float4(-1,-1,-1,-1));
}

TEST(float4 ,add){
    float4 a(1);
    float4 b(1);
    auto c = a+b;
    EXPECT_EQ(c,float4(2));
    c+=a;
    EXPECT_EQ(c,float4(3));
}

TEST(float4 ,sub){
    float4 a(1);
    float4 b(1);
    auto c = a-b;
    EXPECT_EQ(c,float4(0));
    c-=a;
    EXPECT_EQ(c,float4(-1));
}

TEST(float4 ,mul){
    float4 a(1);
    float4 b(1);
    auto c = a*b;
    EXPECT_EQ(c,float4(1));
    c*=a;
    EXPECT_EQ(c,float4(1));
}

TEST(float4 ,div){
    float4 a(1);
    float4 b(1);
    auto c = a/b;
    EXPECT_EQ(c,float4(1));
    c/=a;
    EXPECT_EQ(c,float4(1));
}


TEST(float4 ,rcp){
    float4 a(2);
    auto b=rcp(a);
    EXPECT_FLOAT_EQ(b[0],0.5f);
    EXPECT_FLOAT_EQ(b[1],0.5f);
    EXPECT_FLOAT_EQ(b[2],0.5f);
    EXPECT_FLOAT_EQ(b[3],0.5f);
}




TEST(matrix4x4,init){
    Matrix4x4 mat;
    EXPECT_FLOAT_EQ(mat[0],1);
    EXPECT_FLOAT_EQ(mat[5],1);
    EXPECT_FLOAT_EQ(mat[10],1);
    EXPECT_FLOAT_EQ(mat[15],1);
}

TEST(matrix4x4,size){
    EXPECT_EQ(sizeof(Matrix4x4),64);
}

TEST(matrix4x4,identity){
    Matrix4x4 mat;
    Vector3f v1(1,0,0);
    auto v2=mat*v1;
    EXPECT_EQ(v2,v1);
    SSEVector3f v3(1,0,0);
    auto v4=mat*v3;
    EXPECT_EQ(v3,v4);
}

TEST(matrix4x4,mul){
    Matrix4x4 mat;
    Matrix4x4 mat2(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,0);
    auto r=mat*mat2;
    Vector3f v1(1,0,0);
    auto v2=r*v1;
    EXPECT_EQ(v2,Vector3f(2,0,0));   
}

TEST(matrix4x4,eq){
    Matrix4x4 mat;
    Matrix4x4 mat2;
    EXPECT_EQ(mat,mat2);
    Matrix4x4 mat3(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,2);
    EXPECT_TRUE(mat!=mat3);
    
}

TEST(matrix4x4,transform_inverse_noscale){
    Matrix4x4 mat;
    Matrix4x4 mat2 = transform_inverse_noscale(mat);
    EXPECT_TRUE(mat==mat2);

    Matrix4x4 mat3(1,0,0,0,0,1,0,0,0,0,1,0,1,1,1,1);
    Matrix4x4 mat4=transform_inverse_noscale(mat3);
    Matrix4x4 mat5(1,0,0,0,0,1,0,0,0,0,1,0,-1,-1,-1,1);
    EXPECT_EQ(mat5,mat4);
}


TEST(matrix4x4,transform_inverse){
    Matrix4x4 mat;
    Matrix4x4 mat2 = transform_inverse(mat);
    EXPECT_TRUE(mat==mat2);

    Matrix4x4 mat3(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,1);
    Matrix4x4 mat4=transform_inverse(mat3);
    Matrix4x4 mat5(0.5f,0,0,0,0,0.5f,0,0,0,0,0.5f,0,0,0,0,1);
    EXPECT_EQ(mat5,mat4);
}

TEST(matrix4x4,inverse){
    Matrix4x4 mat;
    Matrix4x4 mat2 = inverse(mat);
    EXPECT_TRUE(mat==mat2);

    Matrix4x4 mat3(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2);
    Matrix4x4 mat4=inverse(mat3);
    Matrix4x4 mat5(0.5f,0,0,0,0,0.5f,0,0,0,0,0.5f,0,0,0,0,0.5f);
    EXPECT_EQ(mat5,mat4);
}

TEST(matrix4x4,mul2x2){
    auto m0=float4(1,0,0,1);
    auto m1=float4(mul2x2(m0,m0));
    EXPECT_EQ(m0,m1);
}

TEST(matrix4x4,adj2x2){
    auto m0=float4(1,2,3,4);
    auto m1=float4(adj2x2(m0));
    auto m2=float4(4,-3,-2,1);
    EXPECT_EQ(m2,m1);
}



TEST(matrix4x4,transpose){
    //|1  5   9  13| 
    //|2  6  10  14|
    //|3  7  11  15|
    //|4  8  12  16|
    Matrix4x4 mat(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);
    auto mat2=transpose(mat);
    Matrix4x4 mat3(1,5,9,13,2,6,10,14,3,7,11,15,4,8,12,16);
    EXPECT_EQ(mat2,mat3);
}

TEST(matrix4x4,add){
    Matrix4x4 mat;
    Matrix4x4 mat2;
    auto mat3=mat+mat2;
    Matrix4x4 mat4(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2);
    EXPECT_EQ(mat4,mat3);
}

TEST(matrix4x4,soavector3){
    Matrix4x4 mat(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,1);
    SoAVector3f v(float4(1),float4(2),float4(3));
    auto v2=mat*v;
    EXPECT_EQ(v2,SoAVector3f(float4(2),float4(4),float4(6)));
}

TEST(matrix4x4,mat_mul_point){
    Matrix4x4 mat(1,0,0,0,0,1,0,0,0,0,1,0,1,2,3,1);
    Point3f p;
    auto p2=mat*p;
    EXPECT_EQ(p2,Point3f(1,2,3));
}

TEST(SoAVector3f,eq){
    SoAVector3f v1;
    SoAVector3f v2(0);
    EXPECT_EQ(v1,v2);
}

TEST(Euclid,point_add_vector){
    Point3f  p(0,0,0);
    Vector3f v(1,0,0);

    Point3f p2=p+v;
    EXPECT_EQ(p2,Point3f(1,0,0));
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
