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
add_executable(narukami src/main.cpp src/core/constant.cpp src/core/sse.cpp src/core/mesh.cpp)
target_link_libraries(narukami glog tinyobjloader)
# target_link_libraries(narukami embree)

#narukami_test
add_executable(narukami_test test/main.cpp src/core/constant.cpp src/core/sse.cpp src/core/mesh.cpp)
target_link_libraries(narukami_test gtest lua5.3.4)

#benchmark rcp
add_executable(narukami_benchmark benchmark/main.cpp src/core/constant.cpp src/core/sse.cpp)
target_link_libraries(narukami_benchmark benchmark)

#demos
add_executable(narukami_demo_render_a_soatriangle demo/render_a_soatriangle.cpp external/lodepng/lodepng.cpp src/core/sse.cpp src/core/constant.cpp)
add_executable(narukami_demo_render_a_mesh demo/render_a_mesh.cpp external/lodepng/lodepng.cpp src/core/sse.cpp src/core/constant.cpp src/core/mesh.cpp 
               src/core/imageio.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
