cmake_minimum_required(VERSION 3.31)
project(PalmTree)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(vendor/imgui)

file(GLOB SOURCES src/**.cpp src/**.h)

add_executable(PalmTree ${SOURCES})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} Vulkan::Vulkan glm::glm imgui)
