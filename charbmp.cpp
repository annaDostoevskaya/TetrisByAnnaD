/* 
Author: github.com/annadostoevskaya
File: charbmp.cpp
Date: September 1st 2022 8:44 pm 

Description: <empty>
*/

#include "charbmp.h"

internal u64 GetChrBmpHash(size_t BufSize, u8 Char)
{
    r64 Rem = Remainder(Char * HASH_MAGIC_VALUE, 1.0f);
    u64 Result = (u64)Floor((r64)BufSize * Rem);
    
    return Result;
}

internal u64 GetCharBitmap(char_bmp_buf *CharBmps, u8 Chr)
{
    u64 BaseIndex = GetChrBmpHash(CharBmps->Size, Chr);
    
    u64 ChrBmp = 0x0;
    u64 Mask = 0xFF;
    
    i16 ShiftIndex = 0;
    for(ShiftIndex = 0; BaseIndex + ShiftIndex < CharBmps->Size; ShiftIndex++)
    {
        ChrBmp = CharBmps->Buf[BaseIndex + ShiftIndex];
        if(Chr == (u8)(ChrBmp & Mask))
        {
            return ChrBmp;
        }
    }
    
    for(ShiftIndex = 0; ShiftIndex < BaseIndex; ShiftIndex++)
    {
        ChrBmp = CharBmps->Buf[ShiftIndex];
        if(Chr == (u8)(ChrBmp & Mask))
        {
            return ChrBmp;
        }
    }
    
    return 0x0;
}

internal void PushCharBitmap(char_bmp_buf *CharBmps, u64 ChrBmp)
{
    u64 Mask = 0xFF;
    u8 Chr = (u8)(ChrBmp & Mask);
    u64 BaseIndex = GetChrBmpHash(CharBmps->Size, Chr);
    
    i16 ShiftIndex = 0;
    for(ShiftIndex = 0; BaseIndex + ShiftIndex < CharBmps->Size; ShiftIndex++)
    {
        if(!CharBmps->Buf[BaseIndex + ShiftIndex])
        {
            CharBmps->Buf[BaseIndex + ShiftIndex] = ChrBmp;
            return;
        }
    }
    
    for(ShiftIndex = 0; ShiftIndex < BaseIndex; ShiftIndex++)
    {
        if(!CharBmps->Buf[ShiftIndex])
        {
            CharBmps->Buf[ShiftIndex] = ChrBmp;
            return;
        }
    }
    
    // NOTE(annad): There no free cells.
    // DEV: assert(1 != 1);
    return;
}


internal void DisplayString(game_screen_buffer *Buffer, 
                            char_bmp_buf *CharBmps, str_buf *String,
                            r32 X, r32 Y, r32 Scale)
{
    
    // DEV: assert(X >= 0 && X <= 1);
    // DEV: assert(Y >= 0 && Y <= 1);
    // DEV: assert(Scale >= 0 && Scale <= 1);
    
    u8 *CharBmp = NULL;
    for(u8 i = 0; i < String->Size; i++)
    {
        u8 Symbol = String->Buf[i];
        
        if(Symbol == '\0' || Symbol == ' ')
        {
            continue;
        }
        
        u64 ProxyChrBmp = GetCharBitmap(CharBmps, Symbol);
        CharBmp = (u8*)(&ProxyChrBmp);
        
        for(i8 j = 7; j > 0; j--)
        {
            for(u8 k = 0; k < 8; k++)
            {
                if(CharBmp[j] & (1 << k))
                {
                    u32 Size = (u32)(Scale * (r32)Buffer->Height);
                    u32 LetterSpace = 8 * Size;
                    u32 LetterShift = i * LetterSpace;
                    u32 BitShiftX = (u32)((r32)Buffer->Width * X) - (k * Size);
                    u32 BitShiftY = (u32)((r32)Buffer->Height * Y) - (j * Size);
                    u32 GlobalX = BitShiftX + LetterShift;
                    u32 GlobalY = BitShiftY;
                    DrawRectangle(Buffer, GlobalX, GlobalY, Size, Size, CHAR_COLOR, CHAR_COLOR, CHAR_COLOR);
                }
            }
        }
    }
}
