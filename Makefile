.DEFAULT_GOAL := all

# Configuration du compilateur
CXX = g++
INCLUDES = -Iinclude -Iexternal/vendor/glad -Iexternal/imgui -Iexternal/imgui/backends -Iinclude/display -Iinclude/simulation -Iexternal -IOpenCL
CXXFLAGS = -g -DIMGUI_IMPL_OPENGL_LOADER_GLAD -std=c++20
IMGUI_CXXFLAGS = -g -DIMGUI_IMPL_OPENGL_LOADER_GLAD

# Dossiers du projet
TARGET_DIR = ./target
SRC_DIR = ./src
MAIN_DIR = ./src/main
EXTERNAL_DIR = ./external
SIM_DIR = ./src/simulation

# Variables pour les fichiers
EXECUTABLE = $(TARGET_DIR)/main
MAIN_SOURCE = $(MAIN_DIR)/main.cpp
SOURCES = $(SRC_DIR)/types.cpp $(SRC_DIR)/Octree.cpp $(SRC_DIR)/Particle.cpp $(SRC_DIR)/Galaxy.cpp $(SRC_DIR)/GalaxyFactory.cpp $(SRC_DIR)/display/Window.cpp $(SRC_DIR)/display/Page.cpp $(SRC_DIR)/display/Input.cpp $(SRC_DIR)/display/PointRenderer.cpp $(SRC_DIR)/display/Camera.cpp $(SRC_DIR)/simulation/Simulation.cpp
MAIN_OBJECT = $(TARGET_DIR)/main.o
OBJECTS = $(TARGET_DIR)/types.o $(TARGET_DIR)/Octree.o $(TARGET_DIR)/Particle.o $(TARGET_DIR)/Galaxy.o $(TARGET_DIR)/GalaxyFactory.o $(TARGET_DIR)/glad.o $(TARGET_DIR)/Window.o $(TARGET_DIR)/Page.o $(TARGET_DIR)/PointRenderer.o $(TARGET_DIR)/Camera.o $(TARGET_DIR)/Simulation.o $(TARGET_DIR)/ComputeShader.o

# Fichiers ImGui
IMGUI_SOURCES = $(EXTERNAL_DIR)/imgui/imgui.cpp $(EXTERNAL_DIR)/imgui/imgui_demo.cpp $(EXTERNAL_DIR)/imgui/imgui_draw.cpp $(EXTERNAL_DIR)/imgui/imgui_tables.cpp $(EXTERNAL_DIR)/imgui/imgui_widgets.cpp $(EXTERNAL_DIR)/imgui/backends/imgui_impl_glfw.cpp $(EXTERNAL_DIR)/imgui/backends/imgui_impl_opengl3.cpp
IMGUI_OBJECTS = $(TARGET_DIR)/imgui.o $(TARGET_DIR)/imgui_demo.o $(TARGET_DIR)/imgui_draw.o $(TARGET_DIR)/imgui_tables.o $(TARGET_DIR)/imgui_widgets.o $(TARGET_DIR)/imgui_impl_glfw.o $(TARGET_DIR)/imgui_impl_opengl3.o

# Liens pour GLFW et OpenGL
LDFLAGS = -lglfw3 -lopengl32

# Règle principale - ne fait plus de clean automatique
all: clean  $(EXECUTABLE)

# Règle pour forcer la recompilation des bibliothèques externes
rebuild-external: clean-external $(EXECUTABLE)

# Règle pour forcer la recompilation complète
rebuild: clean $(EXECUTABLE)

# Compilation de l'exécutable final
$(EXECUTABLE): $(MAIN_OBJECT) $(OBJECTS) $(IMGUI_OBJECTS)
	$(CXX) $(INCLUDES) $(MAIN_OBJECT) $(OBJECTS) $(IMGUI_OBJECTS) -o $(EXECUTABLE) $(CXXFLAGS) $(LDFLAGS) -lOpenCL

# Compilation du fichier main
$(MAIN_OBJECT): $(MAIN_SOURCE)
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(MAIN_SOURCE) -o $(MAIN_OBJECT) $(CXXFLAGS)

# Compilation des autres fichiers source
$(TARGET_DIR)/types.o: $(SRC_DIR)/types.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/types.cpp -o $(TARGET_DIR)/types.o $(CXXFLAGS)

$(TARGET_DIR)/Octree.o: $(SIM_DIR)/Octree.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SIM_DIR)/Octree.cpp -o $(TARGET_DIR)/Octree.o $(CXXFLAGS)

$(TARGET_DIR)/Particle.o: $(SIM_DIR)/Particle.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SIM_DIR)/Particle.cpp -o $(TARGET_DIR)/Particle.o $(CXXFLAGS)

$(TARGET_DIR)/Galaxy.o: $(SIM_DIR)/Galaxy.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SIM_DIR)/Galaxy.cpp -o $(TARGET_DIR)/Galaxy.o $(CXXFLAGS)

$(TARGET_DIR)/GalaxyFactory.o: $(SIM_DIR)/GalaxyFactory.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SIM_DIR)/GalaxyFactory.cpp -o $(TARGET_DIR)/GalaxyFactory.o $(CXXFLAGS)

$(TARGET_DIR)/glad.o: $(EXTERNAL_DIR)/vendor/glad/glad.c
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c -I$(EXTERNAL_DIR)/vendor/glad $(EXTERNAL_DIR)/vendor/glad/glad.c -o $(TARGET_DIR)/glad.o $(CXXFLAGS)

$(TARGET_DIR)/Window.o: $(SRC_DIR)/display/Window.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/display/Window.cpp -o $(TARGET_DIR)/Window.o $(CXXFLAGS)

$(TARGET_DIR)/Page.o: $(SRC_DIR)/display/Page.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/display/Page.cpp -o $(TARGET_DIR)/Page.o $(CXXFLAGS)

$(TARGET_DIR)/PointRenderer.o: $(SRC_DIR)/display/PointRenderer.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/display/PointRenderer.cpp -o $(TARGET_DIR)/PointRenderer.o $(CXXFLAGS)

$(TARGET_DIR)/Camera.o: $(SRC_DIR)/display/Camera.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/display/Camera.cpp -o $(TARGET_DIR)/Camera.o $(CXXFLAGS)

$(TARGET_DIR)/Simulation.o: $(SRC_DIR)/simulation/Simulation.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/simulation/Simulation.cpp -o $(TARGET_DIR)/Simulation.o $(CXXFLAGS)

$(TARGET_DIR)/ComputeShader.o: $(SRC_DIR)/ComputeShader.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/ComputeShader.cpp -o $(TARGET_DIR)/ComputeShader.o $(CXXFLAGS)


# Compilation des fichiers ImGui
$(TARGET_DIR)/imgui.o: $(EXTERNAL_DIR)/imgui/imgui.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(EXTERNAL_DIR)/imgui/imgui.cpp -o $(TARGET_DIR)/imgui.o $(IMGUI_CXXFLAGS)

$(TARGET_DIR)/imgui_demo.o: $(EXTERNAL_DIR)/imgui/imgui_demo.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(EXTERNAL_DIR)/imgui/imgui_demo.cpp -o $(TARGET_DIR)/imgui_demo.o $(IMGUI_CXXFLAGS)

$(TARGET_DIR)/imgui_draw.o: $(EXTERNAL_DIR)/imgui/imgui_draw.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(EXTERNAL_DIR)/imgui/imgui_draw.cpp -o $(TARGET_DIR)/imgui_draw.o $(IMGUI_CXXFLAGS)

$(TARGET_DIR)/imgui_tables.o: $(EXTERNAL_DIR)/imgui/imgui_tables.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(EXTERNAL_DIR)/imgui/imgui_tables.cpp -o $(TARGET_DIR)/imgui_tables.o $(IMGUI_CXXFLAGS)

$(TARGET_DIR)/imgui_widgets.o: $(EXTERNAL_DIR)/imgui/imgui_widgets.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(EXTERNAL_DIR)/imgui/imgui_widgets.cpp -o $(TARGET_DIR)/imgui_widgets.o $(IMGUI_CXXFLAGS)

$(TARGET_DIR)/imgui_impl_glfw.o: $(EXTERNAL_DIR)/imgui/backends/imgui_impl_glfw.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(EXTERNAL_DIR)/imgui/backends/imgui_impl_glfw.cpp -o $(TARGET_DIR)/imgui_impl_glfw.o $(IMGUI_CXXFLAGS)

$(TARGET_DIR)/imgui_impl_opengl3.o: $(EXTERNAL_DIR)/imgui/backends/imgui_impl_opengl3.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(EXTERNAL_DIR)/imgui/backends/imgui_impl_opengl3.cpp -o $(TARGET_DIR)/imgui_impl_opengl3.o $(IMGUI_CXXFLAGS)




# Règles d'exécution
run: $ clear (EXECUTABLE)
	$(EXECUTABLE)

test: $(EXECUTABLE)
	$(EXECUTABLE)

# Nettoyage sélectif (garde GLAD et ImGui)
clean:
	rm -f $(TARGET_DIR)/main.o $(TARGET_DIR)/types.o $(TARGET_DIR)/Octree.o $(TARGET_DIR)/Particle.o $(TARGET_DIR)/Galaxy.o $(TARGET_DIR)/Window.o $(TARGET_DIR)/Page.o $(EXECUTABLE) $(TARGET_DIR)/PointRenderer.o $(TARGET_DIR)/Camera.o

# Nettoyage des bibliothèques externes seulement
clean-external:
	rm -f $(TARGET_DIR)/glad.o $(IMGUI_OBJECTS)

# Nettoyage complet (supprime tout)
clean-all: 
	rm -rf $(TARGET_DIR)

# Aide
help:
	@echo "Commandes disponibles:"
	@echo "  make        - Compile le projet (recompile seulement les fichiers modifiés)"
	@echo "  make run    - Compile et lance le programme"
	@echo "  make test   - Compile et lance le programme"
	@echo "  make rebuild-external - Force la recompilation de GLAD et ImGui"
	@echo "  make rebuild - Force la recompilation complète"
	@echo "  make clean  - Supprime tout SAUF GLAD et ImGui (recompilation rapide)"
	@echo "  make clean-external - Supprime seulement les .o de GLAD et ImGui"
	@echo "  make clean-all - Supprime TOUS les fichiers compilés"
	@echo "  make help   - Affiche cette aide"






