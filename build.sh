#!/bin/bash

## Author: github.com/annadostoevskaya
## File: build.sh
## August 30th 2022 12:43 pm 
##
## Description:
## Before running this file, you need install the SDL
## library on your system.
## For this you can visit this page:
## https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php


SRC=`pwd`
SDL_FLAGS=`sdl2-config --cflags --libs`
OUTPUT_DIR="build"
O_FLAGS=-O3
DEV_FLAGS=-D_GAME_INTERNAL
OPTS="$DEV_FLAGS -D_GAME_LINUX"

if [ ! -d "./$OUTPUT_DIR" ]
then
    mkdir ./$OUTPUT_DIR
fi

pushd $OUTPUT_DIR > /dev/null

gcc $SRC/sdl_game.cpp -o sdl_game $SDL_FLAGS $GAME_FLAGS
gcc $SRC/game.cpp -shared $SDL_FLAGS $GAME_FLAGS -o game.dll

popd > /dev/null

