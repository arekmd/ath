project(tests CXX)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

include_directories(${ath_SOURCE_DIR})
include_directories(${ath_SOURCE_DIR}/src)
include_directories(${tests_SOURCE_DIR})

file(GLOB SOURCE
    "*.cpp"
    "*.hpp"
)

add_executable(tests ${SOURCE})
target_link_libraries(tests gtest pthread ath)
