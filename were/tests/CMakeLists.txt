cmake_minimum_required(VERSION 3.6.0)
project(tests)


add_subdirectory(.. ./were)

include_directories(..)


add_executable(server server.cpp)
set_target_properties(server PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")
target_link_libraries(server LINK_PUBLIC
    were
    pthread
)

add_executable(client client.cpp)
set_target_properties(client PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")
target_link_libraries(client LINK_PUBLIC
    were
    pthread
)

if(NOT DEFINED ANDROID)

add_executable(surface surface.cpp)
set_target_properties(surface PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11")
target_link_libraries(surface LINK_PUBLIC
    were
    pthread
    xcb
    xcb-shm
    dl
    -rdynamic
)

endif(NOT DEFINED ANDROID)

add_executable(pbo pbo.cpp)
set_target_properties(pbo PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -std=c++11 -O2")
if(DEFINED ANDROID)
target_link_libraries(pbo LINK_PUBLIC
    were
    dl
    EGL
    GLESv3
    -static-libstdc++
)
else(DEFINED ANDROID)
target_link_libraries(pbo LINK_PUBLIC
    were
    pthread
    dl
    xcb
    xcb-shm
    EGL
    GLESv2
)
endif(DEFINED ANDROID)
