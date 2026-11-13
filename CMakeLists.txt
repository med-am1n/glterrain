cmake_minimum_required(VERSION 3.10)
project(glterrain)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

add_executable(app
    src/main.cpp
    src/stb_image.cpp
    external/glad/src/glad.c
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp 
)

configure_file(
    ${CMAKE_SOURCE_DIR}/includes/root_directory.h.in
    ${CMAKE_BINARY_DIR}/generated/root_directory.h
)

target_include_directories(app PRIVATE
    external/glad/include
    external/glm
    external/imgui
    includes
    ${CMAKE_BINARY_DIR}/generated   
)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

target_link_libraries(app
    glfw
    assimp::assimp
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)

