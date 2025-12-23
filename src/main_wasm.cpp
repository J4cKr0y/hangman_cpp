#include <emscripten/emscripten.h>
#include <string>
#include <vector>
#include <set>
#include <algorithm> 

#include "hangman/hangman.h"

// Variable globale pour stocker l'instance du jeu
// Cela simplifie l'interaction avec JS
static Hangman* hangmanGame = nullptr;

extern "C" { 

// Initialise un nouveau jeu
// Le mot secret est passé en paramètre depuis JS
EMSCRIPTEN_KEEPALIVE
void initGame(const char* secretWordCStr) {
    if (hangmanGame) {
        delete hangmanGame; // Nettoyer l'ancien jeu si il y en a un
    }
    std::string secretWord(secretWordCStr);
    hangmanGame = new Hangman(secretWord);
}

// Devine une lettre
// Retourne 1 si la lettre est trouvée, 0 sinon
EMSCRIPTEN_KEEPALIVE
int guessLetter(char letter) {
    if (!hangmanGame) return 0; // Pas de jeu en cours
    return hangmanGame->guessLetter(letter) ? 1 : 0;
}

// Obtient le mot affiché (avec les underscores)
EMSCRIPTEN_KEEPALIVE
const char* getCurrentDisplayWord() {
    if (!hangmanGame) return "";
    // On utilise une variable static pour que la mémoire persiste 
    static std::string displayCache; 
    displayCache = hangmanGame->getCurrentDisplayWord(); 
    return displayCache.c_str(); 
}

// Obtient le nombre de tentatives restantes
EMSCRIPTEN_KEEPALIVE
int getRemainingAttempts() {
    if (!hangmanGame) return 0;
    return hangmanGame->getRemainingAttempts();
}

// Vérifie si le jeu est terminé
// Retourne 1 si oui, 0 sinon
EMSCRIPTEN_KEEPALIVE
int isGameOver() {
    if (!hangmanGame) return 0;
    return hangmanGame->isGameOver() ? 1 : 0;
}

// Vérifie si le mot a été deviné
// Retourne 1 si oui, 0 sinon
EMSCRIPTEN_KEEPALIVE
int isWordGuessed() {
    if (!hangmanGame) return 0;
    return hangmanGame->isWordGuessed() ? 1 : 0;
}

// Obtient le mot secret (utile pour l'affichage en fin de partie)
EMSCRIPTEN_KEEPALIVE
const char* getSecretWord() { 
    if (!hangmanGame) return ""; 
    static std::string secretCache; 
    secretCache = hangmanGame->getSecretWord(); 
    return secretCache.c_str(); 
}

// Pour récupérer la liste des lettres devinées, c'est un peu plus complexe
// On va retourner une chaîne de caractères
EMSCRIPTEN_KEEPALIVE
const char* getGuessedLettersString() {
    if (!hangmanGame) return "";
    static std::string lettersStr; // Utiliser static pour éviter la fuite mémoire
    lettersStr.clear();
    for (char c : hangmanGame->getGuessedLetters()) {
        lettersStr += c;
        lettersStr += ' ';
    }
    return lettersStr.c_str();
}

// Fonction de nettoyage (appelée quand le module WASM est déchargé)
EMSCRIPTEN_KEEPALIVE
void cleanup() {
    if (hangmanGame) {
        delete hangmanGame;
        hangmanGame = nullptr;
    }
}

} // extern "C"
