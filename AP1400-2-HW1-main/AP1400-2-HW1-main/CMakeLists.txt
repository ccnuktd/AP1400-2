cmake_minimum_required(VERSION 3.13)
project(AP)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)

add_executable(Matrix_run ${SOURCE_FILES})

include_directories(Matrix_lib)
add_subdirectory(Matrix_lib)


target_link_libraries(Matrix_run Matrix_lib)
add_subdirectory(Google_tests)