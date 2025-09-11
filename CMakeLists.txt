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

target_include_directories(hangman_exec PRIVATE include)

add_subdirectory(src/hangman)

add_executable(hangman_exec src/main.cpp)

target_include_directories(hangman_exec PRIVATE include)

target_link_libraries(hangman_exec PRIVATE hangman_lib)

set_target_properties(hangman_exec PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
configure_file(${CMAKE_SOURCE_DIR}/data/dico.txt ${CMAKE_BINARY_DIR}/bin/dico.txt COPYONLY)
