# Partie 2 : Débogage du projet avec GDB

## 1. Introduction au débogage et importance des symboles de débogage

Le débogage constitue une étape cruciale dans le cycle de développement logiciel, permettant d'identifier et de corriger les erreurs qui surviennent inévitablement lors de l'écriture de code. 
Pour le projet HangmanGame, la configuration CMake étudiée précédemment prépare minutieusement le terrain pour une expérience de débogage efficace grâce à `-g` et `-O0`. 
Ces options de compilation transforment un binaire opaque en un programme transparent et inspectable, créant un pont entre le code source de haut niveau et les instructions machine de bas niveau.

`-g` ordonne au compilateur de générer des informations de débogage au format DWARF (Debug With Attributed Record Format) sur les systèmes Unix-like. 
Ces métadonnées incluent la correspondance entre les adresses mémoire et les numéros de ligne du code source, les noms et types des variables, ainsi que la structure des fonctions. 
Sans ces informations, un débogueur ne pourrait afficher que des adresses hexadécimales cryptiques et des registres processeur, rendant l'analyse du comportement du programme extrêmement laborieuse.

`-O0`, quant à lui, garantit que le compilateur n'applique aucune optimisation au code. 
Cette absence d'optimisation est fondamentale pour le débogage car elle préserve une correspondance directe et prévisible entre chaque ligne de code source et les instructions assembleur générées. 
Les optimisations, bien qu'essentielles pour les performances en production, peuvent réorganiser, fusionner ou éliminer des instructions, rendant le flux d'exécution difficile à suivre lors d'une session de débogage.

## 2. Configuration de l'environnement de débogage

### 2.1 Compilation en mode Debug

Avant d'utiliser GDB, il est essentiel de s'assurer que le projet est compilé en mode Debug. La configuration CMake du projet facilite cette tâche :

```
# Configuration en mode Debug explicite
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Compilation du projet
make -j4

# L'exécutable avec symboles de débogage se trouve dans bin/
./bin/hangman_exec
```

La présence des symboles de débogage peut être vérifiée avec la commande `file` :

```
file bin/hangman_exec
# Sortie attendue : ELF 64-bit LSB executable... with debug_info, not stripped
```

### 2.2 Lancement de GDB

Pour débuguer, il suffit ensuite de lancer GDB :

```
gdb ./bin/hangman_exec
```

## 3. Techniques de débogage appliquées au projet Hangman

### 3.1 Points d'arrêt (Breakpoints)

Les points d'arrêt sont l'outil fondamental du débogage. Dans le contexte du jeu Hangman, ils permettent d'examiner le comportement du programme à des moments critiques :

```
# Point d'arrêt au début de la fonction main
(gdb) break main

# Point d'arrêt dans la méthode guessLetter de la classe Hangman
(gdb) break Hangman::guessLetter

# Point d'arrêt à une ligne spécifique de main.cpp
(gdb) break main.cpp:145

# Point d'arrêt conditionnel : s'arrêter quand il reste 1 tentative
(gdb) break Hangman::getRemainingAttempts if m_remainingAttempts == 1

# Lister tous les points d'arrêt
(gdb) info breakpoints

# Désactiver/réactiver un point d'arrêt
(gdb) disable 2
(gdb) enable 2

# Supprimer un point d'arrêt
(gdb) delete 2
```

### 3.2 Navigation dans le code

Une fois le programme arrêté à un point d'arrêt, plusieurs commandes permettent de naviguer dans l'exécution :

```
# Démarrer l'exécution du programme
(gdb) run

# Cas spécifique : lancer en mode solo (option 1)
(gdb) run < <(echo "1")

# Continuer l'exécution après un arrêt
(gdb) continue

# Exécuter la ligne suivante (sans entrer dans les fonctions)
(gdb) next

# Exécuter la ligne suivante (en entrant dans les fonctions)
(gdb) step

# Sortir de la fonction courante
(gdb) finish

# Exécuter jusqu'à une ligne spécifique
(gdb) until 150
```

### 3.3 Inspection des variables et de l'état du programme

L'inspection des variables permet de comprendre l'état du programme. 
Dans le contexte du jeu Hangman :

```
# Afficher la valeur d'une variable simple
(gdb) print secretWord

# Afficher une variable membre de la classe Hangman
(gdb) print game.m_remainingAttempts

# Afficher le contenu d'une std::string
(gdb) print secretWord.c_str()

# Afficher un std::set (lettres devinées)
(gdb) print game.m_guessedLetters

# Surveiller une variable (affichage automatique à chaque arrêt)
(gdb) display game.m_remainingAttempts

# Modifier une variable pendant le débogage
(gdb) set var letter = 'A'
```

### 3.4 Analyse de la pile d'appels

La pile d'appels révèle le chemin d'exécution qui a mené au point actuel :

```
# Afficher la pile d'appels complète
(gdb) backtrace

# Afficher les 5 premières frames
(gdb) bt 5

# Se déplacer dans la pile d'appels
(gdb) frame 2   # Aller à la frame 2
(gdb) up        # Monter d'une frame
(gdb) down      # Descendre d'une frame

# Afficher les variables locales de la frame courante
(gdb) info locals

# Afficher les arguments de la fonction courante
(gdb) info args
```


### 3.5 Débogage de la lecture du fichier dictionnaire

Un problème courant pourrait être l'échec de la lecture du fichier `dico.txt`. 
Voici comment déboguer ce scénario :

```
# Point d'arrêt au début de chooseRandomWord
(gdb) break chooseRandomWord

# Point d'arrêt sur la vérification de l'ouverture du fichier
(gdb) break main.cpp:16

(gdb) run
# Le programme s'arrête au point d'arrêt

# Vérifier le nom du fichier passé
(gdb) 1
# Devrait afficher "../data/dico.txt"

# Avancer jusqu'à la vérification
(gdb) next

# Si le fichier n'apparait pas, vérifier le répertoire de travail
(gdb) shell pwd
(gdb) shell ls ../data/
```

## 4. Conclusion 

Le débogage efficace du projet Hangman repose sur une utilisation méthodique de GDB combinée aux drapeaux de compilation appropriés. 
La configuration CMake du projet, avec sa distinction claire entre les modes Debug et Release, facilite grandement ce processus. 
Les drapeaux `-g` et `-O0` transforment GDB d'un simple outil en un environnement d'investigation complet, permettant non seulement de corriger les bugs mais aussi de mieux comprendre le comportement interne du programme.
Cette maîtrise du débogage est complémentaire à la structure modulaire du projet étudiée dans la partie 1 et prépare naturellement à l'exploration des bibliothèques qui sera abordée dans la partie 3, 
où nous verrons comment la séparation entre `hangman_lib` et `hangman_exec` facilite non seulement la compilation mais aussi le débogage ciblé de composants spécifiques.
