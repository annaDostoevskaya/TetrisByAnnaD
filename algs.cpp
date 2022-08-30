/* 
Author: github.com/annadostoevskaya
File: algs.cpp
Date: August 30th 2022 11:19 pm 

Description: <empty>
*/

#include "algs.h"

void SIMDMemoryCopy(void *Dst, void *Src, size_t SrcSize)
{
    for(size_t i = 0; i < (SrcSize / 16); i++)
    {
        __m128i Chunk = _mm_set_epi64x(((u64*)Src)[i * 2 + 1], ((u64*)Src)[i * 2 + 0]);
        __m128i* p_m128i_Dst = (__m128i*)(Dst);
        _mm_storeu_si128(&(p_m128i_Dst[i]), Chunk);
    }
    
    //
    //
    // NOTE(annad): We can use in this place __m64i or other and 
    // copy for 8 bit per 1 opcode mb?.. 
    // It's make sense?..
    // 
    //
    
    size_t RemainderAlignment = SrcSize % 16;
    if(RemainderAlignment != 0)
    {
        for(size_t i = 0; i < RemainderAlignment; i++)
        {
            ((u8*)Dst)[SrcSize - i - 1] = ((u8*)Src)[SrcSize - i - 1];
        }
    }
}

internal void IToStr(u8 *StrBuf, u32 StrLength, u64 Integer)
{
    for(i32 i = 0; i < StrLength; i++)
    {
        // NOTE(annad): Gift from Saiel, Thanks.
        u64 Digit = Integer % 10;
        Integer /= 10;
        StrBuf[StrLength - 1 - i] = (u8)(Digit + '0');
    }
}

void AddressSwap(void **Addr1, void **Addr2)
{
    void *temp = *Addr1;
    *Addr1 = *Addr2;
    *Addr2 = temp;
}

