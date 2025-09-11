#include "hangman/hangman.h"
#include <algorithm>
#include <cctype>

Hangman::Hangman(const std::string& secretWord) : m_secretWord(secretWord), m_remainingAttempts(6) {
    m_guessedWord = std::string(m_secretWord.length(), '_');
    std::transform(m_secretWord.begin(), m_secretWord.end(), m_secretWord.begin(), ::tolower);
}

bool Hangman::guessLetter(char letter) {
    letter = std::tolower(letter);
    if (m_guessedLetters.find(letter) != m_guessedLetters.end()) {
        return false;
    }
    m_guessedLetters.insert(letter);
    bool letterFound = false;
    for (size_t i = 0; i < m_secretWord.length(); ++i) {
        if (m_secretWord[i] == letter) {
            m_guessedWord[i] = letter;
            letterFound = true;
        }
    }
    if (!letterFound) {
        m_remainingAttempts--;
    }
    return letterFound;
}
bool Hangman::isGameOver() const {
    return m_remainingAttempts <= 0 || isWordGuessed();
}
bool Hangman::isWordGuessed() const {
    return m_guessedWord == m_secretWord;
}
std::string Hangman::getCurrentDisplayWord() const {
    return m_guessedWord;
}
int Hangman::getRemainingAttempts() const {
    return m_remainingAttempts;
}
std::set<char> Hangman::getGuessedLetters() const {
    return m_guessedLetters;
}
bool Hangman::hasWon() const {
    return isWordGuessed() && m_remainingAttempts > 0;
}
bool Hangman::hasLost() const {
    return m_remainingAttempts <= 0 && !isWordGuessed();
}
