#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cctype>
#include "hangman/hangman.h"


std::string chooseRandomWord(const std::string& filename) {
    std::vector<std::string> words;
    std::ifstream file(filename);
    std::string word;

    if (!file.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
        std::cerr << "Utilisation d'un mot par défaut..." << std::endl;
        return "programmation";
    }

    while (std::getline(file, word)) {
        word.erase(0, word.find_first_not_of(" \t\r\n"));
        word.erase(word.find_last_not_of(" \t\r\n") + 1);

        if (!word.empty()) {
            words.push_back(word);
        }
    }

    file.close();

    if (words.empty()) {
        std::cerr << "Erreur: Le fichier ne contient aucun mot valide." << std::endl;
        std::cerr << "Utilisation d'un mot par défaut..." << std::endl;
        return "programmation";
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int randomIndex = std::rand() % words.size();
    return words[randomIndex];
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void displayHangman(int wrongGuesses) {
    std::cout << "\n";
    switch (wrongGuesses) {
        case 0:
            std::cout << "   +----+\n";
            std::cout << "   |    |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "=========\n";
            break;
        case 1:
            std::cout << "   +----+\n";
            std::cout << "   |    |\n";
            std::cout << "   O    |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "=========\n";
            break;
        case 2:
            std::cout << "   +----+\n";
            std::cout << "   |    |\n";
            std::cout << "   O    |\n";
            std::cout << "   |    |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "=========\n";
            break;
        case 3:
            std::cout << "   +----+\n";
            std::cout << "   |    |\n";
            std::cout << "   O    |\n";
            std::cout << "  /|    |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "=========\n";
            break;
        case 4:
            std::cout << "   +----+\n";
            std::cout << "   |    |\n";
            std::cout << "   O    |\n";
            std::cout << "  /|\   |\n";
            std::cout << "        |\n";
            std::cout << "        |\n";
            std::cout << "=========\n";
            break;
        case 5:
            std::cout << "   +----+\n";
            std::cout << "   |    |\n";
            std::cout << "   O    |\n";
            std::cout << "  /|\   |\n";
            std::cout << "  /     |\n";
            std::cout << "        |\n";
            std::cout << "=========\n";
            break;
        case 6:
            std::cout << "   +----+\n";
            std::cout << "   |    |\n";
            std::cout << "   O    |\n";
            std::cout << "  /|\   |\n";
            std::cout << "  / \   |\n";
            std::cout << "        |\n";
            std::cout << "=========\n";
            break;
    }
    std::cout << "\n";
}

void displayGameState(const Hangman& game) {
    int wrongGuesses = 6 - game.getRemainingAttempts();

    displayHangman(wrongGuesses);

    std::cout << "Mot: ";
    std::string displayWord = game.getCurrentDisplayWord();
    for (char c : displayWord) {
        std::cout << c << " ";
    }
    std::cout << "\n\n";

    std::cout << "Tentatives restantes: " << game.getRemainingAttempts() << std::endl;

    auto guessedLetters = game.getGuessedLetters();
    if (!guessedLetters.empty()) {
        std::cout << "Lettres devinées: ";
        for (char letter : guessedLetters) {
            std::cout << letter << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::string(50, '-') << std::endl;
}

int main() {
    std::cout << "=== JEU DU PENDU ===" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    int mode;
    std::cout << "Choisissez le mode de jeu:" << std::endl;
    std::cout << "1. Solo (joueur contre ordinateur)" << std::endl;
    std::cout << "2. Deux joueurs" << std::endl;
    std::cout << "Votre choix: ";
    std::cin >> mode;

    std::string secretWord;

    if (mode == 1) {

        secretWord = chooseRandomWord("../data/dico.txt");
        std::cout << "Un mot a été choisi aléatoirement. Bonne chance !" << std::endl;
    } else if (mode == 2) {

        std::cout << "Joueur 1, entrez le mot secret: ";
        std::cin >> secretWord;
        clearScreen();
        std::cout << "Le mot secret a été défini. Joueur 2, à vous de jouer !" << std::endl;
    } else {
        std::cout << "Mode de jeu invalide." << std::endl;
        return 1;
    }

    if (secretWord.empty()) {
        std::cout << "Erreur: Le mot secret ne peut pas être vide." << std::endl;
        return 1;
    }

    Hangman game(secretWord);
    char letter;

    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Le jeu commence ! Vous avez 6 tentatives." << std::endl;

    while (!game.isGameOver()) {
        displayGameState(game);

        std::cout << "Entrez une lettre: ";
        std::cin >> letter;

        if (!std::isalpha(letter)) {
            std::cout << "Veuillez entrer une lettre valide." << std::endl;
            continue;
        }

        bool letterFound = game.guessLetter(letter);

        std::cout << std::endl;

        if (letterFound) {
            std::cout << "✓ Bonne lettre !" << std::endl;
        } else {
            std::cout << "✗ Lettre incorrecte..." << std::endl;
        }

        std::cout << std::endl;
    }

    displayGameState(game);

    std::cout << std::string(50, '=') << std::endl;

    if (game.hasWon()) {
        std::cout << "\o/ FÉLICITATIONS ! Vous avez trouvé le mot : " << secretWord << std::endl;
    } else {
        std::cout << "X PENDU ! Le mot était : " << secretWord << std::endl;
    }

    std::cout << std::string(50, '=') << std::endl;

    return 0;
}
