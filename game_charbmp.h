/* 
Author: github.com/annadostoevskaya
File: game_charbmp.h
Date: September 1st 2022 8:47 pm 

Description: <empty>
*/

#ifndef CHARBMP_H
#define CHARBMP_H

// NOTE(annad): See T. Korman, Introdaction to Algrorithms, Vol. 3, 296 page.
#define HASH_MAGIC_VALUE 0.6180339887


// NOTE(annad): Developer defined symbols, you can create youself, if, are you can.
// Remeber about little-endian and that last byte is CHAR 0x43 - 'C'
#define CHAR_BITMAP_C 0xf08080808080f043
#define CHAR_BITMAP_D 0xe09090909090e044
#define CHAR_BITMAP_E 0xf08080e08080f045
#define CHAR_BITMAP_O 0xf09090909090f04f
#define CHAR_BITMAP_R 0xb0e0c0f09090f052
#define CHAR_BITMAP_S 0xf09010f08090f053
#define CHAR_BITMAP_1 0xe040404040c04031
#define CHAR_BITMAP_2 0xf080402010906032
#define CHAR_BITMAP_3 0x6090106010906033
#define CHAR_BITMAP_4 0x1010f09050301034
#define CHAR_BITMAP_5 0xe01010e08090f035
#define CHAR_BITMAP_6 0xf09090f08090f036
#define CHAR_BITMAP_7 0x40e040201090f037
#define CHAR_BITMAP_8 0xf09090f09090f038
#define CHAR_BITMAP_9 0xf09010f09090f039
#define CHAR_BITMAP_0 0xf09090909090f030
#define CHAR_BITMAP_COLON 0x404000000040403a

#define CHR_BMP_BUF_SIZE 17
#define CHAR_COLOR 0.93f

struct char_bmp_buf
{
    u64 Buf[CHR_BMP_BUF_SIZE];
    size_t Size;
};

#endif //CHARBMP_H
