/* 
Author: github.com/annadostoevskaya
File: algs.h
Date: August 30th 2022 11:19 pm 

Description: <empty>
*/

#ifndef ALGS_H
#define ALGS_H

#include "base_types.h"
#include <emmintrin.h> // NOTE(annad): Idk, it's processor or compiler dependecy?..

void SIMDMemoryCopy(void *Dst, void *Src, size_t SrcSize);
void AddressSwap(void **Addr1, void **Addr2);
internal void IToStr(u8 *StrBuf, u32 StrLength, u64 Integer);

#endif //ALGS_H

