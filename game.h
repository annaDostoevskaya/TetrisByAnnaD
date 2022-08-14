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

/* 
enum tetrominoes 
{
    TETROMINO_EMPTY = 0,
    
    TETROMINO_BASE, // NOTE(annad): 1x1 tetomino. Just for tests.
    
    TETROMINO_STRAIGHT,
    TETROMINO_SQUARE,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_SKEW,
    
    TETROMINO_TOTAL
};
 */

enum tetro_states
{
    TETRO_STATE_SPAWN = 0,
    TETRO_STATE_IN_PROGRESS,
    TETRO_STATE_FALL,
    
    TETRO_STATE_TOTAL
};

#define TETRO_MAX_WIDTH 4
#define TETRO_MAX_HEIGHT 4
#define MAX_TETRO_SIZE 4

//
// math.h
//

struct u8Mtrx3x3
{
    u8 Content[3*3];
};

struct u8Mtrx1x3
{
    u8 Content[1*3];
};

void MultiplyMatrixes1x3By3x3(u8Mtrx1x3* m1x3, u8Mtrx3x3* m3x3, m1x3 *Dest)
{
    // TODO(annad): Find algorith for multiplying matrixes!!!
}

//
// math.h
//

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
    // tetrimo CurrentTetrimo;
    
    tetro_states TetroState;
    u16 TetrominoPosXInWell;
    u16 TetrominoPosYInWell;
    u8Vec2 TetroContent[MAX_TETRO_SIZE];
    u32 TetrimoDownTime;
    
    b32 Pause;
};

#endif //GAME_H
