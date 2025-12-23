# Partie 3 : Architecture des bibliothèques et modularité du projet

## 1. Introduction : La philosophie de la séparation des responsabilités

L'architecture du projet HangmanGame illustre un principe fondamental du génie logiciel moderne : 
la séparation entre la logique métier (encapsulée dans `hangman_lib`) et l'interface utilisateur (implémentée dans `hangman_exec`). 
Cette organisation n'est pas une simple convention stylistique mais une décision architecturale aux implications profondes pour la maintenabilité, la testabilité et la réutilisabilité du code.

La création d'une bibliothèque dédiée pour la logique du jeu transforme le code d'un monolithe en un système modulaire. 
La classe `Hangman` devient ainsi un composant autonome, indépendant de son mode d'utilisation. 
Cette indépendance signifie qu'elle pourrait théoriquement être intégrée dans une interface graphique Qt, une application web via WebAssembly, ou même un service REST, sans modification de son code source. 
Cette flexibilité architecturale est le fruit d'une conception orientée bibliothèque dès l'origine du projet.

L'utilisation de CMake pour orchestrer cette architecture renforce cette modularité en formalisant les dépendances et les interfaces entre composants. 
Chaque module possède son propre `CMakeLists.txt`, définissant ses responsabilités et ses dépendances de manière explicite et documentée.

## 2. Anatomie d'une bibliothèque statique

### 2.1 Processus de création de hangman_lib

La bibliothèque `hangman_lib` est définie dans le fichier `src/hangman/CMakeLists.txt` (non montré mais implicite dans la structure). Le processus de création suit ces étapes :

```
# src/hangman/CMakeLists.txt 
add_library(hangman_lib STATIC 
    hangman.cpp
)

target_include_directories(hangman_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
)
```

Lors de la compilation, le processus se décompose ainsi :

1. **Compilation en fichiers objets** : Le compilateur transforme `hangman.cpp` en `hangman.cpp.o`, un fichier objet contenant le code machine mais avec des symboles non résolus.

2. **Archivage** : L'outil `ar` (archiver) regroupe les fichiers objets dans une archive statique `libhangman_lib.a` sur Linux/macOS ou `hangman_lib.lib` sur Windows.

3. **Indexation** : L'outil `ranlib` (ou `ar -s`) crée un index des symboles dans l'archive pour accélérer la résolution des symboles lors de l'édition de liens.

### 2.2 Structure interne d'une bibliothèque statique

Une bibliothèque statique est essentiellement une archive de fichiers objets. Sa structure peut être examinée :

```
# Examiner le contenu de la bibliothèque
ar -t libhangman_lib.a
# Sortie : hangman.cpp.o

# Lister les symboles exportés (démanglé)
nm -C libhangman_lib.a
# Sortie :
# 0000000000000146 T Hangman::guessLetter(char)
# 0000000000000000 T Hangman::Hangman(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&)
# ...
```

### 2.3 Mécanisme d'édition de liens statique

Lorsque `hangman_exec` est lié à `hangman_lib`, le processus d'édition de liens (linking) suit ces étapes :

1. **Résolution des symboles** : Le linker parcourt `main.cpp.o` et identifie tous les symboles non définis (comme `Hangman::Hangman`, `Hangman::guessLetter`).

2. **Extraction sélective** : Seuls les fichiers objets de la bibliothèque contenant les symboles nécessaires sont extraits et intégrés dans l'exécutable final.

3. **Intégration monolithique** : Le code de la bibliothèque devient partie intégrante de l'exécutable, créant un binaire autonome.

Cette intégration peut être visualisée :
```
# Vérifier les symboles dans l'exécutable final
nm hangman_exec | grep Hangman
# Les symboles de Hangman sont maintenant intégrés (T = Text section)
```

## 3. Bibliothèques statiques vs dynamiques : analyse comparative

### 3.1 Architecture alternative avec bibliothèque dynamique

Si le projet utilisait une bibliothèque dynamique, la configuration CMake serait :

```
# Version bibliothèque dynamique 
add_library(hangman_lib SHARED 
    hangman.cpp
)

set_target_properties(hangman_lib PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    PUBLIC_HEADER "include/hangman/hangman.h"
)

# Installation de la bibliothèque
install(TARGETS hangman_lib
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/hangman
)
```

### 3.2 Mécanismes internes des bibliothèques dynamiques

#### 3.2.1 Chargement dynamique et édition de liens à l'exécution

Contrairement aux bibliothèques statiques intégrées à la compilation, les bibliothèques dynamiques sont chargées et liées au moment de l'exécution par le chargeur dynamique 
(`ld.so` sur Linux, `dyld` sur macOS, le loader Windows sur Windows).

Le processus de chargement suit ces étapes :

1. **Lancement du programme** : Le système d'exploitation lit l'en-tête de l'exécutable et détecte les dépendances dynamiques (section `.dynamic` dans l'ELF)

2. **Résolution des chemins** : Le chargeur dynamique recherche les bibliothèques dans un ordre précis :

Sur Linux, l'ordre de recherche est :
1. Chemins encodés dans RPATH (compilés dans le binaire)
2. Variable d'environnement LD_LIBRARY_PATH
3. Chemins dans RUNPATH
4. Chemins système (/lib, /usr/lib, /etc/ld.so.conf)

Chargement en mémoire : Les bibliothèques sont mappées dans l'espace d'adressage du processus via mmap()
Résolution des symboles : Les références non résolues sont liées aux symboles des bibliothèques via la GOT (Global Offset Table) et la PLT (Procedure Linkage Table)

#### 3.2.2 Position Independent Code (PIC)
Les bibliothèques dynamiques doivent être compilées avec l'option -fPIC (Position Independent Code), permettant leur chargement à n'importe quelle adresse mémoire :
```
# CMake active automatiquement PIC pour les bibliothèques SHARED
add_library(hangman_lib SHARED hangman.cpp)
# Équivalent à : g++ -fPIC -c hangman.cpp
```
Le PIC a un coût en performance (utilisation de registres supplémentaires, indirections), mais permet :

- Le partage de la même page mémoire entre plusieurs processus (économie de RAM)
- L'ASLR (Address Space Layout Randomization) pour la sécurité
- Le chargement à différentes adresses selon les besoins

#### 3.2.3 Gestion du versioning (soname)
Les bibliothèques dynamiques utilisent un système de versioning sophistiqué pour gérer la compatibilité :
```
set_target_properties(hangman_lib PROPERTIES
    VERSION 1.2.3        # Version complète
    SOVERSION 1          # Version d'interface ABI
)
```
Cela crée une structure de liens symboliques :
```
libhangman_lib.so.1.2.3  # Fichier réel
libhangman_lib.so.1      # Lien symbolique (soname) → .so.1.2.3
libhangman_lib.so        # Lien de développement → .so.1
```
**Règles de versioning** :

Incrémenter *VERSION* (1.2.3 -> 1.2.4) : corrections de bugs, pas de changement d'API
Incrémenter *SOVERSION* (1 -> 2) : changement incompatible de l'ABI (Application Binary Interface)


#### 3.2.4 Lazy binding et PLT/GOT
Par défaut, les bibliothèques utilisent le "lazy binding" : les fonctions ne sont résolues qu'à leur premier appel.
Mécanisme PLT/GOT :
1. Programme appelle fonction_lib()
2. Saut vers PLT (Procedure Linkage Table) - code stub
3. PLT consulte GOT (Global Offset Table)
4. Si premier appel : GOT pointe vers le résolveur dynamique
5. Résolveur trouve l'adresse réelle, met à jour GOT
6. Appels suivants : PLT → GOT → fonction directement

Pour désactiver le lazy binding (sécurité ou performance prédictible) :
```
target_link_options(hangman_exec PRIVATE "LINKER:-z,now")
# Ou : export LD_BIND_NOW=1
```

### 3.3 Comparaison détaillée

| Aspect | Bibliothèque Statique | Bibliothèque Dynamique |
|--------|----------------------|------------------------|
| **Extension** | `.a` (Unix), `.lib` (Windows) | `.so` (Linux), `.dylib` (macOS), `.dll` (Windows) |
| **Taille exécutable** | Plus grande (code intégré) | Plus petite (références externes) |
| **Dépendances runtime** | Aucune (autonome) | Requiert la présence de la bibliothèque |
| **Mémoire** | Duplication si plusieurs programmes | Partage entre processus |
| **Mise à jour** | Recompilation nécessaire | Remplacement de la bibliothèque suffit |
| **Performance** | Légèrement meilleure (pas d'indirection) | Coût minimal de l'indirection PLT/GOT |
| **Sécurité** | Moins vulnérable aux attaques DLL | Risque de DLL hijacking |
| **Distribution** | Simple (un seul fichier) | Complexe (gestion des versions) |
| **Optimisation LTO** | Possible sur tout le code | Limitée aux frontières de la bibliothèque |

PS: LTO permet au compilateur d’optimiser le code non pas fichier par fichier, mais à l’échelle du programme entier, au moment du linkage. 
Supprimer du code inutile, Fusionner des fonctions, Faire de l’inlining entre modules, Réduire la taille du binaire et Améliorer les performances.

### 3.4 Justification du choix statique pour Hangman

Pour le projet Hangman, le choix d'une bibliothèque statique est pertinent pour plusieurs raisons :

1. **Simplicité de déploiement** : Un seul exécutable à distribuer
2. **Performance optimale** : Le compilateur peut optimiser à travers les frontières de la bibliothèque
3. **Taille modeste** : La bibliothèque est petite (~8 KB), l'overhead est négligeable
4. **Pas de versioning complexe** : Évite les problèmes de compatibilité ABI
5. **Utilisation unique** : La bibliothèque n'est utilisée que par un seul exécutable

## 4. Les mots-clés de portée dans CMake : PRIVATE, PUBLIC, INTERFACE

### 4.1 Signification et impact des mots-clés

Les mots-clés de portée dans CMake définissent la propagation des propriétés de compilation :

```
target_link_libraries(target
    PRIVATE lib1    # Utilisé uniquement pour compiler 'target'
    PUBLIC lib2     # Utilisé pour 'target' ET propagé aux utilisateurs
    INTERFACE lib3  # Non utilisé par 'target', mais propagé aux utilisateurs
)

target_include_directories(target
    PRIVATE include/private    # Headers internes
    PUBLIC include/public      # Headers de l'API publique
    INTERFACE include/interface # Headers pour les utilisateurs seulement
)
```

### 4.2 Application au projet Hangman

Dans le `CMakeLists.txt` principal :

```
target_link_libraries(hangman_exec PRIVATE hangman_lib)
```

Le choix de `PRIVATE` est optimal car :
- `hangman_exec` est un exécutable final, pas une bibliothèque
- Aucune autre cible ne dépendra de `hangman_exec`
- Les dépendances de `hangman_lib` ne doivent pas "fuiter"

Si `hangman_lib` était elle-même liée à d'autres bibliothèques :

```
# Dans src/hangman/CMakeLists.txt 
target_link_libraries(hangman_lib
    PRIVATE fmt::fmt        # Formatage interne seulement
    PUBLIC Boost::algorithm # Utilisé dans les headers publics
    INTERFACE pthread       # Requis par les utilisateurs mais pas par la lib
)
```

### 4.3 Graphe de dépendances et propagation

Visualisation de la propagation des dépendances :

```
hangman_exec
    |
    +-- PRIVATE --> hangman_lib
                        |
                        +-- PUBLIC --> include/hangman/
                        |
                        +-- PRIVATE --> src/hangman/*.cpp
```
Si hangman_lib était PUBLIC dans target_link_libraries :
```
other_target
    |
    +-- PRIVATE --> hangman_exec (ERREUR : un exe ne peut pas être lié)
``` 
Mais si nous avions une bibliothèque intermédiaire :
```
game_ui_lib
    |
    +-- PUBLIC --> hangman_lib
    
app_exec
    |
    +-- PRIVATE --> game_ui_lib
                    (hérite automatiquement de hangman_lib via PUBLIC)
```


## 5. Vers une architecture évolutive

L'architecture en bibliothèque du projet Hangman, bien que simple en apparence, pose les fondations d'un système extensible et maintenable. 
Les points clés à retenir sont :

1. **La bibliothèque statique** est un choix pragmatique pour ce projet, offrant simplicité et performance
2. **Les mots-clés PRIVATE, PUBLIC, INTERFACE** ne sont pas de simples annotations mais définissent le contrat d'interface de la bibliothèque
3. **La modularité** permet l'évolution indépendante des composants
4. **CMake** fournit les outils pour gérer cette complexité de manière portable
5. **La testabilité** est grandement améliorée par la séparation en bibliothèque
6. **L'évolutivité** vers des architectures plus complexes (plugins, microservices) est facilitée

Cette architecture, bien que développée pour un simple jeu du pendu, illustre les principes qui permettent aux grands logiciels de perdurer : 
modularité, séparation des responsabilités et anticipation du changement.

### 5.1 CMake : Plus qu'un générateur de Makefile
#### 5.1.1 Rappel : La compilation traditionnelle avec Make
Avant CMake, la gestion d'un projet multi-fichiers nécessitait l'écriture manuelle de Makefiles.
Limitations de cette approche :
- Non-portable : Différent sur Windows (nmake), macOS, Linux
- Chemins en dur : /usr/include, /usr/lib ne marchent pas partout
- Dépendances manuelles : Si on oublie une dépendance, la recompilation échoue
- Compilateurs multiples : Impossible de supporter GCC, Clang, MSVC facilement
- Configuration complexe : Les flags varient selon Debug/Release
- Pas de gestion de bibliothèques tierces : Trouver Boost, Qt, etc. est artisanal

#### 5.1.2 L'approche CMake : Un méta-système de build
CMake se positionne comme un générateur de systèmes de build, pas un système de build direct.
```
CMakeLists.txt (déclaratif)
        ↓
    [CMake]
        ↓
    ┌───────────────┐
    │   Génération  │
    └───────────────┘
         ↓       ↓         ↓
      Makefile  Ninja  Visual Studio
         ↓       ↓         ↓
       make    ninja     msbuild
         ↓       ↓         ↓
           Binaires finaux
```
**Avantages fondamentaux** : 

1. Abstraction du compilateur : CMake détecte et configure automatiquement GCC, Clang, MSVC
2. Portabilité intrinsèque : Le même CMakeLists.txt fonctionne sur Windows, Linux, macOS
3. Gestion des dépendances : Modules find_package() pour trouver les bibliothèques
4. Configurations multiples : Debug, Release, RelWithDebInfo gérées nativement
5. Build hors-source : Séparation propre entre sources et artefacts de compilation

#### 5.1.3 Le processus de configuration CMake
CMake fonctionne en deux phases distinctes :

- Phase 1 : Configuration
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```
Lors de cette phase, CMake :
1. Parse tous les CMakeLists.txt
2. Détecte le compilateur et ses capacités
3. Recherche les dépendances (bibliothèques tierces)
4. Génère le cache (CMakeCache.txt) avec toutes les variables
5. Produit les fichiers de build (Makefile, projets IDE, etc.)

- Phase 2 : Compilation
```
cmake --build build --config Release -j8
```
Cette commande invoque le système de build sous-jacent (make, ninja, msbuild) de manière unifiée.

#### 5.1.4 CMake et les bibliothèques : Une symbiose parfaite
Pour le projet Hangman, CMake apporte une valeur inestimable dans la gestion des bibliothèques :
```
# Définition claire des composants
add_library(hangman_lib STATIC hangman.cpp)
add_executable(hangman_exec main.cpp)

# Propagation intelligente des dépendances
target_link_libraries(hangman_exec PRIVATE hangman_lib)

# Gestion des chemins d'inclusion
target_include_directories(hangman_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
```
Ce que CMake fait automatiquement ici :
- Détermine l'ordre de compilation (lib avant exe)
- Propage les chemins d'inclusion de hangman_lib vers hangman_exec
- Ajoute les flags nécessaires (-fPIC pour les .so, /MD pour MSVC, etc.)
- Génère les commandes d'archivage (ar, lib.exe) selon la plateforme
- Gère les dépendances transitives si hangman_lib dépendait d'autres libs

#### 5.1.5 CMake comme langage de contrat
Au-delà de la génération de build, CMake formalise le contrat d'interface des composants.
C'est cette formalisation qui permet :
- L'intégration facile dans d'autres projets (via find_package())
- La documentation implicite des dépendances
- La détection précoce d'incompatibilités (version C++, bibliothèques manquantes)

### 5.2 Conclusion : L'architecture moderne, un triptyque indissociable
L'architecture du projet Hangman illustre trois piliers du développement C++ moderne, qui forment un tout cohérent :


1. La modularité par les bibliothèques
La séparation hangman_lib / hangman_exec n'est pas qu'organisationnelle : elle crée des frontières contractuelles qui forcent une conception propre. 
La logique métier, isolée dans sa bibliothèque, devient testable, réutilisable et évolutive indépendamment de son interface utilisateur.


2. CMake comme chef d'orchestre
CMake transcende son rôle de simple outil de build pour devenir le langage de description de l'architecture du projet. 
Il encode les dépendances, définit les interfaces (PRIVATE/PUBLIC/INTERFACE), assure la portabilité et automatise la complexité. 
Sans CMake, cette architecture modulaire nécessiterait des centaines de lignes de Makefiles spécifiques à chaque plateforme.


3. La séparation des responsabilités
Chaque composant a un rôle clair : hangman_lib encapsule la logique, hangman_exec gère l'interaction, et CMake orchestre leur collaboration. 
Cette séparation n'est pas théorique : elle se traduit concrètement dans la structure des fichiers, les commandes de build, et la capacité à faire évoluer chaque partie indépendamment.
