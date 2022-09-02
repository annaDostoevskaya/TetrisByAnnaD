/* 
Author: github.com/annadostoevskaya
File: math.cpp
Date: August 30th 2022 10:51 pm 

Description: <empty>
*/

#include "mmath.h"

#define MMATH_PI 3.14159265359

inline r64 Remainder(r64 Dividend, r64 Divisor)
{
    r64 Result = Dividend - ((r64)(i64)(Dividend / Divisor)) * Divisor;
    
    return Result;
}

internal r64 Fact(u64 X)
{
    r64 Result = 1.0f;
    
    while(X > 0)
    {
        Result *= ((r64)X);
        X--;
    }
    
    return Result;
}

internal r64 Sin(r64 X)
{
    if(((i64)(X / MMATH_PI)) % 2)
    {
        X *= (-1.0f);
    }
    
    X = Remainder(X, MMATH_PI);
    
    r64 Result = X;
    r64 Pow2X = X * X;
    
    r64 Sign = 1.0f;
    
    for(i8 i = 0; i < 4; i++)
    {
        X *= Pow2X;
        
        Sign *= (-1.0f);
        Result += (Sign * (X / Fact(i * 2 + 3)));
    }
    
    return Result;
}

internal r64 Floor(r64 Arg)
{
    r64 Result = (r64)((u32)Arg);
    
    return Result;
}

internal void MultiplyMatrices(i32 *A, i32 *B, 
                               u32 CommonSize, u32 WidthB, u32 HeightA, 
                               i32 *Dest)
{
    for(u32 Y = 0; Y < HeightA; Y++)
    {
        for(u32 X = 0; X < WidthB; X++)
        {
            Dest[Y * WidthB + X] = 0;
            for(u32 i = 0; i < CommonSize; i++)
            {
                Dest[Y * WidthB + X] += A[Y * CommonSize + i] * B[i * WidthB + X];
            }
        }
    }
}

