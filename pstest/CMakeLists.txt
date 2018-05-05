project(pstest CXX)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

include_directories(${ath_SOURCE_DIR})
include_directories(${pstest_SOURCE_DIR})


file(GLOB SOURCE
    "src/*.cpp"
    "*.hpp"
)

add_executable(pstest ${SOURCE})
target_link_libraries(pstest ath)

