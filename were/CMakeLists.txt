cmake_minimum_required(VERSION 3.14.4)
project(were)

file(GLOB EVERYTHING
    "*.cpp"
)

add_library(were STATIC
    ${EVERYTHING}
)

set_target_properties(were PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")
