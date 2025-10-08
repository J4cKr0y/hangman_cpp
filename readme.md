# Hangman Game

## Projet
------
Implémentation complète du jeu du pendu (Hangman) en C++17, avec une architecture modulaire séparant logique métier et interface utilisateur, gérée par CMake pour une compilation portable.


## C'est quoi ?
------------
Le Pendu est un jeu de lettres classique où le joueur doit deviner un mot mystère lettre par lettre. À chaque erreur, le dessin d'un pendu se complète progressivement. L'objectif est de trouver le mot complet avant que le pendu ne soit entièrement dessiné. C'est un jeu qui allie déduction, vocabulaire et stratégie.


## Technologies Utilisées
-----------------------
- **Langage**: C++17
- **Build System**: CMake 3.10+
- **Compilateurs**: GCC, Clang, MSVC
- **Débogage**: GDB/LLDB

![C++](https://img.shields.io/badge/C++-17-blue)
![CMake](https://img.shields.io/badge/CMake-3.10+-green)
![Compiler-GCC](https://img.shields.io/badge/Compiler-GCC-orange)


## Fonctionnalités Clés
---------------------
- **Architecture Modulaire**: Séparation claire entre bibliothèque (hangman_lib) et exécutable (hangman_exec)
- **Compilation Configurable**: Modes Debug (avec symboles -g et sans optimisation -O0) et Release (optimisé -O3)
- **Dictionnaire Externe**: Mots chargés depuis un fichier texte (dico.txt) facilement modifiable
- **Interface Console Interactive**: Affichage en temps réel du mot partiellement découvert et des tentatives restantes
- **Gestion Robuste des Erreurs**: Validation des entrées utilisateur et gestion des cas limites


## Architecture du Projet
------------------------
HangmanGame/
├── CMakeLists.txt              # Configuration CMake principale

├── src/

│   ├── main.cpp                # Point d'entrée du programme

│   └── hangman/

│       ├── CMakeLists.txt      # Configuration de la bibliothèque

│       └── hangman.cpp         # Implémentation de la logique métier

├── include/

│   └── hangman/

│       └── hangman.h           # Interface publique de la classe Hangman

├── data/

│   └── dico.txt                # Dictionnaire de mots

└── build/                      # Répertoire de compilation (généré)

    └── bin/

        ├── hangman_exec        # Exécutable final

        └── dico.txt            # Dictionnaire copié automatiquement


Cette architecture suit les principes de **séparation des responsabilités** :

- hangman_lib : bibliothèque statique contenant toute la logique du jeu (indépendante de l'interface)
- hangman_exec : exécutable léger gérant uniquement l'interaction utilisateur
- CMake orchestre les dépendances avec les mots-clés PRIVATE/PUBLIC/INTERFACE


## Par quoi commencer ? 
--------------------
Si vous voulez juste jouer au jeu, une version en ligne est ici : 
https://j4ckr0y.github.io/hangman_cpp


Sinon, vous pouvez le compiler vous même.

**Prérequis**

- Un compilateur C++17 (GCC 7+, Clang 5+, ou MSVC 2017+)
- CMake 3.10 ou supérieur
- GDB ou LLDB pour le débogage (optionnel)

**Compilation et exécution**
1. Cloner le dépôt
```
git clone https://github.com/votre-username/hangman-game.git
cd hangman-game
```
2. Configuration en mode Release (recommandé pour jouer)
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```
3. Lancer le jeu
```
./build/bin/hangman_exec
```

## Documentation Complète

Le projet est accompagné d'une analyse technique approfondie en 3 parties :

- Partie 1 : Système de compilation CMake, configuration Debug/Release, structure modulaire
- Partie 2 : Techniques de débogage avec GDB, analyse de la pile d'appels, inspection de variables
- Partie 3 : Architecture des bibliothèques (statiques vs dynamiques), mécanismes de linking, portée des dépendances

Consulter la documentation complète dans le dossier Rapport/


## Roadmap et Améliorations Prévues
----------------------------------
- [ ] Mode multijoueur en réseau (serveur/client TCP)
- [ ] Bibliothèque dynamique avec système de plugins pour des dictionnaires thématiques
- [ ] Interface graphique avec Qt6 
- [ ] Tests unitaires avec Google Test 
- [ ] Support de l'internationalisation (i18n) 
- [ ] CI/CD avec GitHub Actions (compilation multi-plateforme)

### Contribuer
Les contributions sont les bienvenues ! Pour contribuer :

1. Forkez le projet
2. Créez une branche feature (git checkout -b feature/amelioration)
3. Committez vos changements (git commit -m 'Ajout d'une fonctionnalité')
4. Pushez vers la branche (git push origin feature/amelioration)
5. Ouvrez une Pull Request

**Conventions de code** : Suivre le style C++ moderne (RAII, smart pointers, const-correctness)
