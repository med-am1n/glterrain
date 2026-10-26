cmake_minimum_required(VERSION 3.10)
project(Opengl-init)

# for nvim do: ln -s build/compile_commands.json compile_commands.json after build!!
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

configure_file(
    ${CMAKE_SOURCE_DIR}/includes/root_directory.h.in
    ${CMAKE_BINARY_DIR}/generated/root_directory.h
)
find_package(glfw3 REQUIRED)

find_package(assimp REQUIRED)

set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# GLad version configuration
set(GLAD_VERSION "41" CACHE STRING "GLAD version")

set(GLAD_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/glad${GLAD_VERSION}")
set(GLAD_INCLUDE_DIR "${GLAD_ROOT}/include")
set(GLAD_SOURCE "${GLAD_ROOT}/src/glad.c")

if(NOT EXISTS "${GLAD_INCLUDE_DIR}/glad/glad.h")
    message(FATAL_ERROR
        "GLAD version ${GLAD_VERSION} not found at ${GLAD_ROOT}"
    )
endif()

if(NOT EXISTS "${GLAD_SOURCE}")
    message(FATAL_ERROR
        "GLAD source not found: ${GLAD_SOURCE}"
    )
endif()

add_executable(app
    src/main.cpp
    src/stb_image.cpp
    ${GLAD_SOURCE}
    ${IMGUI_SOURCES}
)

target_include_directories(app PRIVATE
    ${GLAD_INCLUDE_DIR}
    external/glm
    external/imgui
    includes
    ${CMAKE_BINARY_DIR}/generated   
)

target_link_libraries(app
    glfw
    assimp::assimp
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)
