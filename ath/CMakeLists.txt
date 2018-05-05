project(ath C)

cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 14)

include_directories("${ath_SOURCE_DIR}")


file(GLOB SOURCE
    "src/*.c"
)

file(GLOB HEADERS
    "*.h"
    "src/*.h"
)

add_library(ath SHARED ${SOURCE} ${HEADERS})

install(TARGETS ath LIBRARY DESTINATION lib)
install(FILES "${ath_SOURCE_DIR}/*.h" DESTINATION "include/ath")
