cmake_minimum_required(VERSION 2.8.12)
project(BotRandom CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(bot_random main.cpp bot_random.h bot_random.cpp)
target_link_libraries(bot_random ${CONAN_LIBS})
set_target_properties(bot_random PROPERTIES CXX_STANDARD 11)
