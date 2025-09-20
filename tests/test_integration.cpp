#include <catch2/catch_test_macros.hpp>
#include "hangman/hangman.h"

TEST_CASE("Simuler une partie complète gagnée", "[Integration]") {
    Hangman game("chat");

    // Étape 1 : mauvaise lettre
    REQUIRE(game.guessLetter('z') == false);
    REQUIRE(game.getRemainingAttempts() == 5);
    REQUIRE_FALSE(game.hasWon());

    // Étape 2 : bonne lettre
    REQUIRE(game.guessLetter('c') == true);
    REQUIRE(game.getCurrentDisplayWord() == "c___");

    REQUIRE(game.guessLetter('h') == true);
    REQUIRE(game.getCurrentDisplayWord() == "ch__");

    REQUIRE(game.guessLetter('a') == true);
    REQUIRE(game.getCurrentDisplayWord() == "cha_");

    REQUIRE(game.guessLetter('t') == true);
    REQUIRE(game.getCurrentDisplayWord() == "chat");

    // Fin de partie
    REQUIRE(game.isGameOver());
    REQUIRE(game.hasWon());
    REQUIRE_FALSE(game.hasLost());
}

TEST_CASE("Simuler une partie complète perdue", "[Integration]") {
    Hangman game("chat");

    for (char c : {'x', 'y', 'z', 'u', 'v', 'w'}) {
        REQUIRE(game.guessLetter(c) == false);
    }

    REQUIRE(game.getRemainingAttempts() == 0);
    REQUIRE(game.isGameOver());
    REQUIRE(game.hasLost());
    REQUIRE_FALSE(game.hasWon());
}
