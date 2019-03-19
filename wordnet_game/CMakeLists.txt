cmake_minimum_required(VERSION 2.8.12)
project(WordnetGame CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(wordnet_game main.cpp bot.h bot.cpp wordnet.h wordnet.cpp)
target_link_libraries(wordnet_game ${CONAN_LIBS})
set_target_properties(wordnet_game PROPERTIES CXX_STANDARD 11)
