cmake_minimum_required(VERSION 3.31)
project(Sandbox)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(../PalmTree ${CMAKE_CURRENT_BINARY_DIR}/PalmTree)

file(GLOB_RECURSE SOURCES src/**.cpp src/**.h)

add_executable(Sandbox ${SOURCES})

target_link_libraries(Sandbox PRIVATE PalmTree)
