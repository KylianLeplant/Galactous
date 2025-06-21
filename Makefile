.DEFAULT_GOAL := all

# Configuration du compilateur
CXX = g++
INCLUDES = -Iinclude -Ivendor/glad
CXXFLAGS = -g #-static

# Dossiers du projet
TARGET_DIR = ./target
SRC_DIR = ./src
MAIN_DIR = ./src/main

# Variables pour les fichiers
EXECUTABLE = $(TARGET_DIR)/main
MAIN_SOURCE = $(MAIN_DIR)/main.cpp
SOURCES = $(SRC_DIR)/types.cpp $(SRC_DIR)/Octree.cpp $(SRC_DIR)/Particle.cpp $(SRC_DIR)/Galaxy.cpp
MAIN_OBJECT = $(TARGET_DIR)/main.o
OBJECTS = $(TARGET_DIR)/types.o $(TARGET_DIR)/Octree.o $(TARGET_DIR)/Particle.o $(TARGET_DIR)/Galaxy.o $(TARGET_DIR)/glad.o


# Liens pour GLFW et OpenGL
LDFLAGS = -lglfw3 -lopengl32

# Règle principale
all: clean test

# Compilation de l'exécutable final
$(EXECUTABLE): $(MAIN_OBJECT) $(OBJECTS)
	$(CXX) $(INCLUDES) $(MAIN_OBJECT) $(OBJECTS) -o $(EXECUTABLE) $(CXXFLAGS) $(LDFLAGS)

# Compilation du fichier main
$(MAIN_OBJECT): $(MAIN_SOURCE)
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(MAIN_SOURCE) -o $(MAIN_OBJECT) $(CXXFLAGS)

# Compilation des autres fichiers source
$(TARGET_DIR)/types.o: $(SRC_DIR)/types.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/types.cpp -o $(TARGET_DIR)/types.o $(CXXFLAGS)

$(TARGET_DIR)/Octree.o: $(SRC_DIR)/Octree.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/Octree.cpp -o $(TARGET_DIR)/Octree.o $(CXXFLAGS)

$(TARGET_DIR)/Particle.o: $(SRC_DIR)/Particle.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/Particle.cpp -o $(TARGET_DIR)/Particle.o $(CXXFLAGS)

$(TARGET_DIR)/Galaxy.o: $(SRC_DIR)/Galaxy.cpp
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c $(INCLUDES) $(SRC_DIR)/Galaxy.cpp -o $(TARGET_DIR)/Galaxy.o $(CXXFLAGS)

$(TARGET_DIR)/glad.o: vendor/glad/glad.c
	@mkdir -p $(TARGET_DIR)
	$(CXX) -c -Ivendor/glad vendor/glad/glad.c -o $(TARGET_DIR)/glad.o $(CXXFLAGS)

# Règles d'exécution
run: $(EXECUTABLE)
	$(EXECUTABLE)

test: $(EXECUTABLE)
	$(EXECUTABLE)

# Nettoyage
clean: 
	rm -rf $(TARGET_DIR)






