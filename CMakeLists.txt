cmake_minimum_required(VERSION 3.10)
project(Galactous)

set(CMAKE_CXX_STANDARD 20)

# Ajoute les dossiers d'include
include_directories(
    include
    OpenCL
    include/display
    include/simulation
    external/imgui
    external/imgui/backends
    external/vendor/glad
    
)

# Ajoute les fichiers source principaux
file(GLOB SOURCES
    src/*.cpp
    src/display/*.cpp
    src/simulation/*.cpp
    src/main/main.cpp
    external/vendor/glad/glad.c
    external/imgui/*.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
add_executable(Galactous ${SOURCES})

find_package(glfw3 REQUIRED)
target_link_libraries(Galactous PRIVATE glfw opengl32 OpenCL)


# Ajoute les chemins vers les libs si elles ne sont pas dans le PATH
#link_directories(
#    external/GLEW/lib
#    external/SFML/lib
#)