cmake_minimum_required(VERSION 3.15)
project(Galactous VERSION 1.0.0 LANGUAGES CXX)

# Configuration C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options de build
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/target)

# Répertoires d'inclusion
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/simulation
    ${PROJECT_SOURCE_DIR}/external
)

# Recherche des dépendances
find_package(Threads REQUIRED)

# RayLib
find_package(raylib QUIET)
if(NOT raylib_FOUND)
    message(STATUS "raylib not found via CMake, searching manually...")
    find_library(RAYLIB_LIBRARY NAMES raylib PATHS /mingw64/lib)
    find_path(RAYLIB_INCLUDE_DIR raylib.h PATHS /mingw64/include)
    
    if(RAYLIB_LIBRARY AND RAYLIB_INCLUDE_DIR)
        message(STATUS "Found raylib: ${RAYLIB_LIBRARY}")
        include_directories(${RAYLIB_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "raylib not found!")
    endif()
endif()

# OpenCL
find_package(OpenCL QUIET)
if(NOT OpenCL_FOUND)
    message(STATUS "OpenCL not found via CMake, searching manually...")
    find_library(OPENCL_LIBRARY NAMES OpenCL PATHS /mingw64/lib)
    find_path(OPENCL_INCLUDE_DIR CL/opencl.hpp PATHS /mingw64/include)
    
    if(OPENCL_LIBRARY AND OPENCL_INCLUDE_DIR)
        message(STATUS "Found OpenCL: ${OPENCL_LIBRARY}")
        include_directories(${OPENCL_INCLUDE_DIR})
    else()
        message(FATAL_ERROR "OpenCL not found!")
    endif()
else()
    include_directories(${OpenCL_INCLUDE_DIRS})
endif()

# Collecte des fichiers sources
set(SOURCES
    src/types.cpp
    src/ComputeShader.cpp
    src/simulation/Simulation.cpp
    src/simulation/Particle.cpp
    src/simulation/Octree.cpp
    src/simulation/GalaxyFactory.cpp
    src/simulation/Galaxy.cpp
    src/main/main.cpp
)

# Création de l'exécutable
add_executable(main ${SOURCES})

# Liens des bibliothèques
target_link_libraries(main PRIVATE
    Threads::Threads
)

# Lien RayLib
if(raylib_FOUND)
    target_link_libraries(main PRIVATE raylib)
else()
    target_link_libraries(main PRIVATE ${RAYLIB_LIBRARY})
endif()

# Lien OpenCL
if(OpenCL_FOUND)
    target_link_libraries(main PRIVATE OpenCL::OpenCL)
else()
    target_link_libraries(main PRIVATE ${OPENCL_LIBRARY})
endif()

# Bibliothèques Windows spécifiques
if(WIN32)
    target_link_libraries(main PRIVATE
        gdi32
        winmm
        opengl32
    )
endif()

# Options de compilation
target_compile_options(main PRIVATE
    -Wall
    -Wextra
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O3>
)

# Copier les shaders dans le répertoire de build
file(COPY ${PROJECT_SOURCE_DIR}/shaders 
     DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Message de fin
message(STATUS "Configuration terminée!")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "  Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
