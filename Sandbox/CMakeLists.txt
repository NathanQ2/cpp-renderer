cmake_minimum_required(VERSION 3.31)
project(Sandbox)

set(CMAKE_CXX_STANDARD 23)

if(NOT TARGET PalmTree)
    add_subdirectory(../PalmTree ${CMAKE_CURRENT_BINARY_DIR}/../PalmTree)
endif()

if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

file(GLOB_RECURSE SOURCES src/**.cpp src/**.h)

add_executable(Sandbox ${SOURCES})

target_link_libraries(Sandbox PRIVATE PalmTree)
