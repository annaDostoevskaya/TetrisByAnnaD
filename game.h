/* 
Author: github.com/annadostoevskaya
File: game.h
Date: August 6th 2022 3:35 pm 

Description: <empty>
*/

#ifndef GAME_H
#define GAME_H

//
//
//

#include "well.h"

enum tetro_state
{
    TETRO_STATE_SPAWN = 0,
    TETRO_STATE_IN_PROGRESS,
    TETRO_STATE_FALL,
    
    TETRO_STATE_TOTAL
};

#define SIZEOF_TETRO_BLOCK sizeof(i8Vec2)
#define MAX_TETRO_SIZE 4

enum tetro_type
{
    TETRO_O,
    TETRO_T,
    TETRO_L,
    TETRO_J,
    TETRO_Z,
    TETRO_S,
    TETRO_I,
    
    TETRO_TOTAL,
};

struct tetro
{
    i8Vec2 Tetrominos[TETRO_TOTAL][MAX_TETRO_SIZE];
    
    tetro_state State;
    tetro_type Type;
    
    i16Vec2 *Pos;
    i16Vec2 *ShadowPos;
    i16Vec2 PosBuffers[2];
    
    i8Vec2 *Content;
    i8Vec2 *ShadowContent;
    i8Vec2 ContentBuffers[2][MAX_TETRO_SIZE];
    
    u32 DownTime;
    u32 AccumTime;
};

struct game_state
{
#ifdef _GAME_INTERNAL
    // NOTE(annad): Just for test, see DEBUG_GetInfo()
    bool stub;
    b32 BoolState;
#endif
    b32 Initialized;
    
    u32 MetaPixelSize;
    well Well;
    tetro Tetro;
    
    b32 Pause;
};

#endif //GAME_H
