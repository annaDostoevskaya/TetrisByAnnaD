/* 
Author: github.com/annadostoevskaya
File: base_types.h
Date: August 6th 2022 3:33 pm 

Description: <empty>
*/

#ifndef BASE_TYPES_H
#define BASE_TYPES_H

#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef float r32;
typedef double r64;

typedef u32 b32;
typedef u16 b16;
typedef bool b8;

#define internal static
#define globalv static
#define localv static

#endif //BASE_TYPES_H
