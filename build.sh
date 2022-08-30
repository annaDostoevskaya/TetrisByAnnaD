#!/bin/bash

# NOTE(annad): For pause run it:
# read -p "Press [Enter] key to start backup..."

CODE="/home/annad/projects/TetrisByAnnaD"

SDL_FLAGS="-I/usr/include/SDL2 -L/usr/lib -pthread -lSDL2 -D_REENTRANT"
GAME_FLAGS="-D_GAME_INTERNAL -D_GAME_LINUX"

pushd build > /dev/null

gcc $CODE/sdl_game.cpp -o sdl_game $SDL_FLAGS $GAME_FLAGS
gcc $CODE/game.cpp -shared $SDL_FLAGS $GAME_FLAGS -o game.dll

popd > /dev/null

