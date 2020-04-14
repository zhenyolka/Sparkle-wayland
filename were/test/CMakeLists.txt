cmake_minimum_required(VERSION 3.6.0)
project(test)

add_definitions(-DX_DEBUG)

get_filename_component(WERE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

add_subdirectory(${WERE_ROOT} were)


file(GLOB EVERYTHING
    "*.cpp"
    "*.h"
)

add_executable(test
    ${EVERYTHING}
)

target_include_directories(test PUBLIC
    ${WERE_ROOT}
)

set_target_properties(test PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -O2")
target_compile_options(test PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

target_link_libraries(test LINK_PUBLIC
    were
)
