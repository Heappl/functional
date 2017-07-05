cmake_minimum_required (VERSION 2.8)
project (functional)

set(dir ${PROJECT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(CMAKE_CXX_FLAGS "--std=c++11")

include_directories(${PROJECT_SOURCE_DIR}/googletest/googlemock/include)
include_directories(${PROJECT_SOURCE_DIR}/googletest/googletest/include)
include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory (googletest)

add_executable(
    uts
    test/apply.cpp
    test/curry.cpp
    test/compose.cpp
)
target_link_libraries(uts gtest gtest_main)
