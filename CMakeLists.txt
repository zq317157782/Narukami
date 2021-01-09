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
include_directories(external/lua/src)
include_directories(external/tinyobjloader)
include_directories(external/glfw/include)
include_directories(external/tinyply/source)
include_directories(external/cyCodeBase)
include_directories(external/rgb2spec)
#####################################################################################################################
#所有的CPP的路径 
aux_source_directory(src/core SRC_CORE)
aux_source_directory(src/cameras SRC_CAMERAS)
aux_source_directory(src/materials SRC_MATERIALS)
aux_source_directory(src/third/lodepng THIRD_LODEPNG)
set(SRC_ALL ${SRC_CORE} ${SRC_MATERIALS} ${SRC_CAMERAS}  ${THIRD_LODEPNG})
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
add_subdirectory(external/rgb2spec)
#add benchmark
SET(BENCHMARK_ENABLE_GTEST_TESTS OFF)
add_subdirectory(external/benchmark)
#openGL
find_package(OPENGL REQUIRED)

set(LINK_TARGET_ALL rgb2spec tinyobjloader tinyply)

#narukami
add_executable(narukami src/main.cpp ${SRC_ALL})
target_link_libraries(narukami ${LINK_TARGET_ALL})
# target_link_libraries(narukami embree)
#####################################################################################################################
#tools
add_executable(blas_bvh_view src/tools/blas_bvh_view.cpp ${SRC_ALL})
target_link_libraries(blas_bvh_view glfw ${LINK_TARGET_ALL} ${OPENGL_LIBRARIES}) 

#narukami_test
add_executable(narukami_test test/main.cpp ${SRC_ALL})
target_link_libraries(narukami_test gtest ${LINK_TARGET_ALL})

#benchmark 
add_executable(narukami_benchmark benchmark/main.cpp ${SRC_ALL})
target_link_libraries(narukami_benchmark benchmark ${LINK_TARGET_ALL})

#samples
add_executable(sample_render_a_mesh sample/render_a_mesh.cpp ${SRC_ALL})
target_link_libraries(sample_render_a_mesh ${LINK_TARGET_ALL})
add_custom_command(TARGET sample_render_a_mesh POST_BUILD        # Adds a post-build event to sample_render_a_mesh
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/src/core/srgb.coeff"      # <--this is in-file
        $<TARGET_FILE_DIR:sample_render_a_mesh>)                 # <--this is out-file path

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
 