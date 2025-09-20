cmake_minimum_required(VERSION 3.10)
project(HangmanGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

add_subdirectory(src/hangman)

add_executable(hangman_exec src/main.cpp)

target_include_directories(hangman_exec PRIVATE include)

target_link_libraries(hangman_exec PRIVATE hangman_lib)

set_target_properties(hangman_exec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
configure_file(${CMAKE_SOURCE_DIR}/data/dico.txt ${CMAKE_BINARY_DIR}/bin/dico.txt COPYONLY)

include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2 # ou la dernière version
)
FetchContent_MakeAvailable(catch2)

file(GLOB TEST_SOURCES tests/test_*.cpp)
add_executable(tests ${TEST_SOURCES})
target_include_directories(tests PRIVATE include)
target_link_libraries(tests PRIVATE hangman_lib Catch2::Catch2WithMain)
add_test(NAME HangmanTests COMMAND tests)
enable_testing()
