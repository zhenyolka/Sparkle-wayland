cmake_minimum_required(VERSION 3.14.4)
project(test1)

add_subdirectory(.. ./were)

file(GLOB EVERYTHING
    "*.cpp"
    "*.h"
)

add_executable(test1
    ${EVERYTHING}
)

target_include_directories(test1 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(test1 PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")

target_link_libraries(test1 LINK_PUBLIC
    were
)
