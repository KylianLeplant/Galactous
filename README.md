# Galactous

Simulation de galaxie 3D utilisant l'algorithme de **Barnes-Hut**, accélérée par **OpenCL** (calcul GPU) et **multithreading** (CPU). Affichage OpenGL temps réel via GLFW + ImGui.

## Fonctionnalités

- **Barnes-Hut** : approximation O(n log n) des interactions gravitationnelles via un octree. Critère d'ouverture `theta` configurable (rapport largeur/distance).
- **OpenCL** : calcul des accélérations sur GPU via un kernel de compute shader (`shaders/compute/computeShader.cl`).
- **Multithreading** : un thread rafraîchit continuellement l'octree aplati (`updateFlattenedOctree`), un autre consomme les données via `std::atomic<std::shared_ptr<...>>` pour un accès sans mutex.
- **Affichage OpenGL 3.3** + **ImGui** pour le HUD (FPS, contrôles fenêtre).
- Génération de galaxies disques paramétrable (nombre d'étoiles, masse, rayon, épaisseur, vitesse de rotation).

## Architecture

```
include/
  simulation/
    Simulation.hpp     boucle principale, threads, atomic shared_ptr
    Octree.hpp          arbre 8-fils Barnes-Hut
    FlattenedOctree.hpp version aplatie (tableaux SoA pour OpenCL)
    ParticlesData.hpp   données particules envoyées au GPU (pos/mass/vel)
    Galaxy.hpp          collection de particules
    GalaxyFactory.hpp   générateur de disque en rotation
    Particle.hpp        particule (pos, vel, acc, type)
  display/
    Window.hpp         GLFW + ImGui init
    Camera.hpp         caméra orbitale (souris + ZSQD)
    Page.hpp           boucle d'affichage + thread simulation
    PointRenderer.hpp   renderer OpenGL de points
    Mat4.hpp            matrices 4x4 (lookAt/perspective)
  ComputeShader.hpp     wrapper OpenCL (init, buffers, launch)
  Input.hpp             callbacks GLFW chaînés avec ImGui
src/                    implémentation
shaders/compute/computeShader.cl   kernel OpenCL Barnes-Hut
```

## Dépendances

- **MinGW-w64 / g++** (testé avec MSYS2 GCC 14.2.0, C++20)
- **GLFW 3**
- **OpenGL 3.3**
- **OpenCL 3.0** (header `external/CL/opencl.hpp` fourni)
- **GLAD** (fourni dans `external/vendor/glad`)
- **Dear ImGui** (fourni dans `external/imgui`)

## Compilation

Le projet utilise un **Makefile** (pas de CMake). Compilateur recommandé : MinGW64.

```bash
make            # build target/main.exe (clean partiel puis build)
make run        # compile et lance
make clean      # nettoie les .o et l'exécutable (garde ImGui/GLAD)
make rebuild    # clean complet + rebuild
```

## Utilisation

Lancer `target/main.exe`. Une galaxie de 3000 étoiles (par défaut) est générée et simulée sur le GPU.

| Contrôle | Action |
|----------|--------|
| Z / S    | Avancer / reculer la caméra |
| Q / D    | Gauche / droite |
| Souris + clic gauche | Rotation orbitale autour de la cible |
| Molette  | Zoom |
| Échap    | Quitter |

## Fonctionnement

1. La `Page` démarre un thread qui lance `Simulation::run(true)` (GPU) ou `run(false)` (CPU).
2. `updateParticlesData()` sérialise les particules des galaxies dans un `ParticlesData` (positions/masses/vitesses), exposé via `std::atomic<ParticlesDataPtr>`.
3. `updateFlattenedOctree()` reconstruit en continu l'octree aplati (SoA) et l'expose via `std::atomic<FlattenedOctreePtr>`.
4. `updateWithGPU()` consomme ces deux données, envoie les buffers OpenCL, lance le kernel `accelerations`, lit les positions/vitesses modifiées, et réécrit sur les particules des galaxies.
5. Le thread principal effectue le rendu OpenGL indépendamment.

## Paramètres de simulation

Dans `include/simulation/Simulation.hpp` :

- `time_step` : pas de temps d'intégration
- `G` : constante gravitationnelle (`6.67430e-11`)
- `softening` : adoucissement pour éviter les singularités
- `theta` : critère Barnes-Hut (plus petit = plus précis, plus lent)

## Roadmap (voir `Cahier des charges.txt`)

- GUI avancée (paramètres en direct, multi-galaxies)
- Évènements astrophysiques (formation stellaire, supernovae, fusions)
- Objets compacts (trous noirs, étoiles à neutrons)
- Matière noire / gaz / poussière comme types de particules distincts

## Auteur

Keima ([KylianLeplant](https://github.com/KylianLeplant))