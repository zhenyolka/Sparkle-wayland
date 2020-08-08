cmake_minimum_required(VERSION 3.6.0)
project(sparkle_x11)

set(CLANG_TIDY_CHECKS "*,\
-llvm-header-guard,\
-llvm-include-order,\
-cppcoreguidelines-avoid-magic-numbers,\
-cppcoreguidelines-special-member-functions,\
-cppcoreguidelines-pro-bounds-constant-array-index,\
-cppcoreguidelines-non-private-member-variables-in-classes,\
-cppcoreguidelines-avoid-goto,\
-cppcoreguidelines-pro-type-vararg,\
-cppcoreguidelines-macro-usage,\
-cppcoreguidelines-pro-type-const-cast,\
-cppcoreguidelines-pro-type-reinterpret-cast,\
-cppcoreguidelines-pro-bounds-array-to-pointer-decay,\
-cppcoreguidelines-owning-memory,\
-cppcoreguidelines-pro-bounds-pointer-arithmetic,\
-hicpp-special-member-functions,\
-hicpp-braces-around-statements,\
-hicpp-use-equals-default,\
-hicpp-vararg,\
-hicpp-uppercase-literal-suffix,\
-hicpp-avoid-goto,\
-hicpp-no-array-decay,\
-hicpp-use-auto,\
-performance-unnecessary-value-param,\
-cert-dcl50-cpp,\
-misc-unused-parameters,\
-misc-non-private-member-variables-in-classes,\
-fuchsia-*,\
-google-*,\
-modernize-*,\
-android-*,\
-readability-*,\
")

set(CMAKE_CXX_CLANG_TIDY
    clang-tidy;
    -header-filter=.*;
    -checks=${CLANG_TIDY_CHECKS};
    -warnings-as-errors=*;
)

add_definitions(-DX_DEBUG)

get_filename_component(SPARKLE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

#add_subdirectory(${SPARKLE_ROOT}/were were)
add_subdirectory(${SPARKLE_ROOT}/sparkle_common sparkle_common)

file(GLOB EVERYTHING
    "*.cpp"
    "*.h"
)

add_executable(sparkle
    ${EVERYTHING}
)

target_include_directories(sparkle PUBLIC
    #${CMAKE_CURRENT_SOURCE_DIR}
    ${SPARKLE_ROOT}/were
    ${SPARKLE_ROOT}/sparkle_common
)

set_target_properties(sparkle PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -O2")
target_compile_options(sparkle PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>)

target_link_libraries(sparkle LINK_PUBLIC
    were
    sparkle_common
    wayland-server
    xcb
    xcb-shm
    pthread
    dl
    -rdynamic
)
