#include "benchmark.h"
#include <xmmintrin.h>
#include "core/affine.h"
#include "core/geometry.h"
using namespace narukami;
static void BM_common_rcp(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {   
        x++;
        benchmark::DoNotOptimize(ret += 1.0f /x);
    }
}
BENCHMARK(BM_common_rcp);

static void BM_SSE_rcp(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += narukami::rcp(x));
    }
}

BENCHMARK(BM_SSE_rcp);

static void BM_common_rsqrt(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += 1.0f / std::sqrt(x));
    }
}
BENCHMARK(BM_common_rsqrt);

static void BM_SSE_rsqrt(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += narukami::rsqrt(x));
    }
}
BENCHMARK(BM_SSE_rsqrt);

static void BM_fast_rsqrt(benchmark::State &state)
{
    float ret = 0;
    float x = 100.0f;
    for (auto _ : state)
    {
        x++;
        benchmark::DoNotOptimize(ret += narukami::fast_rsqrt(x));
    }
}
BENCHMARK(BM_fast_rsqrt);


template <typename T>
inline narukami::Vector3f normalize_v1(const narukami::Vector3<T> &v1)
{
    float l = narukami::length(v1);
    return v1 / l;
}

template <typename T>
inline narukami::Vector3f normalize_v2(const narukami::Vector3<T> &v1)
{
    float inv_l = narukami::rsqrt(dot(v1, v1));
    return v1 * inv_l;
}

template <typename T>
inline narukami::Vector3f normalize_v3(const narukami::Vector3<T> &v1)
{
    const __m128 x = _mm_set_ps(1.0f, v1.z, v1.y, v1.x);
    const __m128 s = _mm_mul_ps(x, x);
    const __m128 t = _mm_add_ss(s, _mm_movehl_ps(s, s));
    const __m128 pa = _mm_max_ss(_mm_add_ss(t, _mm_shuffle_ps(t, t, 1)), _mm_set_ss(1.0e-30f));
    const __m128 r = _mm_rsqrt_ss(pa);
    // one more iteration
    return v1 * _mm_cvtss_f32(_mm_mul_ss(r, _mm_add_ss(_mm_set_ss(1.5f),
                                                       _mm_mul_ss(_mm_mul_ss(pa, _mm_set_ss(-0.5f)), _mm_mul_ss(r, r)))));
}

template <typename T>
inline narukami::Vector3f normalize_v4(const narukami::Vector3<T> &a)
{
    const __m128 pa = _mm_max_ss(_mm_set_ss(a.x * a.x + a.y * a.y + a.z * a.z), _mm_set_ss(1.0e-30f));
    const __m128 r = _mm_rsqrt_ss(pa);
    // one more iteration
    const float d = _mm_cvtss_f32(_mm_mul_ss(r, _mm_add_ss(_mm_set_ss(1.5f),
                                                           _mm_mul_ss(_mm_mul_ss(pa, _mm_set_ss(-0.5f)), _mm_mul_ss(r, r)))));
    return a * d;
}

static void BM_normalize_v1(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = (float)rand();
    float y = (float)rand();
    float z = (float)rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1 += normalize_v1(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v1);

static void BM_normalize_v2(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = (float)rand();
    float y = (float)rand();
    float z = (float)rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1 += normalize_v2(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v2);

static void BM_normalize_v3(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = (float)rand();
    float y = (float)rand();
    float z = (float)rand();
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(v1 += normalize_v3(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v3);

static void BM_normalize_v4(benchmark::State &state)
{
    narukami::Vector3f v1(0, 0, 0);
    srand(0);
    float x = (float)rand();
    float y = (float)rand();
    float z = (float)rand();
    for (auto _ : state)
    {

        benchmark::DoNotOptimize(v1 += normalize_v4(narukami::Vector3f(x, y, z)));
    }
}
BENCHMARK(BM_normalize_v4);

static void BM_vector3_cross(benchmark::State &state)
{
    auto data = new float[6*(size_t)state.range(0)];
    for(size_t i=0;i<6*state.range(0);++i){
        data[i]=(float)rand();
    }
    for (auto _ : state)
    {
        for(int i=0;i<state.range(0);++i){
            benchmark::DoNotOptimize(cross(narukami::Vector3f(data[6*i],data[6*i+1], data[6*i+2]),narukami::Vector3f(data[6*i+3], data[6*i+4], data[6*i+5])));
        }
    }
    delete [] data;
}
BENCHMARK(BM_vector3_cross)->Arg(1)->Arg(5)->Arg(10);

static void BM_ssevector3_cross(benchmark::State &state)
{
   auto data = new float[6*(size_t)state.range(0)];
    for(int i=0;i<6*state.range(0);++i){
        data[i]=(float)rand();
    }
    for (auto _ : state)
    {
        for(int i=0;i<state.range(0);++i){
            benchmark::DoNotOptimize(cross(narukami::SSEVector3f(data[6*i],data[6*i+1], data[6*i+2]),narukami::SSEVector3f(data[6*i+3], data[6*i+4], data[6*i+5])));
        }
    }
    delete [] data;
}
BENCHMARK(BM_ssevector3_cross)->Arg(1)->Arg(5)->Arg(10);;

static void BM_soavector3_cross(benchmark::State &state)
{
   
    auto data = new float[6*(size_t)state.range(0)];
    for(int i=0;i<6*state.range(0);++i){
        data[i]=(float)rand();
    }

    for (auto _ : state)
    {
        for(int i=0;i<state.range(0);++i){
            benchmark::DoNotOptimize(cross(narukami::SoAVector3f(data[6*i],data[6*i+1], data[6*i+2]),narukami::SoAVector3f(data[6*i+3], data[6*i+4], data[6*i+5])));
        }
    }
    delete [] data;
}
BENCHMARK(BM_soavector3_cross)->Arg(1)->Arg(5)->Arg(10);


static void BM_soavector3f_dot(benchmark::State &state)
{
   
    auto data = new float[6*(size_t)state.range(0)];
    for(int i=0;i<6*state.range(0);++i){
        data[i]=(float)rand();
    }
    for (auto _ : state)
    {
        for(int i=0;i<state.range(0);++i){
            benchmark::DoNotOptimize(dot(narukami::SoAVector3f(data[6*i],data[6*i+1], data[6*i+2]),narukami::SoAVector3f(data[6*i+3], data[6*i+4], data[6*i+5])));
        }
    }
    delete [] data;
}
BENCHMARK(BM_soavector3f_dot)->Arg(1)->Arg(5)->Arg(10);

inline narukami::Vector3f matMulVector_SSE(const narukami::Matrix4x4& M,const narukami::Vector3f& v){
     __m128 xyzw = _mm_set_ps(0.0f,v.z,v.y,v.x);
     
     narukami::float4 r=narukami::float4(M.col[0])*narukami::float4(v.x);
     r+=narukami::float4(M.col[1])*narukami::float4(v.y);
     r+=narukami::float4(M.col[2])*narukami::float4(v.z);

    //float w=sum(_mm_add_ps(M.col[3],xyzw));
    float rr[4];
    _mm_store_ps(rr,r.xyzw);
    return narukami::Vector3f(rr[0],rr[1],rr[2]);
}

inline narukami::Vector3f matMulVector(const narukami::Matrix4x4& M,const narukami::Vector3f& v){
    float x =  M.m[0]*v.x+M.m[4]*v.y+M.m[8]*v.z;
    float y =  M.m[1]*v.x+M.m[5]*v.y+M.m[9]*v.z;
    float z =  M.m[2]*v.x+M.m[6]*v.y+M.m[10]*v.z;

    return narukami::Vector3f(x,y,z);
}

static void BM_matrix4x4_mul_vector3_sse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Vector3f v;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v=matMulVector_SSE(M,v));
    }
}
BENCHMARK(BM_matrix4x4_mul_vector3_sse);

static void BM_matrix4x4_mul_vector3(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Vector3f v;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v=matMulVector(M,v));
    }
}
BENCHMARK(BM_matrix4x4_mul_vector3);



inline narukami::Matrix4x4 matMul_v2(const narukami::Matrix4x4& A,const narukami::Matrix4x4& B){
    narukami::Matrix4x4 ret;
    for(int r=0;r<4;++r){
        for(int c=0;c<4;++c){
            ret[c*4+r]=A.m[c*4+r]*B.m[c*4+r]+A.m[(c+1)*4+r]*B.m[c*4+(r+1)]+A.m[(c+2)*4+r]*B.m[c*4+(r+2)]+A.m[(c+3)*4+r]*B.m[c*4+(r+3)];
        }
    }   
    return ret;
}

static void BM_matrix4x4_mul_matrix4x4(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M=matMul_v2(M,M2));
    }
}
BENCHMARK(BM_matrix4x4_mul_matrix4x4);

static void BM_matrix4x4_mul_matrix4x4_sse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M=M*M2);
    }
}
BENCHMARK(BM_matrix4x4_mul_matrix4x4_sse);



static void BM_matrix4x4_transform_inverse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M2=narukami::transform_inverse(M));
    }
}
BENCHMARK(BM_matrix4x4_transform_inverse);

static void BM_matrix4x4_inverse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M2=narukami::inverse(M));
    }
}
BENCHMARK(BM_matrix4x4_inverse);


narukami::Matrix4x4 pbrt_inverse(const narukami::Matrix4x4 &m) {
	int indxc[4], indxr[4];
	int ipiv[4] = { 0, 0, 0, 0 };
	float minv[4][4];
	memcpy(minv, m.m, 4 * 4 * sizeof(float));
	for (int i = 0; i < 4; i++) {
		int irow = 0, icol = 0;
		float big = 0.f;
		// Choose pivot
		for (int j = 0; j < 4; j++) {
			if (ipiv[j] != 1) {
				for (int k = 0; k < 4; k++) {
					if (ipiv[k] == 0) {
						if (std::abs(minv[j][k]) >= big) {
							big = float(std::abs(minv[j][k]));
							irow = j;
							icol = k;
						}
					}
				}
			}
		}
		++ipiv[icol];
		// Swap rows _irow_ and _icol_ for pivot
		if (irow != icol) {
			for (int k = 0; k < 4; ++k)
				std::swap(minv[irow][k], minv[icol][k]);
		}
		indxr[i] = irow;
		indxc[i] = icol;
		

		// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
		float pivinv = 1.f / minv[icol][icol];
		minv[icol][icol] = 1.;
		for (int j = 0; j < 4; j++)
			minv[icol][j] *= pivinv;

		// Subtract this row from others to zero out their columns
		for (int j = 0; j < 4; j++) {
			if (j != icol) {
				float save = minv[j][icol];
				minv[j][icol] = 0;
				for (int k = 0; k < 4; k++)
					minv[j][k] -= minv[icol][k] * save;
			}
		}
	}
	// Swap columns to reflect permutation
	for (int j = 3; j >= 0; j--) {
		if (indxr[j] != indxc[j]) {
			for (int k = 0; k < 4; k++)
				std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
		}
	}
	return narukami::Matrix4x4((float*)minv);
}

static void BM_matrix4x4_inverse_pbrt(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Matrix4x4 M2;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(M2=pbrt_inverse(M));
    }
}
BENCHMARK(BM_matrix4x4_inverse_pbrt);

static void BM_matrix_mul_vector3(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Vector3f  v;
    narukami::Vector3f  v2;
    narukami::Vector3f  v3;
    narukami::Vector3f  v4;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v=M*v);
         benchmark::DoNotOptimize(v2=M*v2);
         benchmark::DoNotOptimize(v3=M*v3);
         benchmark::DoNotOptimize(v4=M*v4);
    }
}
BENCHMARK(BM_matrix_mul_vector3);

static void BM_matrix4x4_mul_soavector3(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::SoAVector3f  v;
    narukami::SoAVector3f  v2;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v2=M*v);
    }
}
BENCHMARK(BM_matrix4x4_mul_soavector3);


inline narukami::Point3f mat_mul_point3(const narukami::Matrix4x4& M,const narukami::Point3f& v){
    float x =  M.m[0]*v.x+M.m[4]*v.y+M.m[8]*v.z+M.m[12];
    float y =  M.m[1]*v.x+M.m[5]*v.y+M.m[9]*v.z+M.m[13];
    float z =  M.m[2]*v.x+M.m[6]*v.y+M.m[10]*v.z+M.m[14];

    return narukami::Point3f(x,y,z);
}
static void BM_matrix4x4_mul_point3(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Point3f v;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v=mat_mul_point3(M,v));
    }
}
BENCHMARK(BM_matrix4x4_mul_point3);



inline narukami::Point3f mat_mul_point3_sse(const narukami::Matrix4x4& M,const narukami::Point3f& v){
    narukami::float4 r=narukami::float4(M.col[0])*narukami::float4(v.x);
    r+=narukami::float4(M.col[1])*narukami::float4(v.y);
    r+=narukami::float4(M.col[2])*narukami::float4(v.z);
    r+=narukami::float4(M.col[3])*narukami::float4(1.0f);
    return narukami::Point3f(r.x,r.y,r.z);
}


static void BM_matrix4x4_mul_point3_sse(benchmark::State &state)
{
    narukami::Matrix4x4 M;
    narukami::Point3f v;
    float b=0;
    for (auto _ : state)
    {
         benchmark::DoNotOptimize(v=mat_mul_point3_sse(M,v));
    }
}
BENCHMARK(BM_matrix4x4_mul_point3_sse);






static void BM_intersect_triangle(benchmark::State &state)
{    float t;
    narukami::Point2f uv;
     narukami::Ray r(narukami::Point3f((float)rand(),(float)rand(),(float)rand()),narukami::Vector3f((float)rand(),(float)rand(),(float)rand()));
    auto triangles = new narukami::Triangle[(size_t)state.range(0)];
    
    for(size_t i = 0; i < state.range(0); i++)
    {
       triangles[i].v0=  narukami::Point3f((float)rand(),(float)rand(),(float)rand());
       triangles[i].e1 = narukami::Vector3f((float)rand(),(float)rand(),(float)rand());
       triangles[i].e2 = narukami::Vector3f((float)rand(),(float)rand(),(float)rand());
    }
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(intersect(r,triangles[i],&t,&uv));
        }
        
    }
    delete[] triangles;
}
BENCHMARK(BM_intersect_triangle)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

static void BM_intersect_soatriangle(benchmark::State &state)
{
    narukami::Point2f uv;
    float t;
    int index;
    narukami::SoARay r(narukami::Point3f((float)rand(),(float)rand(),(float)rand()),narukami::Vector3f((float)rand(),(float)rand(),(float)rand()));
    auto triangles = new narukami::SoATriangle[(size_t)state.range(0)]();
    for(size_t i = 0; i < state.range(0); i++)
    {
       triangles[i].v0=narukami::SoAPoint3f((float)rand(),(float)rand(),(float)rand());
       triangles[i].e1 = narukami::SoAVector3f((float)rand(),(float)rand(),(float)rand());
       triangles[i].e2 = narukami::SoAVector3f((float)rand(),(float)rand(),(float)rand());
    }
    
    for (auto _ : state)
    {
        for(size_t i = 0; i < state.range(0); i++)
        {
             benchmark::DoNotOptimize(intersect(r,triangles[i],&t,&uv,&index));
        }
    }

    delete[] triangles;
}
BENCHMARK(BM_intersect_soatriangle)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


static void BM_intersect_soatriangle2(benchmark::State &state)
{
    narukami::Point2f uv;
    float t;
    int index;

    narukami::float4 t_results;
    narukami::float4 u_results;
    narukami::float4 v_results;

    narukami::SoARay r(narukami::Point3f((float)rand(),(float)rand(),(float)rand()),narukami::Vector3f((float)rand(),(float)rand(),(float)rand()));
    auto triangles = new narukami::SoATriangle[(size_t)state.range(0)]();
    for(size_t i = 0; i < state.range(0); i++)
    {
       triangles[i].v0=narukami::SoAPoint3f((float)rand(),(float)rand(),(float)rand());
       triangles[i].e1 = narukami::SoAVector3f((float)rand(),(float)rand(),(float)rand());
       triangles[i].e2 = narukami::SoAVector3f((float)rand(),(float)rand(),(float)rand());
    }
    
    for (auto _ : state)
    {
        for(size_t i = 0; i < state.range(0); i++)
        {   
             narukami::bool4 mask;
             benchmark::DoNotOptimize(mask=intersect(r,triangles[i],&t_results,&u_results,&v_results));
             benchmark::DoNotOptimize(check(mask,t_results,u_results,v_results,&t,&uv));
        }
    }

    delete[] triangles;
}
BENCHMARK(BM_intersect_soatriangle2)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

static void BM_sub_matrix3x3_determinant_sse(benchmark::State &state)
{   narukami::Matrix4x4 mat(1,0,0,0,0,1,0,0,0,0,1,0,1,2,3,1);
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(sub_matrix3x3_determinant(mat));
        }
        
    }

}
BENCHMARK(BM_sub_matrix3x3_determinant_sse)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


float sub_matrix3x3_determinant_common(const narukami::Matrix4x4& mat){
    return mat.m[0]*mat.m[5]*mat.m[10]+mat.m[1]*mat.m[6]*mat.m[8]+mat.m[2]*mat.m[4]*mat.m[9]-mat.m[2]*mat.m[5]*mat.m[8]+mat.m[1]*mat.m[4]*mat.m[10]+mat.m[0]*mat.m[6]*mat.m[9];
}

static void BM_sub_matrix3x3_determinant_common(benchmark::State &state)
{   narukami::Matrix4x4 mat(1,0,0,0,0,1,0,0,0,0,1,0,1,2,3,1);
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(sub_matrix3x3_determinant_common(mat));
        }
        
    }

}
BENCHMARK(BM_sub_matrix3x3_determinant_common)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

#include "core/transform.h"
#include "core/mesh.h"
static void BM_meshdata_intersect(benchmark::State &state)
{   
     std::vector<Point3f> vertices={Point3f(0,1,1),Point3f(0,0,1),Point3f(1,0,1),Point3f(1,1,1)};
     std::vector<Point2f> uvs = {Point2f(0,1),Point2f(0,0),Point2f(1,0),Point2f(1,1)};
     std::vector<Normal3f> normals;
     std::vector<uint32_t> indices={0,1,3,1,2,3};
     auto transform = translate(Vector3f(0,0,0));
     auto transform2 = translate(Vector3f(0,0,0));
     auto meshs=create_mesh_triangles(&transform,&transform2,indices,vertices,normals,uvs);
     float t;
     Point2f uv;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            for(size_t j=0;j<meshs.size();++j){
                Ray ray(Point3f(0.5f,0.5f,0.5f),narukami::Vector3f(0,0,1));
                benchmark::DoNotOptimize(intersect(ray,meshs[j],&t,&uv));
            }
        }
        
    }

}
BENCHMARK(BM_meshdata_intersect)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


static void BM_meshdata_intersect_sse(benchmark::State &state)
{   
    std::vector<Point3f> vertices={Point3f(0,1,1),Point3f(0,0,1),Point3f(1,0,1),Point3f(1,1,1)};
     std::vector<Point2f> uvs = {Point2f(0,1),Point2f(0,0),Point2f(1,0),Point2f(1,1)};
     std::vector<Normal3f> normals;
     std::vector<uint32_t> indices={0,1,3,1,2,3};
     auto transform = translate(Vector3f(0,0,0));
     auto transform2 = translate(Vector3f(0,0,0));
     auto meshs=create_mesh_triangles(&transform,&transform2,indices,vertices,normals,uvs);
     auto soa_triangles=SoA_pack(meshs);
     float t;
     Point2f uv;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            for(size_t j=0;j<soa_triangles.size();++j){
                SoARay ray(Point3f(0.5f,0.5f,0.5f),narukami::Vector3f(0,0,1));
                benchmark::DoNotOptimize(intersect(ray,soa_triangles[j],&t,&uv,nullptr));
            }
        }
        
    }

}
BENCHMARK(BM_meshdata_intersect_sse)->Arg(1)->Arg(5)->Arg(10)->Arg(50);
#include "core/rng.h"
static void BM_rng_next_float(benchmark::State &state)
{   narukami::RNG rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(rng.next_float());
        }
        
    }

}
BENCHMARK(BM_rng_next_float)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

static void BM_rng_next_uint32(benchmark::State &state)
{   narukami::RNG rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(rng.next_uint32());
        }
        
    }

}
BENCHMARK(BM_rng_next_uint32)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


static const float OneMinusEpsilon = 0.99999994f;


#define PCG32_DEFAULT_STATE 0x853c49e6748fea9bULL
#define PCG32_DEFAULT_STREAM 0xda3e39cb94b95bdbULL
#define PCG32_MULT 0x5851f42d4c957f2dULL
class RNG_pbrt {
  public:
    // RNG Public Methods
    RNG_pbrt();
    RNG_pbrt(uint64_t sequenceIndex) { SetSequence(sequenceIndex); }
    void SetSequence(uint64_t sequenceIndex);
    uint32_t UniformUInt32();
    uint32_t UniformUInt32(uint32_t b) {
        uint32_t threshold = (~b + 1u) % b;
        while (true) {
            uint32_t r = UniformUInt32();
            if (r >= threshold) return r % b;
        }
    }
    float UniformFloat() {
        //return std::min(OneMinusEpsilon, Float(UniformUInt32() * 0x1p-32f));
		return std::min(OneMinusEpsilon, float(UniformUInt32() * 2.3283064365386963e-10f));
    }
    template <typename Iterator>
    void Shuffle(Iterator begin, Iterator end) {
        for (Iterator it = end - 1; it > begin; --it)
            std::iter_swap(it,
                           begin + UniformUInt32((uint32_t)(it - begin + 1)));
    }
    void Advance(int64_t idelta) {
        uint64_t cur_mult = PCG32_MULT, cur_plus = _inc, acc_mult = 1u,
                 acc_plus = 0u, delta = (uint64_t)idelta;
        while (delta > 0) {
            if (delta & 1) {
                acc_mult *= cur_mult;
                acc_plus = acc_plus * cur_mult + cur_plus;
            }
            cur_plus = (cur_mult + 1) * cur_plus;
            cur_mult *= cur_mult;
            delta /= 2;
        }
        _state = acc_mult * _state + acc_plus;
    }
    int64_t operator-(const RNG_pbrt &other) const {
       
        uint64_t cur_mult = PCG32_MULT, cur_plus = _inc, cur_state = other._state,
                 the_bit = 1u, distance = 0u;
        while (_state != cur_state) {
            if ((_state & the_bit) != (cur_state & the_bit)) {
                cur_state = cur_state * cur_mult + cur_plus;
                distance |= the_bit;
            }
           
            the_bit <<= 1;
            cur_plus = (cur_mult + 1ULL) * cur_plus;
            cur_mult *= cur_mult;
        }
        return (int64_t)distance;
    }

  private:
    uint64_t _state, _inc;
};

inline RNG_pbrt::RNG_pbrt() : _state(PCG32_DEFAULT_STATE), _inc(PCG32_DEFAULT_STREAM) {}
inline void RNG_pbrt::SetSequence(uint64_t initseq) {
    _state = 0u;
    _inc = (initseq << 1u) | 1u;
    UniformUInt32();
    _state += PCG32_DEFAULT_STATE;
    UniformUInt32();
}

inline uint32_t RNG_pbrt::UniformUInt32() {
    uint64_t oldstate = _state;
    _state = oldstate * PCG32_MULT + _inc;
    uint32_t xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32_t rot = (uint32_t)(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}


static void BM_rng_pbrt_uniform_float(benchmark::State &state)
{   RNG_pbrt rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(rng.UniformFloat());
        }
        
    }

}
BENCHMARK(BM_rng_pbrt_uniform_float)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


static void BM_reverse_u32_bitwise_version(benchmark::State &state)
{   RNG_pbrt rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(narukami::reverse_bits_u32(i+1));
        }
        
    }

}
BENCHMARK(BM_reverse_u32_bitwise_version)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


template<uint32_t base> float radical_inverse_u32_pbrt(uint32_t x){
         float inv_base = 1.0f/base;
         uint32_t reverse = 0;
         float inv_baseN=1.0f;
         while(x){
             // int op
             uint32_t next = static_cast<uint32_t>(x*inv_base);
             uint32_t digit = x - next*base;
             reverse = reverse*base + digit;
              x=next;
             //float op
             inv_baseN*=inv_base;
         }

         return min(reverse*inv_baseN,ONE_MINUS_EPSILON);
}


static void BM_radical_inverse_u32_pbrt_version(benchmark::State &state)
{   RNG_pbrt rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(radical_inverse_u32_pbrt<3>(i+1));
        }
        
    }

}
BENCHMARK(BM_radical_inverse_u32_pbrt_version)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

#include "core/lowdiscrepancy.h"
static void BM_radical_inverse_u32_base3(benchmark::State &state)
{   RNG_pbrt rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(narukami::radical_inverse_u32<3>(i+1));
        }
        
    }

}
BENCHMARK(BM_radical_inverse_u32_base3)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


static void BM_radical_inverse_u32_base2(benchmark::State &state)
{   RNG_pbrt rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(narukami::radical_inverse_u32<2>(i+1));
        }
        
    }

}
BENCHMARK(BM_radical_inverse_u32_base2)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


static void BM_generate_sobol02_sample(benchmark::State &state)
{   RNG_pbrt rng;
    for (auto _ : state)
    {

        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(narukami::sample_sobol02(i));
        }
        
    }

}
BENCHMARK(BM_generate_sobol02_sample)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


#include "core/sampler.h"
static void BM_sampler_get_2D(benchmark::State &state)
{    
    narukami::Sampler s(32,(size_t)state.range(0));
    s.switch_pixel(Point2i(0,0));
    for (auto _ : state)
    {   
        for(size_t i = 0; i < state.range(0); i++)
        {
            benchmark::DoNotOptimize(s.get_2D());
        }
        
    }

}
BENCHMARK(BM_sampler_get_2D)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

class A{
    public:
    std::string value;
    A(const std::string v):value(v){}
    A(const A& a):value(a.value){ 
             
    }
    A& operator=(const A& a){
        value=a.value;
       
         return (*this);
    }
    //  A& operator=(A&& a){
    //     value=std::move(a.value);
    //     std::cout<<"copy"<<std::endl;
    // }
};

class B{
    public:
    std::string value;
    B(const std::string v):value(v){}
    B(const B& b):value(b.value){
          
    }
    B(B&& b):value(b.value){
         
    }
     B& operator=(const B& a){
        value=a.value;
        return (*this);
    }
     B& operator=(B&& a){
        value=std::move(a.value);
        return (*this);
    }
};


static void BM_construct_lvalue(benchmark::State &state)
{  
    for (auto _ : state)
    {
        for(size_t i = 0; i < state.range(0); i++)
        {
             A a("test");
             benchmark::DoNotOptimize(a=A("asdasdasdsadasdasdasdasdsadasd"));
        }
    }

}
BENCHMARK(BM_construct_lvalue)->Arg(1)->Arg(5)->Arg(10)->Arg(50);

static void BM_construct_rvalue(benchmark::State &state)
{  

    for (auto _ : state)
    {
        for(size_t i = 0; i < state.range(0); i++)
        {
           B b("test");
           benchmark::DoNotOptimize(b=B("asdasdasdsadasdasdasdasdsadasd"));
        }
    }

}
BENCHMARK(BM_construct_rvalue)->Arg(1)->Arg(5)->Arg(10)->Arg(50);


BENCHMARK_MAIN();