/* 
Author: github.com/annadostoevskaya
File: game.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "game.h"

#include <assert.h>


//
// draw.cpp
//

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

//
// draw.cpp
//


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
    tetro* Tetro = &State->Tetro;
    
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
        
        Tetro->State = TETRO_STATE_SPAWN;
        
        // TODO(annad): Write special function for loading tetros. mb memcpy?..
        i8Vec2 DefaultTetrominos[TETRO_TOTAL][MAX_TETRO_SIZE] = {
            {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, // O?..
            {{0, 1}, {-1, 0}, {0, 0}, {1, 0}}, // T
            {{1, 1}, {-1, 0}, {0, 0}, {1, 0}}, // L
            {{-1, 1}, {-1, 0}, {0, 0}, {1, 0}}, // J
            {{0, 1}, {1, 1}, {0, 0}, {-1, 0}}, // Z
            {{-1, 1}, {0, 1}, {0, 0}, {1, 0}}, // S
            {{-1, 0}, {0, 0}, {1, 0}, {2, 0}} // I
        };
        
        memcpy((void*)(&Tetro->Tetrominos), (void*)(&DefaultTetrominos), SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE * TETRO_TOTAL);
        
        Tetro->Content = (i8Vec2*)(&Tetro->ContentBuffers[0]);
        Tetro->ShadowContent = (i8Vec2*)(&Tetro->ContentBuffers[1]);
        
        Tetro->Pos = (&Tetro->PosBuffers[0]);
        Tetro->ShadowPos = (&Tetro->PosBuffers[1]);
        
        Tetro->DownTime = 500;
        Tetro->AccumTime = Tetro->DownTime;
        
        State->Pause = false;
        State->Initialized = true;
    }
    
    // DEBUG_CheckWell(Well, Time);
    
    if(State->Pause == (b32)false)
    {
        UpdateTetro(Well, Tetro, Time);
    }
    
    RenderWell(Buffer, Well);
    
    // DrawRectangle(Buffer, State->PosX, State->PosY, State->PlayerSize, State->PlayerSize, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    
    DEBUG_CheckAllPositions(Buffer, Time, Well->CellSideSize);
    DEBUG_DrawGrid(Buffer, Well->CellSideSize);
    // DrawRectangle(Buffer, 0, Buffer->Height, 50, 50, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_DrawELT(Buffer);
    
    {
        if(!CheckCollideTetro(Well, Tetro->Content, Tetro->Pos))
        {
            // NOTE(annad): Input handling.
            if(Input->PressedKey == KEY_UP)
            {
                if(Tetro->Type != TETRO_O) // NOTE(annad): Absolutely legal crutch.
                {
                    memcpy((void*)Tetro->ShadowContent, (void*)Tetro->Content, SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                    RotateTetro(Well, Tetro->ShadowContent);
                    if(!CheckCollideTetro(Well, Tetro->ShadowContent, Tetro->Pos))
                    {
                        UpdateTetroContent(Well, Tetro);
                    }
                }
            }
            else if(Input->PressedKey == KEY_SPACE)
            {
                State->Pause = !State->Pause;
                __debugbreak();
            }
            else
            {
                i8Vec2 MoveVector = {0, 0};
                
                if(Input->PressedKey == KEY_DOWN)
                {
                    MoveVector = {0, -1};
                }
                else if(Input->PressedKey == KEY_LEFT)
                {
                    MoveVector = {-1, 0};
                }
                else if(Input->PressedKey == KEY_RIGHT)
                {
                    MoveVector = {1, 0};
                }
                
                *Tetro->ShadowPos = *Tetro->Pos;
                MoveTetro(Well, Tetro->ShadowPos, MoveVector);
                if(!CheckCollideTetro(Well, Tetro->Content, Tetro->ShadowPos))
                {
                    UpdateTetroPos(Well, Tetro);
                }
            }
            
            if(CheckCollideTetro(Well, Tetro->Content, Tetro->Pos))
            {
                State->BoolState = 1;
            }
            else
            {
                State->BoolState = 0;
            }
            
            DEBUG_BoolInScreen(Buffer, State->BoolState);
        }
    }
}


#include "sdl_game.cpp"
#include "well.cpp"
#include "tetro.cpp"