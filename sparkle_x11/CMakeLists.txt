cmake_minimum_required(VERSION 3.6.0)
project(sparkle_x11)

set(CLANG_TIDY_CHECKS "*,\
-llvm-header-guard,\
-llvm-include-order,\
-readability-*,\
-modernize-use-trailing-return-type,\
-cppcoreguidelines-avoid-magic-numbers,\
-fuchsia-*,\
-cppcoreguidelines-special-member-functions,\
-hicpp-special-member-functions,\
-google-*,\
-modernize-*,\
-misc-non-private-member-variables-in-classes,\
-cppcoreguidelines-non-private-member-variables-in-classes,\
-hicpp-use-equals-default,\
-cppcoreguidelines-avoid-goto,\
-google-*,\
-hicpp-braces-around-statements,\
-cppcoreguidelines-pro-type-vararg,\
-cppcoreguidelines-macro-usage,\
-hicpp-vararg,\
-hicpp-uppercase-literal-suffix,\
-cppcoreguidelines-pro-type-const-cast,\
-cppcoreguidelines-pro-type-reinterpret-cast,\
-hicpp-avoid-goto,\
-cppcoreguidelines-no-malloc,\
-hicpp-no-malloc,\
-cppcoreguidelines-avoid-c-arrays,\
-hicpp-avoid-c-arrays,\
-performance-*,\
-cppcoreguidelines-pro-bounds-array-to-pointer-decay,\
-hicpp-no-array-decay,\
-cppcoreguidelines-owning-memory,\
-cppcoreguidelines-pro-bounds-pointer-arithmetic,\
-cert-dcl50-cpp,\
-android-*,\
-hicpp-use-auto,\
-cppcoreguidelines-pro-bounds-constant-array-index,\
-misc-unused-parameters,\
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
target_compile_options(sparkle PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -Wsuggest-override>)

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
