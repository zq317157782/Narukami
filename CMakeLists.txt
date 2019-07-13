cmake_minimum_required(VERSION 3.0.0)
project(narukami VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_BUILD_TYPE release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")



include_directories(src)
include_directories(external)
#include_directories(external/embree/include/embree3)
include_directories(external/benchmark/include/benchmark)
include_directories(external/lodepng)
include_directories(external/lua/src)
include_directories(external/tinyobjloader)


aux_source_directory(src/core SRC_CORE)
aux_source_directory(src/cameras SRC_CAMERAS)
aux_source_directory(external/lodepng EXT_LODEPNG)
#add gtest
set(gtest_force_shared_crt ON)
set(BUILD_GMOCK OFF)
set(BUILD_GTEST ON)
add_subdirectory(external/googletest)
 
#add glog
set(WITH_GFLAGS OFF)
add_subdirectory(external/glog)

#add lua
add_subdirectory(external/lua)


#add tinyobjloader
add_subdirectory(external/tinyobjloader)

#add embree3
# set(EMBREE_ISPC_SUPPORT OFF)
# set(EMBREE_TUTORIALS OFF)
# set(EMBREE_TASKING_SYSTEM "NO") #TODO change to TBB
# add_subdirectory(external/embree)

#add benchmark
SET(BENCHMARK_ENABLE_GTEST_TESTS OFF)
add_subdirectory(external/benchmark)
 
#narukami
add_executable(narukami src/main.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
target_link_libraries(narukami glog tinyobjloader)
# target_link_libraries(narukami embree)

#narukami_test
add_executable(narukami_test test/main.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
target_link_libraries(narukami_test gtest lua5.3.4)

#benchmark 
add_executable(narukami_benchmark benchmark/main.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
target_link_libraries(narukami_benchmark benchmark)

#demos
add_executable(narukami_demo_render_a_soatriangle demo/render_a_soatriangle.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
add_executable(narukami_demo_render_a_mesh demo/render_a_mesh.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
target_link_libraries(narukami_demo_render_a_mesh tinyobjloader)
add_executable(narukami_demo_render_a_texture_space demo/render_a_texture_space.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
add_executable(narukami_demo_render_a_bounds demo/render_a_bounds.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
add_executable(narukami_demo_output_2d_random_number demo/output_2d_random_number.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
add_executable(narukami_demo_output_sampler_data demo/output_sampler_data.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
add_executable(narukami_demo_render_a_sample_distribution demo/render_a_sample_distribution.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
add_executable(narukami_demo_accelerator_test demo/accelerator_test.cpp ${SRC_CORE} ${SRC_CAMERAS} ${EXT_LODEPNG})
target_link_libraries(narukami_demo_accelerator_test tinyobjloader)


if(MSVC)
  if(${MSVC_VERSION} GREATER_EQUAL 1915)
    # You must acknowledge that you understand MSVC resolved a byte alignment issue in this compiler
    # We get this due to using Eigen objects and allocating those objects with make_shared
    target_compile_definitions( narukami_test PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE )
    target_compile_definitions( narukami_demo_render_a_mesh PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE )
    target_compile_definitions( narukami_demo_accelerator_test PRIVATE _ENABLE_EXTENDED_ALIGNED_STORAGE )
  endif()
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
