project(livetest CXX)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

include_directories(${ath_SOURCE_DIR})
include_directories(${livetest_SOURCE_DIR})


file(GLOB SOURCE
    "src/*.cpp"
    "*.hpp"
)

add_executable(livetest ${SOURCE})
target_link_libraries(livetest ath)

