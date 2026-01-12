.DEFAULT_GOAL := all

# Configuration du compilateur
CXX = g++
INCLUDES = -Iinclude -Iinclude/simulation -IOpenCL -I/mingw64/include
CXXFLAGS = -g -std=c++20
IMGUI_CXXFLAGS = -g 

# Dossiers du projet
TARGET_DIR = ./target
SRC_DIR = ./src
MAIN_DIR = ./src/main
SIM_DIR = ./src/simulation

# Variables pour les fichiers
EXECUTABLE = $(TARGET_DIR)/main
MAIN_SOURCE = $(MAIN_DIR)/main.cpp
SOURCES = $(SRC_DIR)/types.cpp $(SRC_DIR)/Octree.cpp $(SRC_DIR)/Particle.cpp $(SRC_DIR)/Galaxy.cpp $(SRC_DIR)/GalaxyFactory.cpp $(SRC_DIR)/simulation/Simulation.cpp
MAIN_OBJECT = $(TARGET_DIR)/main.o
OBJECTS = $(TARGET_DIR)/types.o $(TARGET_DIR)/Octree.o $(TARGET_DIR)/Particle.o $(TARGET_DIR)/Galaxy.o $(TARGET_DIR)/GalaxyFactory.o $(TARGET_DIR)/Simulation.o $(TARGET_DIR)/ComputeShader.o


# Liens pour OpenGL
LDFLAGS = -lopengl32 -lOpenCL -lraylib -lgdi32 -lwinmm

# Règle principale - ne fait plus de clean automatique
all: clean  $(EXECUTABLE)

# Règle pour forcer la recompilation des bibliothèques externes
rebuild-external: clean-external $(EXECUTABLE)

# Règle pour forcer la recompilation complète
rebuild: clean $(EXECUTABLE)

# Compilation de l'exécutable final
$(EXECUTABLE): $(MAIN_OBJECT) $(OBJECTS)
	$(CXX) $(INCLUDES) $(MAIN_OBJECT) $(OBJECTS) -o $(EXECUTABLE) $(CXXFLAGS) $(LDFLAGS) -lOpenCL

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

$(TARGET_DIR)/Simulation.o: $(SRC_DIR)/simulation/Simulation.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/simulation/Simulation.cpp -o $(TARGET_DIR)/Simulation.o $(CXXFLAGS)

$(TARGET_DIR)/ComputeShader.o: $(SRC_DIR)/ComputeShader.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/ComputeShader.cpp -o $(TARGET_DIR)/ComputeShader.o $(CXXFLAGS)




# Règles d'exécution
run: $ clear (EXECUTABLE)
	$(EXECUTABLE)

test: $(EXECUTABLE)
	$(EXECUTABLE)

# Nettoyage sélectif (garde GLAD et ImGui)
clean:
	rm -f $(TARGET_DIR)/main.o $(TARGET_DIR)/types.o $(TARGET_DIR)/Octree.o $(TARGET_DIR)/Particle.o $(TARGET_DIR)/Galaxy.o $(TARGET_DIR)/GalaxyFactory.o $(TARGET_DIR)/Simulation.o $(EXECUTABLE) $(TARGET_DIR)/ComputeShader.o


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






