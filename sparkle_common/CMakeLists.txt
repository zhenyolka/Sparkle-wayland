cmake_minimum_required(VERSION 3.6.0)
project(sparkle_common)

get_filename_component(SPARKLE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

add_subdirectory(${SPARKLE_ROOT}/were were)

file(GLOB EVERYTHING
    "*.cpp"
    "*.h"
    "generated/*.cpp"
    "generated/*.h"
)

add_library(sparkle_common STATIC
    ${EVERYTHING}
)

target_include_directories(sparkle_common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SPARKLE_ROOT}/were
)

set_target_properties(sparkle_common PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -O0")
target_compile_options(sparkle_common PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

target_link_libraries(sparkle_common LINK_PUBLIC
    were
)
