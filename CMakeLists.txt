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
    external/GLEW/include
    external/GLM
    external/ImGui
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
file(GLOB_RECURSE HEADERS include/*.hpp include/*.h)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
add_executable(Galactous ${SOURCES} ${HEADERS})



add_link_options(-lmingw32 -lgdi32 -luser32 -lkernel32 -ladvapi32 -lshell32 -lgcc -lmoldname -lmingwex -lmsvcrt -lwinspool -lcomdlg32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32)
#add_subdirectory(external)
#target_link_libraries(Galactous PUBLIC external)
# Dimension3D
target_include_directories(Galactous PUBLIC external/Dimension3D/includes )
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(SFML_PATH external/SFML/MinGW/x64)
	else()
		set(SFML_PATH external/SFML/MinGW/x86)
	endif()
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5.1 COMPONENTS REQUIRED system window graphics PATHS ${SFML_PATH})
target_link_libraries(${PROJECT_NAME} PUBLIC sfml-system sfml-window sfml-graphics)
# GLM
target_include_directories(${PROJECT_NAME} PUBLIC libraries/GLM)
add_definitions(-DGLEW_STATIC)




find_package(glfw3 REQUIRED)
target_link_libraries(Galactous PRIVATE glfw opengl32 OpenCL)


# Ajoute les chemins vers les libs si elles ne sont pas dans le PATH
#link_directories(
#    external/GLEW/lib
#    external/SFML/lib
#)