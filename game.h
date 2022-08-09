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
    u32 PosX;
    u32 PosY;
    
    // NOTE(annad): The position at which the inner square of the field begins.
    u32 FieldPosX;
    u32 FieldPosY;
    
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
#endif
    b32 Initialized;
    
    u32 PosX;
    u32 PosY;
    u32 PlayerSize;
    
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
