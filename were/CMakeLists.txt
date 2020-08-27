cmake_minimum_required(VERSION 3.6.0)
project(were)

file(GLOB EVERYTHING
    "*.c"
    "*.cpp"
    "*.h"
    "math/*.cpp"
    "math/*.h"
)

if(DEFINED ANDROID)
    list(FILTER EVERYTHING EXCLUDE REGEX ".*xcb.*")
    list(FILTER EVERYTHING EXCLUDE REGEX ".*x11.*")
endif(DEFINED ANDROID)

add_library(were STATIC
    ${EVERYTHING}
)

target_include_directories(were PUBLIC
    math
)

set_target_properties(were PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -O2")
target_compile_options(were PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
