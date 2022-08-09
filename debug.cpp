/* 
Author: github.com/annadostoevskaya
File: debug.cpp
Date: August 7th 2022 5:29 pm 

Description: <empty>
*/

#include "game.h"
#include "sdl_game.h"

internal void DEBUG_GetStructInfo(game_state *Data)
{
    char Buffer[256];
    
    b32 StructAddressAligned = false;
    u64 StructAddress = (u64)Data;
    if(StructAddress % 8 == 0)
    {
        StructAddressAligned = true;
    }
    
    b32 BoolStrubAddressAligned = false;
    u64 BoolStubAddress = (u64)(&(Data->stub));
    if(BoolStubAddress % 8 == 0)
    {
        BoolStrubAddressAligned = true;
    }
    
    b32 AfterOneByteBoolAligned = false;
    u64 AfterOneByteBoolAddress = (u64)(&(Data->Initialized));
    if(AfterOneByteBoolAddress % 8 == 0 || AfterOneByteBoolAddress % 4 == 0)
    {
        AfterOneByteBoolAligned = true;
    }
    
    
    sprintf(Buffer, "Struct Address: 0x%p\nStruct Address aligned: %d\n", 
            Data, StructAddressAligned);
    OutputDebugString(Buffer);
    
    sprintf(Buffer, "BoolStubAddress: 0x%p\nBoolStrubAddressAligned aligned: %d\n", 
            &(Data->stub), BoolStrubAddressAligned);
    OutputDebugString(Buffer);
    
    sprintf(Buffer, "AfterOneByteBoolAddress: 0x%p\nAfterOneByteBoolAligned aligned: %d\n", 
            &(Data->Initialized), AfterOneByteBoolAligned);
    OutputDebugString(Buffer);
    
    // __debugbreak();
}


internal void DEBUG_DrawELT(game_buffer *Buffer)
{
    u32 *Pixels = (u32 *)Buffer->Memory;
    for(u32 Y = 0; Y < Buffer->Height; Y++)
    {
        for(u32 X = 0; X < Buffer->Width; X++)
        {
            Pixels[Y * Buffer->Width + X] = 0xFFFF0000;
            
            if(X % 2 == 0)
            {
                Pixels[Y * Buffer->Width + X] = 0xFF00FF00;
            }
            
            if(X % 3 == 0)
            {
                Pixels[Y * Buffer->Width + X] = 0xFF0000FF;
            }
        }
    }
}


internal void DEBUG_DrawGrid(game_buffer *Buffer, u32 GridCellSize)
{
    u32 *Pixels = (u32 *)Buffer->Memory;
    for(u32 Y = 0; Y < Buffer->Height; Y++)
    {
        for(u32 X = 0; X < Buffer->Width; X++)
        {
            if(X % GridCellSize == 0 || Y % GridCellSize == 0)
            {
                Pixels[Y * Buffer->Width + X] = 0xFF111111;
            }
        }
    }
}


internal void DEBUG_CheckAllPositions(game_buffer *Buffer, game_time *Time, u32 CheckerSize)
{
    u32 CountX = Buffer->Width / CheckerSize;
    u32 CountY = Buffer->Height / CheckerSize;
    
    localv u32 Accum = 0;
    localv u32 IterCountX = 0;
    localv u32 IterCountY = CountY;
    
    if(Accum >= 100)
    {
        if(IterCountX > CountX)
        {
            IterCountX = 0;
            IterCountY--;
        }
        else
        {
            IterCountX++;
        }
        
        if(IterCountY > CountY)
        {
            IterCountY = CountY;
        }
        
        Accum = 0;
    }
    
    u32 GlobalPosX = IterCountX * CheckerSize;
    u32 GlobalPosY = IterCountY * CheckerSize;
    
    if(IterCountY == CountY)
    {
        GlobalPosY += Buffer->Height % CheckerSize;
    }
    
    DrawRectangle(Buffer, 
                  GlobalPosX, GlobalPosY, 
                  CheckerSize, CheckerSize,
                  0.93f, 0.93f, 0.93f);
    
    Accum += Time->dt;
}


void DEBUG_CheckWell(well *Well, game_time *Time)
{
    localv u32 Accum = 0;
    localv u32 Index = 0;
    
    if(Accum >= 100)
    {
        if(Index < Well->Width * Well->Height)
        {
            if(Index == 0)
            {
                if(Well->Field[Well->Width * Well->Height - 1])
                {
                    Well->Field[Well->Width * Well->Height - 1] = false;
                }
                Well->Field[Index++] = true;
            }
            else
            {
                Well->Field[Index - 1] = false;
                Well->Field[Index++] = true;
            }
        }
        else
        {
            Index = 0;
        }
        Accum = 0;
    }
    
    Accum += Time->dt;
}