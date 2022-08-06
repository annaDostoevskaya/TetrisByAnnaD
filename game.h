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

struct well
{
    u16 Width;
    u16 Height;
    
    u32 PosX;
    u32 PosY;
    
    u32 CellSideSize;
    
    u8 Field[WELL_WIDTH * WELL_HEIGHT];
};

struct tetromino
{
    tetrominoes TetrominoType;
    u16 WellPosX;
    u16 WellPosY;
};

struct game_state
{
    b32 Initialized;
    
    u32 PosX;
    u32 PosY;
    
    u32 MetaPixelSize;
    well Well;
    tetrimo CurrentTetrimo;
};

#endif //GAME_H
