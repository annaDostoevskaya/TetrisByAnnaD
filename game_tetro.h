/* 
Author: github.com/annadostoevskaya
File: game_tetro.h
Date: August 20th 2022 0:48 am 

Description: <empty>
*/

#ifndef TETRO_H
#define TETRO_H

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
    // NOTE(saiel): Use enum classes.
    TETRO_O = 0,
    TETRO_T,
    TETRO_L,
    TETRO_J,
    TETRO_Z,
    TETRO_S,
    TETRO_I,
    
    TETRO_TOTAL,
    // NOTE(saiel): TETRO_UNDEFINED or TETRO_INVALID for invalid states, for example.
};

struct tetro
{
    i8Vec2 Tetrominos[TETRO_TOTAL][MAX_TETRO_SIZE];
    tetro_state State;
    tetro_type Type;
    tetro_type TypesBag[TETRO_TOTAL];
    u32 TypesBagSize;
    
    i8Vec2 *Pos;
    i8Vec2 *ShadowPos;
    i8Vec2 PosBuffers[2];
    
    i8Vec2 *Content;
    i8Vec2 *ShadowContent;
    i8Vec2 ContentBuffers[2][MAX_TETRO_SIZE];
};

#endif //TETRO_H

