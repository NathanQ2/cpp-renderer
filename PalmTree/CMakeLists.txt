cmake_minimum_required(VERSION 3.31)
project(PalmTree)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)

# For spdlog on windows
if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

file(GLOB_RECURSE SOURCES src/**.cpp src/**.h)

# ImGui
file(GLOB IMGUI_SOURCES
        vendor/imgui/*.cpp
        vendor/imgui/*.h
        vendor/imgui/backends/imgui_impl_glfw.cpp
        vendor/imgui/backends/imgui_impl_glfw.h
        vendor/imgui/backends/imgui_impl_vulkan.cpp
        vendor/imgui/backends/imgui_impl_vulkan.h
)

# ImPlot
file(GLOB IMPLOT_SOURCES
        vendor/implot/implot.h
        vendor/implot/implot_internal.h
        vendor/implot/implot.cpp
        vendor/implot/implot_items.cpp
        vendor/implot/implot_demo.cpp
)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(PalmTree STATIC ${SOURCES} ${IMGUI_SOURCES} ${IMPLOT_SOURCES})

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

target_link_libraries(PalmTree glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)

target_include_directories(PalmTree PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>)
target_include_directories(PalmTree PRIVATE vendor/tinyobjloader)
target_include_directories(PalmTree PUBLIC vendor/spdlog/include)
target_include_directories(PalmTree PUBLIC vendor/imgui/ vendor/imgui/backends)
target_include_directories(PalmTree PUBLIC vendor/implot)
target_include_directories(PalmTree PUBLIC vendor/glm)

target_compile_definitions(PalmTree PUBLIC $<$<CONFIG:Debug>:PT_ENABLE_ASSERTS>)
target_compile_definitions(PalmTree PUBLIC $<$<CONFIG:Debug>:PT_DEBUG>)
target_compile_definitions(PalmTree PUBLIC $<$<CONFIG:Release>:PT_RELEASE>)
target_compile_definitions(PalmTree PUBLIC VK_ENABLE_BETA_EXTENSIONS)
target_compile_definitions(PalmTree PUBLIC IMGUI_USER_CONFIG="PalmTree/ImGui/ImGuiUserConfig.h")
target_compile_definitions(PalmTree PUBLIC GLM_ENABLE_EXPERIMENTAL)

target_precompile_headers(PalmTree PUBLIC src/ptpch.h)

file(GLOB shaders shaders/*.vert shaders/*.frag)

foreach (SHADER IN LISTS shaders)
    get_filename_component(FILENAME ${SHADER} NAME)
    add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${SHADER} -o ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv
            DEPENDS ${SHADER}
            COMMENT "Compiling Shader: ${FILENAME}"
    )
    list(APPEND SPV_SHADERS ${CMAKE_CURRENT_BINARY_DIR}/${FILENAME}.spv)
endforeach ()

add_custom_target(CompileShaders ALL DEPENDS ${SPV_SHADERS})
add_dependencies(PalmTree CompileShaders)
