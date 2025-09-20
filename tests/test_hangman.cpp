#include <catch2/catch_test_macros.hpp>
#include "hangman/hangman.h"

TEST_CASE("Initial state of Hangman", "[Hangman]") {
    Hangman game("Bonjour");

    REQUIRE(game.getCurrentDisplayWord() == "_______");
    REQUIRE(game.getRemainingAttempts() == 6);
    REQUIRE(game.getGuessedLetters().empty());
    REQUIRE_FALSE(game.isGameOver());
    REQUIRE_FALSE(game.hasWon());
    REQUIRE_FALSE(game.hasLost());
}

TEST_CASE("Correct letter guess updates display", "[Hangman]") {
    Hangman game("bonjour");

    REQUIRE(game.guessLetter('b') == true);
    REQUIRE(game.getCurrentDisplayWord()[0] == 'b');
    REQUIRE(game.getRemainingAttempts() == 6);
    REQUIRE(game.getGuessedLetters().count('b') == 1);
}

TEST_CASE("Incorrect letter guess reduces attempts", "[Hangman]") {
    Hangman game("bonjour");

    REQUIRE(game.guessLetter('z') == false);
    REQUIRE(game.getRemainingAttempts() == 5);
    REQUIRE(game.getGuessedLetters().count('z') == 1);
}

TEST_CASE("Repeated guess does not reduce attempts", "[Hangman]") {
    Hangman game("bonjour");

    game.guessLetter('x');
    int attempts = game.getRemainingAttempts();
    REQUIRE(game.guessLetter('x') == false);
    REQUIRE(game.getRemainingAttempts() == attempts); // no penalty
}

TEST_CASE("Winning the game", "[Hangman]") {
    Hangman game("abc");

    game.guessLetter('a');
    game.guessLetter('b');
    game.guessLetter('c');

    REQUIRE(game.hasWon());
    REQUIRE(game.isGameOver());
    REQUIRE_FALSE(game.hasLost());
}

TEST_CASE("Losing the game", "[Hangman]") {
    Hangman game("abc");

    for (char c : {'x', 'y', 'z', 'u', 'v', 'w'}) {
        game.guessLetter(c);
    }

    REQUIRE(game.hasLost());
    REQUIRE(game.isGameOver());
    REQUIRE_FALSE(game.hasWon());
}
