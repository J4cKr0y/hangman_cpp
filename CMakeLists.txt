cmake_minimum_required(VERSION 3.10)
project(HangmanGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- CONFIGURATION DES FLAGS ---
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# --- LA LIBRAIRIE ---
add_subdirectory(src/hangman)

# --- EXÉCUTABLE DESKTOP (NCURSES) ---
if(NOT EMSCRIPTEN)
    find_package(NCurses REQUIRED)
    
    add_executable(hangman_exec src/main.cpp)
    target_include_directories(hangman_exec PRIVATE include)
    target_link_libraries(hangman_exec PRIVATE hangman_lib NCurses::NCurses)

    set_target_properties(hangman_exec PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    
    # Copie du dictionnaire
    configure_file(${CMAKE_SOURCE_DIR}/data/dico.txt ${CMAKE_BINARY_DIR}/bin/dico.txt COPYONLY)

    # --- TESTS UNITAIRES (Seulement en Desktop) ---
    include(FetchContent)
    FetchContent_Declare(
      catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.5.2
    )
    FetchContent_MakeAvailable(catch2)

    file(GLOB TEST_SOURCES tests/test_*.cpp)
    if(TEST_SOURCES)
        add_executable(tests ${TEST_SOURCES})
        target_link_libraries(tests PRIVATE hangman_lib Catch2::Catch2WithMain)
        enable_testing()
        add_test(NAME HangmanTests COMMAND tests)
    endif()

# --- VERSION WEB (EMSCRIPTEN) ---
else()
    add_executable(hangman_wasm src/main_wasm.cpp)
    target_link_libraries(hangman_wasm PRIVATE hangman_lib)
    
    # Options spécifiques à Emscripten pour l'export JS
    set_target_properties(hangman_wasm PROPERTIES
        LINK_FLAGS "-s EXPORTED_FUNCTIONS=['_initGame','_guessLetter','_getCurrentDisplayWord','_getRemainingAttempts','_isGameOver','_isWordGuessed','_getSecretWord','_getGuessedLettersString','_cleanup'] -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        SUFFIX ".js"
    )
endif()
