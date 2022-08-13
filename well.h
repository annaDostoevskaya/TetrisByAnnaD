/* 
Author: github.com/annadostoevskaya
File: well.h
Date: August 13th 2022 10:49 pm 

Description: <empty>
*/

#ifndef WELL_H
#define WELL_H

//
//
//

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

#define WELL_WIDTH 9
#define WELL_HEIGHT 16

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

#endif //WELL_H
