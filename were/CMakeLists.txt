cmake_minimum_required(VERSION 3.6.0)
project(were)

file(GLOB EVERYTHING
    "*.c"
    "*.cpp"
    "*.h"
)

if(DEFINED ANDROID)
    list(FILTER EVERYTHING EXCLUDE REGEX ".*xcb.*")
    list(FILTER EVERYTHING EXCLUDE REGEX ".*x11.*")
endif(DEFINED ANDROID)

add_library(were STATIC
    ${EVERYTHING}
)

set_target_properties(were PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -O0")
target_compile_options(were PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)
