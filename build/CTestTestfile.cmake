# CMake generated Testfile for 
# Source directory: /home/jacky/Projects/hangman_cpp
# Build directory: /home/jacky/Projects/hangman_cpp/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(HangmanTests "/home/jacky/Projects/hangman_cpp/build/tests")
set_tests_properties(HangmanTests PROPERTIES  _BACKTRACE_TRIPLES "/home/jacky/Projects/hangman_cpp/CMakeLists.txt;41;add_test;/home/jacky/Projects/hangman_cpp/CMakeLists.txt;0;")
subdirs("src/hangman")
subdirs("_deps/catch2-build")
