/* 
Author: github.com/annadostoevskaya
File: game_well.h
Date: August 13th 2022 10:49 pm 

Description: <empty>
*/

#ifndef WELL_H
#define WELL_H

#define WELL_WIDTH 10
#define WELL_HEIGHT 20

enum block_state
{
    BLOCK_STATE_EMPTY = 0,
    BLOCK_STATE_FILLED,
    BLOCK_STATE_TETRO,
};

struct well
{
    block_state Field[WELL_WIDTH * WELL_HEIGHT];
    
    u32Vec2 Pos;
    
    // NOTE(annad): The position at which the inner square of the field begins.
    u32Vec2 FieldPos;
    
    u16 Width;
    u16 Height;
    
    // NOTE(annad): Width and Height with borders.
    u16 FullWidth;
    u16 FullHeight;
    
    u32 CellSideSize;
};

#endif //WELL_H

