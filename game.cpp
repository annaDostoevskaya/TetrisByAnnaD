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

#define WHITE_COLOR_RGB 0.93f
#define META_PIXEL_COLOR 0.93f

#include <assert.h>

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

#include "well.cpp"

#ifdef _GAME_INTERNAL
#include "debug.cpp"
#include <string.h>
#endif

void DrawTetro(well *Well, blocks_states BlockState, u8 *TetroContent, u16 TetroPosX, u16 TetroPosY)
{
    for(u16 RelY = 0; RelY < TETRO_MAX_HEIGHT; RelY += 1)
    {
        u16 GlobalY = TetroPosY - RelY;
        for(u16 RelX = 0; RelX < TETRO_MAX_WIDTH; RelX += 1)
        {
            u16 GlobalX = TetroPosX + RelX;
            if(TetroContent[RelY * TETRO_MAX_WIDTH + RelX])
            {
                SetWellBlockState(Well, BlockState, GlobalX, GlobalY);
            }
        }
    }
}

GAME_UPDATE_AND_RENDER(UpdateAndRender)
{
    assert(sizeof(game_state) < Memory->PermanentStorageSize);
    
    // NOTE(annad): Init.
    // NOTE(saiel): Objectively, this UB.
    game_state *State = (game_state *)Memory->PermanentStorage;
    well *Well = &State->Well;
    
    if(State->Initialized != (b32)true)
    {
#ifdef _GAME_INTERNAL
        State->BoolState = 1;
#endif
        
        State->MetaPixelSize = Buffer->Height / 55;
        
        Well->CellSideSize = State->MetaPixelSize * 3;
        Well->Width = WELL_WIDTH;
        Well->Height = WELL_HEIGHT;
        Well->FullWidth = Well->Width + 2;
        Well->FullHeight = Well->Height + 2;
        
        Well->Pos.X = Buffer->Width / 2 - (Well->FullWidth * Well->CellSideSize / 2);
        Well->Pos.Y = Well->FullHeight * Well->CellSideSize;
        
        Well->FieldPos.X = Well->Pos.X + Well->CellSideSize;
        Well->FieldPos.Y = Well->Pos.Y - Well->CellSideSize;
        
        State->TetroState = TETRO_STATE_SPAWN;
        
        // TODO(annad): Write special function for loading tetros. mb memcpy?..
        u8 TetroDefault[TETRO_MAX_WIDTH * TETRO_MAX_HEIGHT] = {
            0, 1, 0, 0,
            0, 1, 0, 0,
            1, 1, 1, 0
        };
        memcpy((void*)State->TetroParts, (void*)TetroDefault, TETRO_MAX_WIDTH * TETRO_MAX_HEIGHT);
        
        State->TetrominoPosXInWell = Well->Width / 2;
        State->TetrominoPosYInWell = Well->Height - 1;
        State->TetrimoDownTime = 0;
        
        State->Initialized = true;
    }
    
    if(WellBlockIs(Well, BLOCK_STATE_TETRO, State->TetrominoPosXInWell, State->TetrominoPosYInWell))
    {
        State->BoolState = 1;
    }
    else
    {
        State->BoolState = 0;
    }
    
    // DEBUG_CheckWell(Well, Time);
    
    localv u32 Accum = 0;
    if(Accum >= 500)
    {
        switch(State->TetroState)
        {
            case TETRO_STATE_SPAWN:
            {
                // it is center of drawing
                State->TetrominoPosYInWell = Well->Height - 1;
                State->TetrominoPosXInWell = Well->Width / 2;
                
                DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroParts, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                
                State->TetroState = TETRO_STATE_IN_PROGRESS;
                break;
            }
            
            case TETRO_STATE_IN_PROGRESS:
            {
                assert(State->TetrominoPosYInWell >= 0);
                if(State->TetrominoPosYInWell == 4 || WellBlockIsFilled(Well, State->TetrominoPosXInWell, State->TetrominoPosYInWell - 1)) // TODO(annad): is block filled?...
                {
                    State->TetroState = TETRO_STATE_FALL;
                }
                else
                {
                    DrawTetro(Well, BLOCK_STATE_EMPTY, State->TetroParts, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                    State->TetrominoPosYInWell -= 1;
                    DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroParts, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                }
                
                break;
            }
            
            case TETRO_STATE_FALL:
            {
                DrawTetro(Well, BLOCK_STATE_FILLED, State->TetroParts, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                State->TetroState = TETRO_STATE_SPAWN;
                break;
            }
            
            default:
            {
                assert(1 != 1);
                break;
            }
        }
        Accum = 0;
    }
    
    Accum += Time->dt;
    
    RenderWell(Buffer, Well);
    
    // DrawRectangle(Buffer, State->PosX, State->PosY, State->PlayerSize, State->PlayerSize, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    
    DEBUG_CheckAllPositions(Buffer, Time, Well->CellSideSize);
    DEBUG_DrawGrid(Buffer, Well->CellSideSize);
    // DrawRectangle(Buffer, 0, Buffer->Height, 50, 50, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_DrawELT(Buffer);
    
    {
        if(WellBlockIs(Well, BLOCK_STATE_TETRO, State->TetrominoPosXInWell, State->TetrominoPosYInWell))
        {
            // NOTE(annad): Input handling.
            if(Input->PressedKey == KEY_UP)
            {
                ;
            }
            else if(Input->PressedKey == KEY_DOWN)
            {
                if(State->TetrominoPosYInWell == 0 || WellBlockIsFilled(Well, State->TetrominoPosXInWell, State->TetrominoPosYInWell - 1)) // TODO(annad): is block filled?...
                {
                    State->TetroState = TETRO_STATE_FALL;
                }
                else
                {
                    SetWellBlockState(Well, BLOCK_STATE_EMPTY, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                    State->TetrominoPosYInWell -= 1;
                    SetWellBlockState(Well, BLOCK_STATE_TETRO, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                }
            }
            else if(Input->PressedKey == KEY_LEFT)
            {
                if(State->TetrominoPosXInWell > 0 && !WellBlockIsFilled(Well, State->TetrominoPosXInWell - 1, State->TetrominoPosYInWell))
                {
                    SetWellBlockState(Well, BLOCK_STATE_EMPTY, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                    State->TetrominoPosXInWell -= 1;
                    SetWellBlockState(Well, BLOCK_STATE_TETRO, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                }
            }
            else if(Input->PressedKey == KEY_RIGHT)
            {
                if(State->TetrominoPosXInWell < Well->Width - 1 && !WellBlockIsFilled(Well, State->TetrominoPosXInWell + 1, State->TetrominoPosYInWell))
                {
                    SetWellBlockState(Well, BLOCK_STATE_EMPTY, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                    State->TetrominoPosXInWell += 1;
                    SetWellBlockState(Well, BLOCK_STATE_TETRO, State->TetrominoPosXInWell, State->TetrominoPosYInWell);
                }
            }
            else if(Input->PressedKey == KEY_SPACE)
            {
                __debugbreak();
            }
        }
    }
    
    DEBUG_BoolInScreen(Buffer, State->BoolState);
}

