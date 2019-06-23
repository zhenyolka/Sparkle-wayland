cmake_minimum_required(VERSION 3.14.4)
project(sparkle)

add_subdirectory(were)

file(GLOB EVERYTHING
    "*.cpp"
    "*.h"
    "generated/*.cpp"
    "generated/*.h"
)

add_executable(sparkle
    ${EVERYTHING}
)

target_include_directories(sparkle PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/were
)

set_target_properties(sparkle PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")

target_link_libraries(sparkle LINK_PUBLIC
    were
    wayland-server
    X11
)
