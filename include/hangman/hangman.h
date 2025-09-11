#ifndef HANGMAN_H
#define HANGMAN_H

#include <string>
#include <set>

class Hangman {
public:
    Hangman(const std::string& secretWord);

    bool guessLetter(char letter);
    bool isGameOver() const;
    bool isWordGuessed() const;
    std::string getCurrentDisplayWord() const;
    int getRemainingAttempts() const;

    std::set<char> getGuessedLetters() const;
    bool hasWon() const;
    bool hasLost() const;

private:
    std::string m_secretWord;
    std::string m_guessedWord;
    int m_remainingAttempts;
    std::set<char> m_guessedLetters;
};

#endif // HANGMAN_H
