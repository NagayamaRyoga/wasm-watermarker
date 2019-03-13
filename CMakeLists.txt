cmake_minimum_required(VERSION 3.0.0)

project(wasm-watermarker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Werror -pedantic -fconcepts")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -Og")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG")

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_BUILD_TYPE)

include(ExternalProject)
include(cmake/binaryen.cmake)
include(cmake/fmt.cmake)

include_directories(
    src
)

add_subdirectory(src)
add_subdirectory(test)
