cmake_minimum_required(VERSION 3.17)

set(PROJECT_NAME sdl_game)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)

add_library(game SHARED game.cpp)
add_executable(sdl_game sdl_game.cpp)

target_compile_options(game PUBLIC -D_GAME_LINUX -D_GAME_INTERNAL -g)
target_compile_options(sdl_game PUBLIC -D_GAME_LINUX -D_GAME_INTERNAL -g)

target_link_libraries(sdl_game SDL2)

