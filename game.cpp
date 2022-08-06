/* 
Author: github.com/annadostoevskaya
File: main.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "platform_layer.cpp"

#include "base_types.h"
#include "game.h"
#include "platform_layer.h"

#define WHITE_COLOR_RGB 0.93f
#define META_PIXEL_COLOR 0.93f


internal void DrawRectangle(game_buffer *Buffer, 
                            i32 PosX, i32 PosY, u32 W, u32 H,
                            r32 Red, r32 Green, r32 Blue)
{
    // NOTE(annad): Reverse buffer.
    if (PosY > Buffer->Height)
    {
        PosY = Buffer->Height;
    }
    
    if (PosX < 0) 
    {
        PosX = 0;
    }
    
    PosY = Buffer->Height - PosY;
    
    W = W % Buffer->Width;
    H = H % Buffer->Height;
    if (PosX + W >= Buffer->Width) W = Buffer->Width - PosX;
    if (PosY + H >= Buffer->Height) H = Buffer->Height - PosY;
    
    u32 Color = (0xFF000000 | // A R G B
                 ((u32)(Blue * 255.0f) << 0) |
                 ((u32)(Green * 255.0f) << 8) | 
                 ((u32)(Red * 255.0f) << 16));
    
    u32 *Pixels = (u32 *)Buffer->Memory;
    
    for (u32 Y = PosY; Y < PosY + H; Y++)
    {
        for (u32 X = PosX; X < PosX + W; X++)
        {
            Pixels[Y * Buffer->Width + X] = Color;
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


internal void RenderWell(game_buffer *Buffer, well *Well)
{
    for (u32 Y = 0; Y < Well->Height + 2; Y++)
    {
        u32 GlobalPosY = Well->PosY + Y * Well->CellSideSize;
        for (u32 X = 0; X < Well->Width + 2 ; X++)
        {
            if (X == 0 || Y == 0 || 
                X == (Well->Width + 2) - 1 || 
                Y == (Well->Height + 2) - 1 ||
                Well->Field[Y * Well->Width + X]) // NOTE(annad): DANGEROUS: What if X or Y with Well->Width + 2 or Well->Height + 2 move to this Well->Field?..
            {
                u32 GlobalPosX = Well->PosX + X * Well->CellSideSize;
                
                DrawRectangle(Buffer, 
                              GlobalPosX, GlobalPosY, 
                              Well->CellSideSize, Well->CellSideSize,
                              META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
            }
        }
    }
}

GAME_UPDATE_AND_RENDER(UpdateAndRender)
{
    // NOTE(annad): Init.
    game_state *State = (game_state *)Memory->State;
    well *Well = &State->Well;
    
    if(State->Initialized != (b32)true)
    {
        State->MetaPixelSize = Buffer->Height / 54;
        
        Well->CellSideSize = State->MetaPixelSize * 3;
        Well->Width = WELL_WIDTH;
        Well->Height = WELL_HEIGHT;
        Well->PosX = Buffer->Width / 2 - (Well->Width * Well->CellSideSize / 2);
        Well->PosY = Well->CellSideSize;
        
        State->PosX = Buffer->Width / 2;
        State->PosY = Buffer->Height / 2;
        
        State->Initialized = true;
    }
    
    
    RenderWell(Buffer, Well);
    
    {
        // NOTE(annad): Input handling.
        if(Input->PressedKey == KEY_UP)
        {
            State->PosY += 10;
        }
        else if(Input->PressedKey == KEY_DOWN)
        {
            State->PosY -= 10;
        }
        else if(Input->PressedKey == KEY_LEFT)
        {
            State->PosX -= 10;
        }
        else if(Input->PressedKey == KEY_RIGHT)
        {
            State->PosX += 10;
        }
    }
    
    
    // DrawRectangle(Buffer, State->PosX, State->PosY, 100, 100, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    
    DEBUG_CheckAllPositions(Buffer, Time, State->MetaPixelSize * 5);
    DEBUG_DrawGrid(Buffer, Well->CellSideSize);
    // DrawRectangle(Buffer, 0, Buffer->Height, 50, 50, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
}

