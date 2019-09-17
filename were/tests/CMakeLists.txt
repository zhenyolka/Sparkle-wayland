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
