cmake_minimum_required(VERSION 3.31)
project(cpp_renderer)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE sources src/**.cpp src/**.h)

add_executable(cpp_renderer ${sources})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} Vulkan::Vulkan glm::glm)
