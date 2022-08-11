/* 
Author: github.com/annadostoevskaya
File: game.h
Date: August 6th 2022 3:35 pm 

Description: <empty>
*/

#ifndef GAME_H
#define GAME_H

#define WELL_WIDTH 9
#define WELL_HEIGHT 16

struct U8Vec2
{
    u8 X;
    u8 Y;
};

struct U32Vec2
{
    u32 X;
    u32 Y;
};

//
//
//

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

enum tetro_states
{
    TETRO_STATE_SPAWN = 0,
    TETRO_STATE_IN_PROGRESS,
    TETRO_STATE_FALL,
    
    TETRO_STATE_TOTAL
};

enum blocks_states
{
    BLOCK_STATE_EMPTY = 0,
    BLOCK_STATE_FILLED,
    BLOCK_STATE_TETRO,
};

struct well
{
    U32Vec2 Pos;
    
    // NOTE(annad): The position at which the inner square of the field begins.
    U32Vec2 FieldPos;
    
    u16 Width;
    u16 Height;
    
    // NOTE(annad): Width and Height with borders.
    u16 FullWidth;
    u16 FullHeight;
    
    u32 CellSideSize;
    
    blocks_states Field[WELL_WIDTH * WELL_HEIGHT];
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
    // tetrimo CurrentTetrimo;
    
    tetrominoes TetrominoType;
    tetro_states TetroState;
    u16 TetrominoPosXInWell;
    u16 TetrominoPosYInWell;
    
    u32 TetrimoDownTime;
};

#endif //GAME_H
