/* 
Author: github.com/annadostoevskaya
File: well.h
Date: August 13th 2022 10:49 pm 

Description: <empty>
*/

#ifndef WELL_H
#define WELL_H

//
// math.h
//

struct i8Vec2
{
    i8 X;
    i8 Y;
};

struct u8Vec2
{
    u8 X;
    u8 Y;
};

struct i16Vec2
{
    i16 X;
    i16 Y;
};

struct u16Vec2
{
    u16 X;
    u16 Y;
};

struct i32Vec2
{
    i32 X;
    i32 Y;
};

struct u32Vec2
{
    u32 X;
    u32 Y;
};

struct r64Vec2
{
    union {
        struct {
            r64 X;
            r64 Y;
        };
        
        struct {
            r64 A;
            r64 B;
        };
    };
};

//
// math. h
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
    u32Vec2 Pos;
    
    // NOTE(annad): The position at which the inner square of the field begins.
    u32Vec2 FieldPos;
    
    u16 Width;
    u16 Height;
    
    // NOTE(annad): Width and Height with borders.
    u16 FullWidth;
    u16 FullHeight;
    
    u32 CellSideSize;
    
    blocks_states Field[WELL_WIDTH * WELL_HEIGHT];
};

#endif //WELL_H
