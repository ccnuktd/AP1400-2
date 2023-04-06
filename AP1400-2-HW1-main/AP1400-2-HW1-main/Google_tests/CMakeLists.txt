cmake_minimum_required(VERSION 3.14)
project(Google_tests)
add_subdirectory(lib)
link_libraries(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#
link_libraries(${gmock_SOURCE_DIR}/include ${gmock_SOURCE_DIR})
# adding the Google_Tests_run target
add_executable(Google_Tests_run unit_test.cpp)
add_executable(Google_Mocks_run unit_test.cpp)

# linking Google_Tests_run with Matrix_lib which will be tested
target_link_libraries(Google_Tests_run Matrix_lib)
target_link_libraries(Google_Mocks_run Matrix_lib)

target_link_libraries(Google_Tests_run gtest gtest_main)
target_link_libraries(Google_Mocks_run gmock gmock_main)