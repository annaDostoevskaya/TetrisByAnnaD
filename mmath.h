/* 
Author: github.com/annadostoevskaya
File: mmath.h
Date: August 19th 2022 10:57 pm 

Description: <empty>
*/

#ifndef MATH_H
#define MATH_H

#include "base_types.h"

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

//
//
//

inline r64 Remainder(r64 Dividend, r64 Divisor);
internal r64 Floor(r64 Arg);
internal r64 Fact(u64 X);
internal r64 Sin(r64 X);
internal void MultiplyMatrices(i32 *M1, i32 *M2, 
                               u32 RowsAndColumnSizeForM1AndM2,  // NOTE(annad): Rows size is Colum and Column size is Rows?... idk...
                               u32 ColumnsSizeM1, u32 RowsSizeM2, i32 *Dest);

#endif //MATH_H

