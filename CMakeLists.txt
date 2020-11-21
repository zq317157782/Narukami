cmake_minimum_required(VERSION 3.0.0)
project(narukami VERSION 0.1.0)

include(CTest)
enable_testing()

#set(CMAKE_BUILD_TYPE release)
if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
else() 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
endif()
#####################################################################################################################
#所有的头文件的路径
include_directories(src)
include_directories(external)
include_directories(external/benchmark/include/benchmark)
include_directories(external/lodepng)
include_directories(external/lua/src)
include_directories(external/tinyobjloader)
include_directories(external/glfw/include)
include_directories(external/tinyply/source)
include_directories(external/cyCodeBase)
#####################################################################################################################
#所有的CPP的路径
aux_source_directory(src/core SRC_CORE)
aux_source_directory(src/cameras SRC_CAMERAS)
aux_source_directory(src/materials SRC_MATERIALS)
aux_source_directory(external/lodepng EXT_LODEPNG)
set(SRC_ALL ${SRC_CORE} ${SRC_MATERIALS} ${SRC_CAMERAS}  ${EXT_LODEPNG})
#####################################################################################################################
#add gtest
set(gtest_force_shared_crt ON)
set(BUILD_GMOCK OFF)
set(BUILD_GTEST ON)
add_subdirectory(external/googletest)   
# #add glog
# set(WITH_GFLAGS OFF)
# add_subdirectory(external/glog)
#add lua
add_subdirectory(external/lua)
#add tinyobjloader
add_subdirectory(external/tinyobjloader)
# #add sdl2
# add_subdirectory(external/sdl2)
add_subdirectory(external/glfw)
add_subdirectory(external/tinyply)
#add embree3
# set(EMBREE_ISPC_SUPPORT OFF)
# set(EMBREE_TUTORIALS OFF)
# set(EMBREE_TASKING_SYSTEM "NO") #TODO change to TBB
# add_subdirectory(external/embree)
#add benchmark
SET(BENCHMARK_ENABLE_GTEST_TESTS OFF)
add_subdirectory(external/benchmark)
#openGL
find_package(OPENGL REQUIRED)
#narukami
add_executable(narukami src/main.cpp ${SRC_ALL})
target_link_libraries(narukami tinyobjloader)
# target_link_libraries(narukami embree)
#####################################################################################################################
#tools
add_executable(blas_bvh_view src/tools/blas_bvh_view.cpp ${SRC_ALL})
target_link_libraries(blas_bvh_view glfw tinyobjloader tinyply ${OPENGL_LIBRARIES}) 

#narukami_test
add_executable(narukami_test test/main.cpp ${SRC_ALL})
target_link_libraries(narukami_test gtest lua5.3.4)

#benchmark 
add_executable(narukami_benchmark benchmark/main.cpp ${SRC_ALL})
target_link_libraries(narukami_benchmark benchmark)

 

#samples
#add_executable(sample_render_a_soatriangle sample/render_a_soatriangle.cpp ${SRC_ALL})
add_executable(sample_render_a_mesh sample/render_a_mesh.cpp ${SRC_ALL})
target_link_libraries(sample_render_a_mesh tinyobjloader tinyply)
add_executable(sample_render_xyz_to_rgb sample/render_xyz_to_rgb.cpp ${SRC_ALL})
#add_executable(sample_render_a_texture_space sample/render_a_texture_space.cpp ${SRC_ALL})
#add_executable(sample_render_a_bounds sample/render_a_bounds.cpp ${SRC_ALL})
#add_executable(sample_output_2d_random_number sample/output_2d_random_number.cpp ${SRC_ALL})
#add_executable(sample_output_sampler_data sample/output_sampler_data.cpp ${SRC_ALL})
#add_executable(sample_render_a_sample_distribution sample/render_a_sample_distribution.cpp ${SRC_ALL})
#add_executable(sample_accelerator_test sample/accelerator_test.cpp ${SRC_ALL})
#target_link_libraries(sample_accelerator_test tinyobjloader tinyply)
#add_executable(sample_test_parallel_for sample/test_parallel_for.cpp ${SRC_ALL})
#add_executable(sample_test_animatedtransform sample/test_animatedtrasform.cpp ${SRC_ALL})


 
if(MSVC)
  if(${MSVC_VERSION} GREATER_EQUAL 1915)
    # You must acknowledge that you understand MSVC resolved a byte alignment issue in this compiler
    # We get this due to using Eigen objects and allocating those objects with make_shared
    target_compile_definitions( narukami_test PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE )
    target_compile_definitions( sample_render_a_mesh PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE )
    #target_compile_definitions( sample_accelerator_test PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE )
  endif()
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 