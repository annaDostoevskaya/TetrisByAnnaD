/* 
Author: github.com/annadostoevskaya
File: math.cpp
Date: August 30th 2022 10:51 pm 

Description: <empty>
*/

#include "mmath.h"

// TODO(annad): We must rewrite without using immintrin.h!!!
r64 SVMLSin(r64 Degress)
{
    r64 Result[2] = {};
    __m128d m128d_Degress = _mm_set_pd(0.0f, Degress);
    __m128d m128d_Result = _mm_sin_pd(m128d_Degress);
    _mm_storeu_pd(Result, m128d_Result);
    
    // NOTE(annad): Idk, it's just don't work...
    // __m128i Mask = _mm_set_epi64x(0x0, 0xFFFFFFFF); // NOTE(annad): 0x0 or 0xFFFFFFFF
    // _mm_maskstore_pd(&Degress, Mask, m128d_Result);
    
    return Result[0];
}

r64 Floor(r64 Arg)
{
    r64 Result = (r64)((u32)Arg);
    
    return Result;
}


inline r64 Remainder(r64 Dividend, r64 Divisor)
{
    return Dividend - (r64)((u64)((Dividend / Divisor) * Divisor));
}

