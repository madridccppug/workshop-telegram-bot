cmake_minimum_required(VERSION 2.8.12)
project(BotHelloWorld CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(bot_hello_world main.cpp)
target_link_libraries(bot_hello_world ${CONAN_LIBS})
set_target_properties(bot_hello_world PROPERTIES CXX_STANDARD 11)
