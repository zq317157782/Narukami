cmake_minimum_required(VERSION 3.0.0)
project(narukami VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(src)
include_directories(src/core)
include_directories(external)

#add gtest
set(gtest_force_shared_crt ON)
set(BUILD_GMOCK OFF)
set(BUILD_GTEST ON)
add_subdirectory(external/googletest)

#add glog
set(WITH_GFLAGS OFF)
add_subdirectory(external/glog)

#narukami
add_executable(narukami src/main.cpp)
target_link_libraries(narukami glog)

#narukami_test
add_executable(narukami_test test/main.cpp)
target_link_libraries(narukami_test gtest)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
