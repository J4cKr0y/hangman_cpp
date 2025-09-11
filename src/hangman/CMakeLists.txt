add_library(hangman_lib STATIC hangman.cpp)
target_include_directories(hangman_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(hangman_lib PUBLIC cxx_std_17)
