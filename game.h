/* 
Author: github.com/annadostoevskaya
File: game.h
Date: August 6th 2022 3:35 pm 

Description: <empty>
*/

#ifndef GAME_H
#define GAME_H

#include "math.h"
#include "base_types.h"
#include "sdl_game.h"
#include "well.h"
#include "tetro.h"

struct game_state
{
#ifdef _GAME_INTERNAL
    // NOTE(annad): Just for test, see DEBUG_GetInfo()
    bool stub;
    b32 BoolState;
#endif
    b16 Initialized;
    b16 Pause;
    
    u32 MetaPixelSize;
    well Well;
    tetro Tetro;
};

#define GAME_UPDATE_AND_RENDER(name) internal void name(game_buffer *Buffer, \
game_input *Input, \
game_memory *Memory, \
game_time *Time)

GAME_UPDATE_AND_RENDER(UpdateAndRender);

#endif //GAME_H
