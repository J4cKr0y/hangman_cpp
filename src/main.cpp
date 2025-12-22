#include <ncurses.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib> 
#include <ctime>   
#include <limits>  
#include <set>     

#include "hangman/hangman.h" 

// Fonctions utilitaires
std::string chooseRandomWord(const std::string& filename) {
    std::vector<std::string> words;
    std::ifstream file(filename);
    if (!file.is_open()) {
        mvprintw(LINES / 2, (COLS - 30) / 2, "Erreur: dico.txt introuvable!");
        refresh();
        getch(); // Attendre une touche
        endwin();
        exit(1);
    }
    std::string word;
    while (file >> word) {
        if (word.length() > 2) { // Minimum 3 lettres pour le pendu
            words.push_back(word);
        }
    }
    file.close();

    if (words.empty()) {
        mvprintw(LINES / 2, (COLS - 30) / 2, "Erreur: Dictionnaire vide ou mots trop courts!");
        refresh();
        getch();
        endwin();
        exit(1);
    }

    srand(time(0));
    return words[rand() % words.size()];
}

void drawHangman(int attempts) {
    attron(A_BOLD);
    mvprintw(2, COLS - 15, " _____ ");
    mvprintw(3, COLS - 15, "|     |");
    mvprintw(4, COLS - 15, "|     %c", (attempts <= 5 ? 'O' : ' '));
    mvprintw(5, COLS - 15, "|    %c%c%c", (attempts <= 3 ? '/' : ' '), (attempts <= 4 ? '|' : ' '), (attempts <= 2 ? '\\' : ' '));
    mvprintw(6, COLS - 15, "|     %c", (attempts <= 1 ? '|' : ' '));
    mvprintw(7, COLS - 15, "|    %c %c", (attempts <= 0 ? '/' : ' '), (attempts <= 0 ? '\\' : ' '));
    mvprintw(8, COLS - 15, "|______|");
    attroff(A_BOLD);
}

void displayGameState(const Hangman& game) {
    clear(); // Effacer l'écran pour rafraîchir
    std::string wordDisplay = game.getCurrentDisplayWord();
    int centerWord = (COLS - wordDisplay.length()) / 2;
    int centerMessage = (COLS - 20) / 2; // Pour centrer les messages

    mvprintw(1, centerMessage, "Jeu du Pendu");
    mvprintw(10, centerWord, "%s", wordDisplay.c_str());

    mvprintw(12, centerMessage, "Tentatives restantes: %d", game.getRemainingAttempts());

    std::string guessedLettersStr = "Lettres devinees: ";
    for (char c : game.getGuessedLetters()) {
        guessedLettersStr += c;
        guessedLettersStr += ' ';
    }
    mvprintw(14, centerMessage, "%s", guessedLettersStr.c_str());

    drawHangman(game.getRemainingAttempts());
    refresh();
}

int main_ncurses() {
    // Initialisation de NCurses
    initscr();             // Démarre le mode curses
    cbreak();              // Saisie immédiate, pas de buffer
    noecho();              // N'affiche pas les caractères tapés
    curs_set(0);           // Cache le curseur
    keypad(stdscr, TRUE);  // Active les touches spéciales (comme F1, flèches)

    // Vérifier si le terminal est assez grand
    if (LINES < 20 || COLS < 60) {
        endwin();
        std::cerr << "Le terminal est trop petit! Minimum 20 lignes x 60 colonnes." << std::endl;
        return 1;
    }

    int choice_mode;
    std::string secretWord;

    // --- Menu de sélection du mode ---
    while(true) {
        clear();
        mvprintw(LINES / 2 - 2, (COLS - 25) / 2, "Choisissez le mode de jeu:");
        mvprintw(LINES / 2, (COLS - 25) / 2, "1. Solo (joueur vs ordi)");
        mvprintw(LINES / 2 + 1, (COLS - 25) / 2, "2. Deux joueurs");
        mvprintw(LINES / 2 + 3, (COLS - 25) / 2, "Votre choix: ");
        refresh();

        char input = getch();
        if (input == '1') {
            choice_mode = 1;
            break;
        } else if (input == '2') {
            choice_mode = 2;
            break;
        }
    }

    if (choice_mode == 1) {
        secretWord = chooseRandomWord("dico.txt");
    } else { // mode 2
        echo(); // Réactiver l'écho pour la saisie du mot
        curs_set(1); // Afficher le curseur
        clear();
        mvprintw(LINES / 2 - 1, (COLS - 30) / 2, "Joueur 1, entrez le mot secret:");
        mvprintw(LINES / 2, (COLS - 30) / 2, "(min 3 lettres, pas de caractères spéciaux)");
        mvprintw(LINES / 2 + 2, (COLS - 20) / 2, "Mot: ");
        refresh();
        
        char wordBuffer[256];
        getnstr(wordBuffer, sizeof(wordBuffer) - 1);
        secretWord = std::string(wordBuffer);
        
        noecho(); // Désactiver l'écho à nouveau
        curs_set(0); // Cacher le curseur
    }

    if (secretWord.empty() || secretWord.length() < 3) {
        mvprintw(LINES / 2, (COLS - 30) / 2, "Mot invalide. Fin du jeu.");
        refresh();
        getch();
        endwin();
        return 1;
    }

    Hangman game(secretWord);

    while (!game.isGameOver()) {
        displayGameState(game);
        mvprintw(LINES - 2, (COLS - 20) / 2, "Devinez une lettre: ");
        refresh();

        char guess = getch();
        if (isalpha(guess)) { // S'assurer que c'est une lettre
            game.guessLetter(guess);
        } else {
            mvprintw(LINES - 1, (COLS - 30) / 2, "Veuillez entrer une lettre valide.");
            refresh();
            napms(1000); // Pause 1 seconde
        }
    }

    displayGameState(game); // Afficher l'état final
    if (game.isWordGuessed()) {
        mvprintw(LINES - 4, (COLS - 30) / 2, "FELICITATIONS ! Vous avez trouvé le mot.");
    } else {
        mvprintw(LINES - 4, (COLS - 40) / 2, "DOMMAGE, vous avez été pendu. Le mot était: %s", game.getSecretWord().c_str());
    }
    mvprintw(LINES - 2, (COLS - 20) / 2, "Appuyez sur une touche pour quitter.");
    refresh();
    getch(); // Attendre une touche avant de quitter

    endwin(); // Termine le mode curses
    return 0;
}

int main() {
    return main_ncurses();
}
