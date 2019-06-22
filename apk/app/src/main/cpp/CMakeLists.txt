cmake_minimum_required(VERSION 3.6.0)

#set(SPARKLE_ROOT "../../../../../")
get_filename_component(SPARKLE_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../../../../" ABSOLUTE)


add_library(sparkle SHARED
    dummy.cpp
)

set_target_properties(sparkle PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")

