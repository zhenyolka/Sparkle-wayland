cmake_minimum_required(VERSION 3.6.0)
project(were)

file(GLOB EVERYTHING
    "*.cpp"
    "*.h"
)

add_library(were STATIC
    ${EVERYTHING}
)

set_target_properties(were PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")
