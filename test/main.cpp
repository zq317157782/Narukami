#include "gtest/gtest.h"
#include "core/affine.h"

using namespace narukami;

TEST(constant,ONE_MINUS_EPSILON){
   EXPECT_FLOAT_EQ(ONE_MINUS_EPSILON,1); 
}

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

TEST(math,sin){
    EXPECT_FLOAT_EQ(narukami::sin(deg2rad(90.0f)),1);
}

TEST(math,cos){
    EXPECT_LT(narukami::cos(deg2rad(90.0f)),0.0000001);
    EXPECT_GT(narukami::cos(deg2rad(90.0f)),-0.0000001);
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

TEST(math,reverse_bits_u32){
    uint32_t num=1;
    EXPECT_EQ(reverse_bits_u32(1),2147483648);
    EXPECT_EQ(reverse_bits_u32(2),1073741824);
    EXPECT_EQ(reverse_bits_u32(3),3221225472);
    EXPECT_EQ(reverse_bits_u32(4),536870912);
}


TEST(math,zero_mul_infinite){
    EXPECT_TRUE(isnan(0.0f*(1.0f/0.0f)));
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

// TEST(ssevector3f,sum){
//     SSEVector3f v1(1,2,3);
//     float a=reduce_add(v1);
//     EXPECT_EQ(a,6);
// }


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

TEST(vector3f,min){
    Vector3f p0(1,10,2);
    Vector3f p1(10,3,7);
    auto min_point=min(p0,p1);
    EXPECT_EQ(min_point,Vector3f(1,3,2));
}

TEST(vector3f,max){
    Vector3f p0(1,10,2);
    Vector3f p1(10,3,7);
    auto max_point=max(p0,p1);
    EXPECT_EQ(max_point,Vector3f(10,10,7));
}



TEST(point3f,min){
    Point3f p0(1,10,2);
    Point3f p1(10,3,7);
    auto min_point=min(p0,p1);
    EXPECT_EQ(min_point,Point3f(1,3,2));
}


TEST(point3f,max){
    Point3f p0(1,10,2);
    Point3f p1(10,3,7);
    auto max_point=max(p0,p1);
    EXPECT_EQ(max_point,Point3f(10,10,7));
}


TEST(soapoint3f,min){
    SoAPoint3f p0(1,10,2);
    SoAPoint3f p1(10,3,7);
    auto min_point=min(p0,p1);
    EXPECT_EQ(min_point,SoAPoint3f(1,3,2));
}

TEST(soapoint3f,max){
    SoAPoint3f p0(1,10,2);
    SoAPoint3f p1(10,3,7);
    auto max_point=max(p0,p1);
    EXPECT_EQ(max_point,SoAPoint3f(10,10,7));
}


// TEST(sse,_sse_blendv_ps){
//     __m128 a =_mm_set1_ps(5.0f);
//     __m128 b =_mm_set1_ps(6.0f);
//     __m128 mask = _mm_set1_ps(cast_i2f(0xFFFFFFFF));
//     __m128 ret=_sse_blendv_ps(a,b,mask);

//     EXPECT_FLOAT_EQ(_mm_cvtss_f32(ret),6.0F);
// }

TEST(sse,reduce_add){
    __m128 a =_mm_set_ps(1,2,3,4);
    EXPECT_FLOAT_EQ(reduce_add(a),10);
}

TEST(bool4,index){
    bool4 a(true);
    EXPECT_TRUE(a[2]);
}

TEST(bool4,not){
    bool4 a(true);
    EXPECT_TRUE(none(!(a)));
}

TEST(bool4,or){
    bool4 a(true);
    bool4 b(false);

    EXPECT_TRUE(all(a|b));
}

TEST(bool4,any){
    bool4 a(true,false,false,false);
    EXPECT_TRUE(any(a));
}

TEST(bool4,none){
    bool4 a(false);
    EXPECT_TRUE(none(a));
}

TEST(float4,eq){
    float4 a(1.0f,1.0f,1.0f,1.0f);
    float4 b(1.0f,1.0f,1.0f,1.0f);
    float4 c(1.0f,1.0f,1.0f,2.0f);
    EXPECT_TRUE(all(a==b));
    EXPECT_TRUE(any(a!=c));
}


TEST(float4,select){
    float4 a(1.0f,1.0f,1.0f,1.0f);
    float4 b(0.0f,0.0f,0.0f,0.0f);
    float4 c=select(bool4(true,false,true,false),a,b);
    EXPECT_TRUE(all(c==float4(1.0f,0.0f,1.0f,0.0f)));
}


TEST(bool4,select){
    bool4 a(true);
    bool4 b(false);
    bool4 c=select(bool4(true,false,true,false),a,b);
    EXPECT_TRUE(all(c==bool4(true,false,true,false)));
}


TEST(float4,positive_and_negative){
     float4 a(1.0f,1.0f,1.0f,1.0f);
     EXPECT_TRUE(all(-a==float4(-1.0f,-1.0f,-1.0f,-1.0f)));
}

TEST(float4 ,add){
    float4 a(1.0f);
    float4 b(1.0f);
    auto c = a+b;
    EXPECT_TRUE(all(c==float4(2.0f)));
    c+=a;
    EXPECT_TRUE(all(c==float4(3.0f)));
}

TEST(float4 ,sub){
    float4 a(1);
    float4 b(1);
    auto c = a-b;
    EXPECT_TRUE(all(c==float4(0.0f)));
    c-=a;
    EXPECT_TRUE(all(c==float4(-1.0f)));
}

TEST(float4 ,mul){
    float4 a(1.0f);
    float4 b(1.0f);
    auto c = a*b;
    EXPECT_TRUE(all(c==float4(1)));
    c*=a;
    EXPECT_TRUE(all(c==float4(1)));
}

TEST(float4 ,div){
    float4 a(1.0f);
    float4 b(1.0f);
    auto c = a/b;
    EXPECT_TRUE(all(c==float4(1)));
    c/=a;
    EXPECT_TRUE(all(c==float4(1)));
}


TEST(float4 ,rcp){
    float4 a(2.0f);
    auto b=rcp(a);
    EXPECT_FLOAT_EQ(b[0],0.5f);
    EXPECT_FLOAT_EQ(b[1],0.5f);
    EXPECT_FLOAT_EQ(b[2],0.5f);
    EXPECT_FLOAT_EQ(b[3],0.5f);
}
TEST(float4 ,reduce_min_mask){
    float4 a(5.0f,6.0f,3.0f,4.0f);
    float b;
    auto mask=reduce_min_mask(a,&b);
    EXPECT_EQ(b,3);
    EXPECT_EQ(movemask(mask),4);
}

TEST(float4 ,reduce_max_mask){
    float4 a(5.0f,6.0f,3.0f,4.0f);
    float b;
    auto mask=reduce_max_mask(a,&b);
    EXPECT_EQ(b,6);
    EXPECT_EQ(movemask(mask),2);
}

TEST(float4,vreduce_add){
    float4 a(1.0f,2.0f,3.0f,4.0f);
    auto b =vreduce_add(a);
    EXPECT_TRUE(all(b==float4(10)));
}
// TEST(float4 ,safe_rcp){
//     float4 a(0);
//     auto b=safe_rcp(a);
//     EXPECT_FLOAT_EQ(b[0],0.5f);
//     EXPECT_FLOAT_EQ(b[1],0.5f);
//     EXPECT_FLOAT_EQ(b[2],0.5f);
//     EXPECT_FLOAT_EQ(b[3],0.5f);
// }

TEST(float4 ,vreduce_min){
    float4 a(1.0f,2.0f,3.0f,4.0f);
    a = vreduce_min(a);
    EXPECT_TRUE(all(a==float4(1.0f,1.0f,1.0f,1.0f)));
}


TEST(float4 ,vreduce_max){
    float4 a(1.0f,2.0f,3.0f,4.0f);
    a = vreduce_max(a);
    EXPECT_TRUE(all(a==float4(4,4,4,4)));
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

TEST(matrix4x4,blockwise_inverse){
    Matrix4x4 mat;
    Matrix4x4 mat2 = blockwise_inverse(mat);
    EXPECT_TRUE(mat==mat2);

    Matrix4x4 mat3(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2);
    Matrix4x4 mat4=blockwise_inverse(mat3);
    Matrix4x4 mat5(0.5f,0,0,0,0,0.5f,0,0,0,0,0.5f,0,0,0,0,0.5f);
    EXPECT_EQ(mat5,mat4);
}

// TEST(matrix4x4,inverse){
//     Matrix4x4 mat;
//     Matrix4x4 mat2 = inverse(mat);
//     EXPECT_TRUE(mat==mat2);

//     Matrix4x4 mat3(2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2);
//     Matrix4x4 mat4=inverse(mat3);
//     Matrix4x4 mat5(0.5f,0,0,0,0,0.5f,0,0,0,0,0.5f,0,0,0,0,0.5f);
    
//     EXPECT_EQ(mat5,mat4);
// }


TEST(matrix4x4,mul2x2){
    auto m0=float4(1,0,0,1);
    auto m1=float4(mul2x2(m0,m0));
    EXPECT_TRUE(all(m0==m1));
}

TEST(matrix4x4,adj2x2){
    auto m0=float4(1,2,3,4);
    auto m1=float4(adj2x2(m0));
    auto m2=float4(4,-3,-2,1);
    EXPECT_TRUE(all(m2==m1));
}

TEST(matrix4x4,minor){
   Matrix4x4 mat;
   auto mat2=minor(mat);
   EXPECT_EQ(mat,mat2);
}

TEST(matrix4x4,cofactor){
   Matrix4x4 mat;
   auto mat2=cofactor(mat);
   EXPECT_EQ(mat,mat2);
}

TEST(matrix4x4,determinant){
   Matrix4x4 mat;
   auto det=determinant(mat);
   EXPECT_EQ(det,1);
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

TEST(matrix4x4,mat_mul_ssepoint){
    Matrix4x4 mat(1,0,0,0,0,1,0,0,0,0,1,0,1,2,3,1);
    SSEPoint3f p;
    auto p2=mat*p;
    EXPECT_EQ(p2,SSEPoint3f(1,2,3));
}

TEST(matrix4x4,mat_mul_soapoint){
    Matrix4x4 mat(1,0,0,0,0,1,0,0,0,0,1,0,1,2,3,1);
    SoAPoint3f p;
    auto p2=mat*p;
    EXPECT_EQ(p2,SoAPoint3f(1,2,3));
}

TEST(matrix4x4,sub_matrix3x3_determinant){
    Matrix4x4 mat(1,0,0,0,0,1,0,0,0,0,1,0,1,2,3,1);
    float det=sub_matrix3x3_determinant(mat);
    EXPECT_EQ(det,1);
}

TEST(SoAVector3f,eq){
    SoAVector3f v1;
    SoAVector3f v2(0);
    EXPECT_EQ(v1==v2,15);
}

TEST(SoAVector3f,not_eq){
    SoAVector3f v1;
    SoAVector3f v2(1);
    EXPECT_EQ(v1!=v2,15);
}

TEST(SoAVector3f,dot){
    SoAVector3f v1(1,2,0);
    SoAVector3f v2(1,2,0);
    auto value =dot(v1,v2);
    EXPECT_TRUE(all(float4(value)==float4(5)));
}


TEST(SoAVector3f,cross){
    SoAVector3f v1(1,0,0);
    SoAVector3f v2(0,1,0);
    SoAVector3f v3 =cross(v1,v2);
    EXPECT_EQ(v3,SoAVector3f(0,0,1));
}


// TEST(SoAVector3f,safe_rcp){
//     SoAVector3f v1(1,0,0);
//     auto v2 = safe_rcp(v1);
//     EXPECT_EQ(v2,SoAVector3f(0,0,1));
// }

TEST(Euclid,point_add_vector){
    Point3f  p(0,0,0);
    Vector3f v(1,0,0);

    Point3f p2=p+v;
    EXPECT_EQ(p2,Point3f(1,0,0));
}

TEST(Euclid,distance){
    Point3f p(0,0,0);
    Point3f p2(1,0,0);
    float d=distance(p,p2);
    EXPECT_FLOAT_EQ(d,1);
}

TEST(Euclid,distance_sse){
    SSEPoint3f p(0,0,0);
    SSEPoint3f p2(1,0,0);
    float d=distance(p,p2);
    EXPECT_FLOAT_EQ(d,1);
}


TEST(Euclid,point_minus_point){
    SoAPoint3f p(2);
    SoAPoint3f p2(1);
    SoAVector3f v=p-p2;

    EXPECT_EQ(v,SoAVector3f(1));
}
#include "core/geometry.h"
TEST(geometry,ray_intersect_triangle){
    Triangle triangle;
    triangle.v0 = Point3f(0,0,1);
    triangle.e1 = Vector3f(1,0,0);
    triangle.e2 = Vector3f(0,1,0);

    Ray r(Point3f(0,0,0),Vector3f(0,0,1));
    bool a=intersect(r,triangle);
    EXPECT_EQ(a,true);

    Ray r2(Point3f(1,0,0),Vector3f(0,0,1));
    bool a2=intersect(r2,triangle);
    EXPECT_EQ(a2,true);

    Ray r3(Point3f(1.1,0,0),Vector3f(0,0,1));
    bool a3=intersect(r3,triangle);
    EXPECT_EQ(a3,false);

    Ray r4(Point3f(0,1.1,0),Vector3f(0,0,1));
    bool a4=intersect(r4,triangle);
    EXPECT_EQ(a4,false);
}

TEST(geometry,get_normalized_normal){
    Triangle triangle;
    triangle.v0 = Point3f(0,0,0);
    triangle.e1 = Vector3f(1,0,0);
    triangle.e2 = Vector3f(0,1,0);
    
    auto N = get_normalized_normal(triangle);
    EXPECT_EQ(N,Normal3f(0,0,1));
}

TEST(geometry,ray_intersect_soatriangle){
    

    SoATriangle triangle;
    triangle.v0 = SoAPoint3f(0,0,2);
    triangle.e1 = SoAVector3f(1,0,0);
    triangle.e2 = SoAVector3f(0,1,0);
    float t;
    Point2f uv;

    SoARay r(Point3f(0,0,0),Vector3f(0,0,1));
    auto a=intersect(r,triangle);
    EXPECT_EQ((a),true);

    SoARay r2(Point3f(1,0,0),Vector3f(0,0,1));
    auto a2=intersect(r2,triangle,&t,&uv,nullptr,SSE_MASK(false,false,false,false));
    EXPECT_EQ((a2),false);



    SoARay r3(Point3f(1.1,0,0),Vector3f(0,0,1));
    auto a3=intersect(r3,triangle);
    EXPECT_EQ((a3),false);


    SoARay r4(Point3f(0,1.1,0),Vector3f(0,0,1));
    auto a4=intersect(r4,triangle);
    EXPECT_EQ((a4),false);
   
}

TEST(geometry,size_of_soatriangle){
    EXPECT_EQ(sizeof(SoATriangle),144);
}

TEST(SoAbound3f,intersect0){
    
    SoABounds3f bound(SoAPoint3f(Point3f(0.1f,0.1f,0.1f),Point3f(0.1f,0.6f,0.1f),Point3f(0.6f,0.1f,0.1f),Point3f(0.6f,0.6f,0.1f)),SoAPoint3f(Point3f(0.4f,0.4f,1.0f),Point3f(0.4f,0.9f,1.0f),Point3f(0.9f,0.4f,1.0f),Point3f(0.9f,0.9f,1.0f)));
    SoARay ray(Point3f(0.2f,0.2f,0),Vector3f(0,0,1));
    int p[3]={1,1,1};
    auto a=intersect(ray.o,SoAVector3f(INFINITE,INFINITE,1),float4(0.0f),float4(INFINITE),p,bound);
    EXPECT_EQ(any(a),true);
}


TEST(SoAbound3f,intersect1){
    
    SoABounds3f bound(SoAPoint3f(Point3f(0.1f,0.1f,0.1f),Point3f(0.1f,0.6f,0.1f),Point3f(0.6f,0.1f,0.1f),Point3f(0.6f,0.6f,0.1f)),SoAPoint3f(Point3f(0.4f,0.4f,1.0f),Point3f(0.4f,0.9f,1.0f),Point3f(0.9f,0.4f,1.0f),Point3f(0.9f,0.9f,1.0f)));
    SoARay ray(Point3f(0.2f,0.2f,0),Vector3f(0,0,1));
    int p[3]={1,1,1};
    float4 tHit;
    auto a=intersect(ray.o,safe_rcp(ray.d),float4(0.0f),float4(INFINITE),p,bound,&tHit);
    EXPECT_EQ(any(a),true);
    EXPECT_FLOAT_EQ(tHit[0],0.1f);
}


// #include "core/qbvh.h"

// TEST(qbvh,qbvhnode_size){
//     EXPECT_EQ(sizeof(QBVHNode),128);
// }


#include "core/memory.h"

TEST(memory,alloc_aligned){
    auto alloced_area=alloc_aligned<128>(1000);
    EXPECT_EQ(long(alloced_area)%128,0);
    auto alloced_area2=alloc_aligned<float,256>(1000);
    EXPECT_EQ(long(alloced_area2)%256,0);
}

TEST(memory,STACK_ALLOC){
    auto ptr = STACK_ALLOC(int,32);
}


#include "core/spectrum.h"
TEST(spectrum,index){
    Spectrum a(1,1,1);
    a[2]=2;
    EXPECT_EQ(a.b,2);
}

// TEST(databuffer,construct){
//     int data[3]={2,3,4};
//     DataBuffer<int> buf(10,data);
//     EXPECT_EQ(buf[2],4);
//     load(buf,data,3,3);
//     EXPECT_EQ(buf[3],2);
// }

// TEST(databuffer,assign){
//     int data[3]={2,3,4};
//     DataBuffer<int> buf(3,data);
//     DataBuffer<int> buf2 = buf;
//     EXPECT_EQ(buf2[2],4);

// }
#include "core/transform.h"
TEST(transform,translate){
    auto transform = translate(Vector3f(1,0,0));
    auto p0=transform.mat * Point3f(0,0,0);
    EXPECT_EQ(p0,Point3f(1,0,0));
    auto p1=transform.inv_mat * Point3f(0,0,0);
    EXPECT_EQ(p1,Point3f(-1,0,0));
}

TEST(transform,scale){
    auto transform = scale(0.5f,0.5f,0.5f);
    auto p0=transform.mat * Point3f(1,0,0);
    EXPECT_EQ(p0,Point3f(0.5f,0,0));
    auto p1=transform.inv_mat * Point3f(1,0,0);
    EXPECT_FLOAT_EQ(p1.x,2);
}

TEST(transform,rotate_x){
    auto transform = rotate_x(90);
    auto v0=transform.mat * Vector3f(0,0,1);
    EXPECT_FLOAT_EQ(v0.x,0.0f);
    EXPECT_FLOAT_EQ(v0.y,-1.0f);
   // EXPECT_FLOAT_EQ(v0.z,0.0f);
    auto v1=transform.inv_mat * Vector3f(0,0,1);
    EXPECT_FLOAT_EQ(v1.x,0.0f);
    EXPECT_FLOAT_EQ(v1.y,1.0f);
   // EXPECT_FLOAT_EQ(v1.z,0.0f);
}

TEST(transform,rotate_y){
    auto transform = rotate_y(90);
    auto v0=transform.mat * Vector3f(0,0,1);
    EXPECT_FLOAT_EQ(v0.x,1.0f);
    EXPECT_FLOAT_EQ(v0.y,0.0f);
   // EXPECT_FLOAT_EQ(v0.z,0.0f);
    auto v1=transform.inv_mat * Vector3f(0,0,1);
    EXPECT_FLOAT_EQ(v1.x,-1.0f);
    EXPECT_FLOAT_EQ(v1.y,0.0f);
   // EXPECT_FLOAT_EQ(v1.z,0.0f);
}

// TEST(transform,rotate_z){
//     auto transform = rotate_z(90);
//     auto v0=transform.mat * Vector3f(1,0,0);
//     EXPECT_FLOAT_EQ(v0.x,0.0f);
//     EXPECT_FLOAT_EQ(v0.y,1.0f);
//    // EXPECT_FLOAT_EQ(v0.z,0.0f);
//     auto v1=transform.inv_mat * Vector3f(1,0,0);
//     EXPECT_FLOAT_EQ(v1.x,0.0f);
//     EXPECT_FLOAT_EQ(v1.y,-1.0f);
//    // EXPECT_FLOAT_EQ(v1.z,0.0f);
// }

TEST(transform,rotate){
    auto transform = rotate(90,Vector3f(1,0,0));
    auto v0=transform.mat * Vector3f(0,0,1);
    EXPECT_FLOAT_EQ(v0.x,0.0f);
    EXPECT_FLOAT_EQ(v0.y,-1.0f);
    //EXPECT_FLOAT_EQ(v0.z,0.0f);
    auto v1=transform.inv_mat * Vector3f(0,0,1);
    EXPECT_FLOAT_EQ(v1.x,0.0f);
    EXPECT_FLOAT_EQ(v1.y,1.0f);
    //EXPECT_FLOAT_EQ(v1.z,0.0f);
}

TEST(transform,look_at){
    auto transform=look_at(Point3f(0,0,0),Point3f(5,0,0),Vector3f(0,1,0));
    // std::cout<<transform.mat;
    // std::cout<<transform.inv_mat;
    auto v0=transform.mat * Vector3f(0,0,1);
    EXPECT_FLOAT_EQ(v0.x,-1.0f);
    EXPECT_FLOAT_EQ(v0.y, 0.0f);
    EXPECT_FLOAT_EQ(v0.z, 0.0f);

    auto v1=transform.mat * Vector3f(1,0,0);
    EXPECT_FLOAT_EQ(v1.x, 0.0f);
    EXPECT_FLOAT_EQ(v1.y, 0.0f);
    EXPECT_FLOAT_EQ(v1.z, 1.0f);
    // std::cout<<transform.mat;
    // std::cout<<transform.inv_mat;
}

TEST(transform,vector3f){
    auto transform = rotate(90,Vector3f(1,0,0));
    auto v=transform(Vector3f(0,1,0));
    
    EXPECT_NEAR(v.x, 0.0f,0.00001);
    EXPECT_NEAR(v.y, 0.0f,0.00001);
    EXPECT_NEAR(v.z, 1.0f,0.00001);

}

TEST(transform,point3f){
    auto transform = translate(Vector3f(1,0,0));
    auto v=transform(Point3f(0,0,0));
    
    EXPECT_NEAR(v.x, 1.0f,0.00001);
    EXPECT_NEAR(v.y, 0.0f,0.00001);
    EXPECT_NEAR(v.z, 0.0f,0.00001);
}

TEST(transform,normal3f){
    auto transform = scale(2,1,1);
    auto v=transform(Normal3f(1,0,0));
    
    EXPECT_FLOAT_EQ(v.x, 0.5f);
    EXPECT_FLOAT_EQ(v.y, 0.0f);
    EXPECT_FLOAT_EQ(v.z, 0.0f);
}

TEST(transform,transform){
    auto transform = translate(Vector3f(1,0,0));
    //auto transform1 = rotate(90,Vector3f(0,1,0));
    auto transform2 = scale(2,2,2);
    auto transform3 = transform2((transform));

    EXPECT_EQ(transform3.mat,Matrix4x4(2,0,0,0,0,2,0,0,0,0,2,0,2,0,0,1));
}

TEST(transform,swap_handedness){
    auto transform = scale(-2,2,2);
    EXPECT_TRUE(swap_handedness(transform));
}

TEST(transform,perspective){
   auto persp = perspective(120,1,10);
   Point3f a(0,0,10);
   auto b = transform_h(persp,a);
   EXPECT_EQ(b,Point3f(0,0,1));
}


#include "core/mesh.h"


TEST(mesh,create_mesh_triangles){
     std::vector<Point3f> vertices={Point3f(0,1,0),Point3f(0,0,0),Point3f(1,0,0),Point3f(1,1,0)};
     std::vector<Normal3f> normals;
     std::vector<Point2f> uvs;
     std::vector<uint32_t> indices={0,1,2,1,2,3};
     auto transform = translate(Vector3f(1,0,0));
     auto transform2 = translate(Vector3f(-1,0,0));
     auto meshs=create_mesh_triangles(&transform,&transform2,indices,vertices,normals,uvs);
     EXPECT_EQ(meshs.size(),2);
     auto triange = meshs[0];
     auto p = (*triange)[1];
     EXPECT_EQ(p,Point3f(1,0,0));
}

TEST(mesh,get_world_bounds){
    std::vector<Point3f> vertices={Point3f(0,1,0),Point3f(0,0,0),Point3f(1,0,0),Point3f(1,1,0)};
     std::vector<Normal3f> normals;
     std::vector<Point2f> uvs;
     std::vector<uint32_t> indices={0,1,2,1,2,3};
     auto transform = translate(Vector3f(1,0,0));
     auto transform2 = translate(Vector3f(-1,0,0));
     auto meshs=create_mesh_triangles(&transform,&transform2,indices,vertices,normals,uvs);

     auto triangle_bounds=meshs[0]->world_bounds();
     Bounds3f b0{{1,0,0},{2,1,0}};
     EXPECT_EQ(triangle_bounds,b0);
}


#include "core/primitive.h"
TEST(primitive,create_primitives){
    //  std::vector<Point3f> vertices={Point3f(0,1,0),Point3f(0,0,0),Point3f(1,0,0),Point3f(1,1,0)};
    //  std::vector<Normal3f> normals;
    //  std::vector<Point2f> uvs;
    //  std::vector<uint32_t> indices={0,1,2,1,2,3};
    //  auto transform = translate(Vector3f(1,0,0));
    //  auto transform2 = translate(Vector3f(-1,0,0));
    //  auto triangles=create_mesh_triangles(&transform,&transform2,indices,vertices,normals,uvs);
    //  auto primitives=create_primitives(triangles);
    //  EXPECT_EQ(primitives.size(),2);
    //TODO mesh
}

#include "lua/narukami_lua.h"
TEST(lua,init){
    NarukamiLua lua;
    lua.init();
}


TEST(Bounds3f,intersect){
    Bounds3f bounds(Point3f(0,0,0),Point3f(1,1,1));
    int isPositive[3]={1,1,1};
    bool isHit=intersect(Point3f(0,0,0),Vector3f(INFINITE,INFINITE,1),0,1,isPositive,bounds);
    EXPECT_TRUE(isHit);
}

TEST(Bounds2f,area){
    Bounds2f b(Point2f(0,0),Point2f(1,1));
    EXPECT_EQ(area(b),1);
}

TEST(Bounds2i,area){
    Bounds2i b(Point2i(0,0),Point2i(2,2));
    EXPECT_EQ(area(b),4);
}
#include "core/film.h"
TEST(Film,sample_bounds){
    Film film(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
    auto bounds=film.get_sample_bounds();
    EXPECT_EQ(bounds.min_point,Point2i(-1,-1));
    EXPECT_EQ(bounds.max_point,Point2i(129,129));
}
#include "core/rng.h"
TEST(rng,next_float){
    RNG rng;
    
    float  f = rng.next_float();
    EXPECT_GE(f,0.0f);
    EXPECT_LE(f,1.0f);
}

TEST(rng,seed){
    RNG rng(1024);
    RNG rng2(1024);
    
    EXPECT_EQ(rng.next_uint32(),rng2.next_uint32());
}


#include "core/lowdiscrepancy.h"
TEST(lowdiscrepancy,radical_inverse){
    EXPECT_FLOAT_EQ(radical_inverse_u32<2>(1),0.5f);
    EXPECT_FLOAT_EQ(radical_inverse_u32<2>(2),0.25f);
    EXPECT_FLOAT_EQ(radical_inverse_u32<2>(3),0.75f);
    EXPECT_FLOAT_EQ(radical_inverse_u32<2>(4),0.125f);
    EXPECT_FLOAT_EQ(radical_inverse_u32<2>(5),0.625f);
    EXPECT_FLOAT_EQ(radical_inverse_u32<2>(6),0.375f);
}

TEST(lowdiscrepancy,scrambled_radical_inverse_u32_base2){
    uint32_t scramble = 0xFFFFFFFF;
    EXPECT_FLOAT_EQ(scrambled_radical_inverse_u32_base2(1,scramble),0.5f);
    EXPECT_FLOAT_EQ(scrambled_radical_inverse_u32_base2(2,scramble),0.75f);
    EXPECT_FLOAT_EQ(scrambled_radical_inverse_u32_base2(3,scramble),0.25f);
    EXPECT_FLOAT_EQ(scrambled_radical_inverse_u32_base2(4,scramble),0.875f);
    EXPECT_FLOAT_EQ(scrambled_radical_inverse_u32_base2(5,scramble),0.375f);
    EXPECT_FLOAT_EQ(scrambled_radical_inverse_u32_base2(6,scramble),0.625f);
}


TEST(lowdiscrepancy,sobol_multi_generator_matrix){
    EXPECT_EQ(sample_sobol02(0),Point2f(0.5f,0.5f));
}



TEST(lowdiscrepancy,sample_scrambled_gray_code_sobol02){
    uint32_t sx=0,sy=0;
    EXPECT_EQ(sample_scrambled_gray_code_sobol02(0,&sx,&sy),Point2f(0.5,0.5));
    EXPECT_EQ(sample_scrambled_gray_code_sobol02(1,&sx,&sy),Point2f(0.75,0.25));
}


TEST(lowdiscrepancy,sample_scrambled_gray_code_van_der_corput){
    uint32_t s=0;
    EXPECT_EQ(sample_scrambled_gray_code_van_der_corput(0,&s),0.5f);
    EXPECT_EQ(sample_scrambled_gray_code_van_der_corput(1,&s),0.75f);
}




TEST(narukami,count_trailing_zero){
    EXPECT_EQ(count_trailing_zero(1),0);
    EXPECT_EQ(count_trailing_zero(2),1);
}


#include "core/sampler.h"
// TEST(sampler,get_1d_array){
//     Sampler sampler(32);
//     sampler.request_1d_array(2);
//     sampler.commit();
//     sampler.start_pixel(Point2i(0,0));
//     auto array = sampler.get_1D_array(4);
//     EXPECT_TRUE(array.size()>0);
//     array = sampler.get_1D_array(4);
//     EXPECT_TRUE(array.size()>0);
//     array = sampler.get_1D_array(4);
//     EXPECT_FALSE(array.size()>0);
// }


// TEST(sampler,get_2d_array){
//     Sampler sampler(32);
//     sampler.request_2d_array(2);
//     sampler.commit();
//     sampler.start_pixel(Point2i(0,0));
//     auto array = sampler.get_2D_array(4);
//     EXPECT_TRUE(array.size()>0);
//     array = sampler.get_2D_array(4);
//     EXPECT_TRUE(array.size()>0);
//     array = sampler.get_2D_array(4);
//     EXPECT_FALSE(array.size()>0);
// }


TEST(sampler,clone){
    Sampler sampler(32);
    auto sampler2=sampler.clone(0);
    auto sampler3=sampler.clone(0);
    auto sampler4=sampler.clone(1);

    sampler2->switch_pixel(Point2i(0,0));
    sampler3->switch_pixel(Point2i(0,0));
    sampler4->switch_pixel(Point2i(0,0));

    EXPECT_EQ(sampler2->get_2D(),sampler3->get_2D());
    EXPECT_EQ(sampler2->get_spp(),sampler4->get_spp());

}
#include "cameras/orthographic.h"
#include "core/integrator.h"

TEST(orthographic,size){
    EXPECT_EQ(sizeof(OrthographicCamera)%16,0);
}

TEST(memory,make_unique){
   auto a= make_unique<int>(1);
   EXPECT_EQ(*a,1);
}


TEST(memory,memory_pool){
   MemoryPool<Point3f> mp;
   for(int i=0;i<100000;++i)
   {
        Point3f * p0 = mp.alloc();
        Point3f * p1 = mp.alloc();
        mp.dealloc(p0);
        mp.dealloc(p1);
   }
  
}



// TEST(integrator,integrator){
//     auto film = std::make_shared<Film>(Point2i(128,128),Bounds2f(Point2f(0,0),Point2f(1,1)));
//     Transform t;
//     Bounds2f b={{0,0},{1,1}};
//     auto a = std::make_shared<Transform>();
//     auto camera = std::shared_ptr<const Camera>( new OrthographicCamera(t,b,film));
//     auto sampler = std::make_shared<Sampler>(1024);
//     Integrator integrator(camera,sampler);
// }

TEST(memory,alloc_aligned_test){
    auto a =new(alloc_aligned<16>(4)) int(100);
    EXPECT_EQ(*a ,100);
}

TEST(transform,new){
    //auto a=std::make_shared<Transform>();
    //EXPECT_EQ(*a,Transform());
    auto a=new Transform();
    EXPECT_TRUE(((uint64_t)a%16)==0);
}


TEST(memory,MemoryArena){
    MemoryArena arena;
    auto a=arena.alloc<int>(256);
    for (size_t i = 0; i < 256; i++)
    {
        EXPECT_EQ(a[i],0);
    }
    
   
    arena.reset();
   
}

TEST(Bounds3f,_union){
    Point3f p0(0,0,0);
    Point3f p1(2,2,2);
    Bounds3f b0=_union(p0,p1);
    Bounds3f b1={{0,0,0},{2,2,2}};
    EXPECT_EQ(b0,b1);
    Bounds3f b2={{0,0,0},{-2,-2,-2}};
    b0=_union(b2,b0);
    Bounds3f b3={{2,2,2},{-2,-2,-2}};
    EXPECT_EQ(b0,b3);
}

TEST(Bounds3f,intersect1){
    Point3f p0(0,0,0);
    Point3f p1(2,2,2);
    Bounds3f b0=_union(p0,p1);
    Point3f p2(1,1,1);
    Point3f p3(3,3,3);
    Bounds3f b1=_union(p2,p3);

    Bounds3f b2 = intersect(b0,b1);
    Bounds3f b3 = Bounds3f(p2,p1);
    EXPECT_EQ(b2,b3);
}

TEST(Bounds3f,max_extent){
    Point3f p0(0,0,0);
    Point3f p1(1,2,3);
    Bounds3f b0=_union(p0,p1);
    EXPECT_EQ(max_extent(b0),2);
}
#include "core/accelerator.h"
TEST(QBVHNode,size){
    EXPECT_EQ(sizeof(QBVHNode),128);
}

TEST(QBVHNode,is_leaf){
    uint32_t a=0;
    EXPECT_TRUE(!is_leaf(a));
    a=leaf(10,4);
    EXPECT_TRUE(is_leaf(a));
}

TEST(QBVHNode,leaf_num){
    uint32_t a=0;
    a=leaf(10,4);
    EXPECT_EQ(leaf_num(a),4);
}

TEST(QBVHNode,leaf_offset){
    uint32_t a=0;
    a=leaf(10,4);
    EXPECT_EQ(leaf_offset(a),10);
}


TEST(math,max_nan){
    float zero=0.0f;
    float x=0.0f/zero;
    auto a =max(1.0f,x);
    EXPECT_TRUE(isnan(a));
}

TEST(float4,max_nan){
    float4 zero=float4(0.0f);
    float4 x=float4(0.0f)/zero;
    auto a =max(float4(1.0f),x);
    EXPECT_TRUE(isnan(a[0]));
    EXPECT_TRUE(isnan(a[1]));
    EXPECT_TRUE(isnan(a[2]));
    EXPECT_TRUE(isnan(a[3]));
}

TEST(bounds3f,offset){
    Bounds3f a={{0,0,0},{1,1,1}};
    auto b=offset(a,Point3f(0.5f,2.0f,-1.0f));
    EXPECT_EQ(b,Vector3f(0.5f,2.0f,-1.0f));
}

#include "core/stat.h"
STAT_MEMORY_COUNTER("memory_count",memory_count)
TEST(stat,memory_counter){
    STAT_INCREASE_MEMORY_COUNTER(memory_count,1024);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc,argv);
    auto ret = RUN_ALL_TESTS();
    return ret;
}
