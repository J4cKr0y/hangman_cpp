# CMake

## Analyse du système de compilation d'un projet de jeu du pendu en C++ avec CMake

### 1\. Vue d'ensemble du processus de compilation C++ avec CMake

Nous passons d'un projet simple en langage C (une calculatrice), à un projet plus structuré en C++ (jeu du pendu). 
Alors que la compilation d'un programme C basique peut se faire avec une simple ligne de commande, les projets C++ modernes bénéficient grandement d'outils de gestion de build sophistiqués. 
Dans le cas présent, le projet "HangmanGame" utilise CMake, un choix stratégique qui facilite la portabilité et la modularité du code.
Il est essentiel de comprendre que CMake n'est pas un compilateur. Il s'agit plutôt d'un générateur de système de build.
Son rôle est d'abstraire la complexité inhérente à la compilation sur différentes plateformes. Le processus se décompose en deux phases distinctes. 
- La première, la phase de configuration, implique la lecture des fichiers CMakeLists.txt par CMake. 
Durant cette étape, CMake analyse l'arborescence du projet, les dépendances et les options de build pour générer des fichiers de construction natifs. 
- La seconde phase, la phase de build, est l'exécution de ces fichiers de construction natifs par l'outil approprié (comme make), 
qui se charge de la compilation des fichiers source et de l'édition de liens pour produire l'exécutable final.
L'architecture, qui sépare la logique du jeu en une bibliothèque distincte dans un sous-répertoire, est une pratique qui vise à améliorer la modularité et la réutilisabilité du code.

### 2\. Analyse du fichier principal CMakeLists.txt

Le fichier CMakeLists.txt à la racine du projet est le point d'entrée de la configuration du système de build. Il définit les paramètres globaux qui s'appliqueront à l'ensemble du projet.
- La première ligne, `cmake\_minimum\_required(VERSION 3.10)`, impose une version minimale de CMake. 
Cela garantit que la configuration du projet sera compatible avec les fonctionnalités de la version 3.10 et les suivantes.
- La commande suivante, project(HangmanGame), assigne un nom logique au projet.
- Les lignes `set(CMAKE\_CXX\_STANDARD 17\)` et `set(CMAKE\_CXX\_STANDARD\_REQUIRED ON)` indiquent que le projet doit impérativement être compilé en respectant la norme C++17. 
Cette exigence n'est pas un simple détail technique ; elle a des implications majeures pour le développement. 
En spécifiant C++17, le projet peut tirer parti de fonctionnalités modernes du langage telles que les structured bindings, std::string\_view et if constexpr, 
qui conduisent généralement à un code plus clair, plus sûr et plus efficace. 
Cette spécification explicite garantit également la cohérence du projet et empêche toute compilation avec un compilateur obsolète qui ne prendrait pas en charge ces fonctionnalités. 
C'est un choix de conception qui oriente le projet vers les meilleures pratiques de programmation contemporaines.
- Le fichier de configuration gère également la distinction entre les modes de compilation Debug et Release en utilisant des indicateurs spécifiques. 
Le bloc conditionnel `if(CMAKE\_CXX\_COMPILER\_ID MATCHES "GNU|Clang")` s'assure que les drapeaux de compilation sont correctement appliqués aux compilateurs GCC et Clang.
- En mode Debug, la configuration `set(CMAKE\_CXX\_FLAGS\_DEBUG "-g \-Wall \-Wextra \-pedantic \-O0")` est choisie.
\-g est fondamental pour le débogage, car il ordonne au compilateur de générer et insérer dans le fichier compilé les informations utiles pour le débogueur.
Cla permet d'établir une correspondance entre les adresses mémoire dans le binaire compilé et les noms des fonctions, des variables et des lignes de code source. 
Sans ces informations, il serait extrêmement difficile pour un débogueur comme GDB d'inspecter l'état du programme. 
\-O0 désactive toutes les optimisations du compilateur. 
La raison en est que les optimisations, bien qu'elles améliorent les performances en mode de production, peuvent transformer le code de manière significative 
(par exemple, en regroupant des instructions, en supprimant des variables ou en insérant du code de fonction en ligne). 
Ces transformations rendent le débogage pas à pas très difficile, voire impossible, car la relation directe entre une ligne de code source et l'instruction machine correspondante est rompue.
Cela assure une traduction fidèle du code source, permettant une expérience de débogage fiable et prévisible. 
\-Wall, \-Wextra et \-pedantic sont des mesures de qualité de code qui activent un ensemble étendu d'avertissements du compilateur, encourageant la détection précoce d'erreurs et de comportements potentiellement indésirables.
En revanche, le mode Release est optimisé pour les performances avec `set(CMAKE\_CXX\_FLAGS\_RELEASE "-O3 \-DNDEBUG")`.
\-O3 active un niveau d'optimisation agressif pour maximiser la vitesse d'exécution et réduire la taille du binaire.
\-DNDEBUG désactive les vérifications internes (comme assert) pour produire un code plus optimisé.
Enfin, la ligne `configure\_file(${CMAKE\_SOURCE\_DIR}/data/dico.txt ${CMAKE\_BINARY\_DIR}/bin/dico.txt COPYONLY)` est utilisée pour gérer le fichier de dictionnaire.
Au lieu d'une commande de copie rudimentaire, cette approche intègre le fichier dico.txt dans le système de dépendances de CMake. 
Si le fichier source est modifié, CMake le détecte et re-déclenche automatiquement la copie lors du prochain build, garantissant que le dictionnaire utilisé par le programme est toujours à jour. 
Cela évite les erreurs d'exécution potentielles dues à l'utilisation d'un fichier de données obsolète.

### 3\. Structure modulaire du projet

La structure du projet est organisée en modules distincts, comme en témoigne l'utilisation de la commande `add\_subdirectory(src/hangman)`. 
Cette commande est la clé de la modularité du projet, car elle permet de traiter un autre fichier CMakeLists.txt situé dans un sous-répertoire.
Cette approche architecturale sépare la logique du jeu (la bibliothèque) de son point d'entrée (l'exécutable). 
Le fichier CMakeLists.txt principal délègue la responsabilité de la compilation de la bibliothèque au fichier de configuration de son sous-répertoire. 
Cela crée une encapsulation claire : l'exécutable (`hangman\_exec`) n'a pas besoin de connaître les fichiers source de la bibliothèque (`hangman.cpp`), il ne se soucie que de la cible de build finale, hangman\_lib. 
Cette séparation des préoccupations rend le code de la bibliothèque potentiellement réutilisable dans d'autres projets et simplifie la maintenance. 

### 4\. Définition des cibles et liaison

Le processus de compilation se termine par la définition des cibles et leur liaison. Le fichier principal CMakeLists.txt contient deux commandes pour cela.
La commande `add\_executable(hangman\_exec src/main.cpp)` est utilisée pour définir la cible de l'exécutable principal du projet, en spécifiant que sa source est le fichier src/main.cpp.
La liaison de cet exécutable à la bibliothèque de jeu est gérée par la ligne `target\_link\_libraries(hangman\_exec PRIVATE hangman\_lib`). 
Cette commande est un élément fondamental de la gestion des dépendances en C++ avec CMake. Le mot-clé PRIVATE est particulièrement important ici.
Il indique que `hangman\_exec` a besoin de `hangman\_lib` pour son implémentation, mais que la bibliothèque n'est pas exposée comme faisant partie de l'interface publique de `hangman\_exec`. 
En d'autres termes, si une autre cible était liée à `hangman\_exec`, elle n'aurait pas automatiquement accès aux informations de liaison de `hangman\_lib`. 
Le choix de PRIVATE est sémantiquement correct et optimisé pour ce projet, car un exécutable n'est pas conçu pour être lié à d'autres projets.
Le mot-clé PRIVATE communique clairement la nature de cette dépendance et aide CMake à construire un graphe de dépendances efficace pour le processus de build.

### 5\. Tableau récapitulatif

Afin de résumer les différences fondamentales dans les configurations de compilation, le tableau suivant présente les drapeaux utilisés en mode Debug et Release ainsi que leurs rôles et impacts respectifs.

| Drapeau | Mode Debug | Mode Release | Rôle et impact |
| :---- | :---- | :---- | :---- |
| **\-g** | Oui | Non | Génère des informations de débogage pour permettre l'inspection du code source et des variables lors de l'exécution. |
| **\-O0** | Oui | Non | Désactive les optimisations du compilateur pour garantir une correspondance directe entre le code source et le code binaire, facilitant ainsi le débogage pas à pas. |
| **\-O3** | Non | Oui | Active le niveau d'optimisation le plus élevé pour maximiser la performance et réduire la taille du binaire final. |
| **\-Wall \-Wextra \-pedantic** | Oui | Non (en général) | Active un ensemble strict d'avertissements du compilateur pour identifier les erreurs potentielles et les mauvaises pratiques de codage dès la compilation. |
| **\-DNDEBUG** | Non | Oui | Définit une macro qui désactive les assertions (assert) et les sections de code de débogage pour la version de production du programme. |

Voici donc la fin de cette première partie, sur le système de compilation du projet. 
Il utilise CMake pour gérer la complexité de la construction de manière modulaire, tout en distinguant clairement la configuration pour le développement et la production. 
Les choix techniques reflètent une approche moderne du développement C++, axée sur la qualité du code et la réutilisabilité.

### 6\. Préparation pour les parties suivantes

Cette analyse du processus de compilation fournit un socle solide pour les parties suivantes du rapport. 
La partie 2, consacrée au débogage, s'appuiera directement sur l'étude des drapeaux \-g et \-O0. 
Elle expliquera comment ces options permettent une inspection détaillée du programme à l'aide d'outils comme GDB ou LLDB.
La partie 3, sur les bibliothèques, explorera plus en profondeur l'architecture choisie. 
Elle examinera la nature d'une bibliothèque statique, comme hangman\_lib, en la comparant aux bibliothèques dynamiques, 
et expliquera l'importance des mots-clés de portée (PRIVATE, PUBLIC, INTERFACE) dans le contexte de la réutilisation et de la maintenance du code.
