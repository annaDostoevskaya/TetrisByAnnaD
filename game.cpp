/* 
Author: github.com/annadostoevskaya
File: game.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "base_types.h"
#include "game.h"
#include "sdl_game.h"

#include "sdl_game.cpp"

#include <assert.h>

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

internal void RenderWell(game_buffer *Buffer, well *Well)
{
    DrawRectangle(Buffer, 
                  Well->PosX, Well->PosY, 
                  (Well->FullWidth) * Well->CellSideSize, (Well->FullHeight) * Well->CellSideSize,
                  META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    
    DrawRectangle(Buffer, 
                  Well->FieldPosX, Well->FieldPosY, 
                  Well->Width * Well->CellSideSize, Well->Height * Well->CellSideSize,
                  0.0f, 0.0f, 0.0f);
    
    for (u32 Y = 0; Y < Well->Height; Y++)
    {
        u32 CellPosY = Well->FieldPosY - Y * Well->CellSideSize;
        for (u32 X = 0; X < Well->Width; X++)
        {
            if (Well->Field[Y * Well->Width + X])
            {
                u32 CellPosX = Well->FieldPosX + X * Well->CellSideSize;
                
                DrawRectangle(Buffer, 
                              CellPosX, CellPosY,
                              Well->CellSideSize, Well->CellSideSize,
                              META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
            }
        }
    }
}

internal void SwitchBlock(well *Well, u16 TetroX, u16 TetroY)
{
    assert(Well->Height > TetroY);
    
    TetroY = Well->Height - TetroY - 1;
    
    if(Well->Field[TetroY * Well->Width + TetroX] == BLOCK_STATE_EMPTY)
    {
        Well->Field[TetroY * Well->Width + TetroX] = BLOCK_STATE_TETRO;
    }
    else if(Well->Field[TetroY * Well->Width + TetroX] == BLOCK_STATE_TETRO)
    {
        Well->Field[TetroY * Well->Width + TetroX] = BLOCK_STATE_EMPTY;
    }
}

internal b32 IsBlockFilled(well *Well, u16 TetroX, u16 TetroY)
{
    assert(Well->Height > TetroY);
    TetroY = Well->Height - TetroY - 1;
    return Well->Field[TetroY * Well->Width + TetroX] != BLOCK_STATE_EMPTY;
}

inline void SetWellBlockState(well *Well, blocks_states State, u16 PosX, u16 PosY)
{
    assert(PosX < Well->Width);
    assert(PosY < Well->Height);
    assert(PosX >= 0 && PosY >= 0);
    
    PosY = Well->Height - PosY - 1;
    Well->Field[PosY * Well->Width + PosX] = State;
}

#ifdef _GAME_INTERNAL
#include "debug.cpp"
#endif

GAME_UPDATE_AND_RENDER(UpdateAndRender)
{
    assert(sizeof(game_state) < Memory->PermanentStorageSize);
    
    // NOTE(annad): Init.
    // NOTE(saiel): Objectively, this UB.
    game_state *State = (game_state *)Memory->PermanentStorage;
    well *Well = &State->Well;
    
    
    if(State->Initialized != (b32)true)
    {
        State->MetaPixelSize = Buffer->Height / 55;
        
        Well->CellSideSize = State->MetaPixelSize * 3;
        Well->Width = WELL_WIDTH;
        Well->Height = WELL_HEIGHT;
        Well->FullWidth = Well->Width + 2;
        Well->FullHeight = Well->Height + 2;
        Well->PosX = Buffer->Width / 2 - (Well->FullWidth * Well->CellSideSize / 2);
        Well->PosY = Well->FullHeight * Well->CellSideSize;
        Well->FieldPosX = Well->PosX + Well->CellSideSize;
        Well->FieldPosY = Well->PosY - Well->CellSideSize;
        
        State->PosX = Buffer->Width / 2;
        State->PosY = Buffer->Height / 2;
        State->PlayerSize = State->MetaPixelSize;
        
        State->TetroState = TETRO_STATE_SPAWN;
        State->TetrominoType = TETROMINO_BASE;
        State->TetrominoPosXInWell = Well->Width / 2;
        State->TetrominoPosYInWell = Well->Height - 1;
        State->TetrimoDownTime = 0;
        
        State->Initialized = true;
    }
    
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
            if(State->TetrominoPosXInWell > 0)
            {
                SetWellBlockState(Well, BLOCK_STATE_EMPTY, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                State->TetrominoPosXInWell -= 1;
                SetWellBlockState(Well, BLOCK_STATE_TETRO, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
            }
        }
        else if(Input->PressedKey == KEY_RIGHT)
        {
            if(State->TetrominoPosXInWell < Well->Width - 1)
            {
                SetWellBlockState(Well, BLOCK_STATE_EMPTY, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                State->TetrominoPosXInWell += 1;
                SetWellBlockState(Well, BLOCK_STATE_TETRO, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
            }
        }
        else if(Input->PressedKey == KEY_SPACE)
        {
            State->PlayerSize += 1;
        }
    }
    
    // DEBUG_CheckWell(Well, Time);
    
    /*
    for (u32 Index = 0; Index < Well->Height * Well->Width; Index++)
    {
        Well->Field[Index] = false;
    }
    */
    /*
    switch(State->TetroState)
    {
        case TETRO_STATE_SPAWN:
        {
            SwitchBlock(Well, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
            State->TetroState = TETRO_STATE_IN_PROGRESS;
            break;
        }
        
        case TETRO_STATE_IN_PROGRESS:
        {
            if(State->TetrominoPosYInWell <= 0 || IsBlockFilled(Well, State->TetrominoPosXInWell, State->TetrominoPosYInWell - 1))
            {
                State->TetroState = TETRO_STATE_FALL;
            }
            else
            {
                SwitchBlock(Well, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                State->TetrominoPosYInWell -= 1;
                SwitchBlock(Well, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
            }
            
            break;
        }
        
        case TETRO_STATE_FALL:
        {
            SwitchBlock(Well, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
            Well->Field[State->TetrominoPosYInWell * Well->Width + State->TetrominoPosXInWell] = BLOCK_STATE_FILLED;
            State->TetrominoPosYInWell = Well->Height - 1;
            State->TetroState = TETRO_STATE_SPAWN;
            break;
        }
        
        default:
        {
            assert(1 != 1);
            break;
        }
    }
    */
    RenderWell(Buffer, Well);
    
    
    // DrawRectangle(Buffer, State->PosX, State->PosY, State->PlayerSize, State->PlayerSize, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    
    DEBUG_CheckAllPositions(Buffer, Time, Well->CellSideSize);
    DEBUG_DrawGrid(Buffer, Well->CellSideSize);
    // DrawRectangle(Buffer, 0, Buffer->Height, 50, 50, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_DrawELT(Buffer);
}

