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

void MultiplyMatrixes(i32 *M1, i32 *M2, u32 RowsAndColumnM1AndM2, u32 ColumnsM1, u32 RowsM2, i32 *Dest)
{
    for(u32 i = 0; i < ColumnsM1; i++)
    {
        for(u32 j = 0; j < RowsM2; j++)
        {
            Dest[i * RowsM2 + j] = 0;
            for(u32 k = 0; k < RowsAndColumnM1AndM2; k++)
            {
                Dest[i * RowsM2 + j] += (M1[i * RowsAndColumnM1AndM2 + k] * M2[k * RowsAndColumnM1AndM2 + j]);
            }
        }
    }
}

void DrawTetro(well *Well, blocks_states BlockState, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        i16 GlobalX = TetroPos->X + CurrentPart->X;
        i16 GlobalY = TetroPos->Y - CurrentPart->Y;
        SetWellBlockState(Well, BlockState, GlobalX, GlobalY);
    }
}

b32 CheckTetroControl(well* Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        i16 GlobalX = TetroPos->X + CurrentPart->X;
        i16 GlobalY = TetroPos->Y - CurrentPart->Y;
        if(!WellBlockIs(Well, BLOCK_STATE_TETRO, GlobalX, GlobalY))
            return false;
    }
    
    return true;
}

void MoveTetro(well *Well, i16Vec2 *TetroPos, i8Vec2 MoveVector)
{
    i32 Position[1][3] = {
        {TetroPos->X, TetroPos->Y, 1}
    };
    
    i32 TranstionMatrix[3][3] = {
        1, 0, 0, 
        0, 1, 0, 
        MoveVector.X, MoveVector.Y, 1, 
    };
    
    i32 Result[1][3] = {};
    
    MultiplyMatrixes((i32*)Position, (i32*)TranstionMatrix, 3, 1, 3, (i32*)Result);
    
    TetroPos->X = (u16)Result[0][0];
    TetroPos->Y = (u16)Result[0][1];
}

void RotateTetro(well *Well, i8Vec2 *TetroContent)
{
    i32 RotateMatrix[2][2] = {
        {0, -1},
        {1, 0}
    };
    
    i32 Result[1][2] = {};
    
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        
        i32 Position[1][2] = {
            {CurrentPart->X, CurrentPart->Y}
        };
        
        MultiplyMatrixes((i32*)Position, (i32*)RotateMatrix, 2, 1, 2, (i32*)Result);
        
        CurrentPart->X = (i8)Result[0][0];
        CurrentPart->Y = (i8)Result[0][1];
    }
}

b32 CheckTetroCollide(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        i16 GlobalX = TetroPos->X + CurrentPart->X;
        i16 GlobalY = TetroPos->Y - CurrentPart->Y;
        if(GlobalY < 0 || GlobalX < 0 || GlobalX >= Well->Width || GlobalY >= Well->Height || WellBlockIsFilled(Well, GlobalX, GlobalY))
        {
            return true;
        }
    }
    
    return false;
}

void AddressSwap(void **Addr1, void **Addr2)
{
    void *temp = *Addr1;
    *Addr1 = *Addr2;
    *Addr2 = temp;
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
        i8Vec2 TetroDefault[MAX_TETRO_SIZE] = {
            {0, 1}, {1, 1}, {2, 1}, {1, 0}
        };
        
        memcpy((void*)(&State->TetroContentBuffers[0]), (void*)TetroDefault, sizeof(i8Vec2) * MAX_TETRO_SIZE);
        memcpy((void*)(&State->TetroContentBuffers[1]), (void*)TetroDefault, sizeof(i8Vec2) * MAX_TETRO_SIZE);
        State->TetroContent = (i8Vec2*)(&State->TetroContentBuffers[0]);
        State->TetroShadowContent = (i8Vec2*)(&State->TetroContentBuffers[1]);
        
        State->TetroPos = (&State->TetroPosBuffers[0]);
        State->TetroShadowPos = (&State->TetroPosBuffers[1]);
        State->TetroPos->X = State->TetroShadowPos->X = Well->Width / 2;
        State->TetroPos->Y = State->TetroShadowPos->Y = Well->Height - 1;
        
        State->TetrimoDownTime = 0;
        
        State->Pause = false;
        
        State->Initialized = true;
    }
    
    if(CheckTetroControl(Well, State->TetroContent, State->TetroPos))
    {
        State->BoolState = 1;
    }
    else
    {
        State->BoolState = 0;
    }
    
    // DEBUG_CheckWell(Well, Time);
    localv u32 Accum = 0;
    
    if(State->Pause == (b32)false)
    {
        if(Accum >= 500)
        {
            switch(State->TetroState)
            {
                // TODO(annad): This should be done better.
                case TETRO_STATE_SPAWN:
                {
                    // it is center of drawing
                    State->TetroPos->X = State->TetroShadowPos->X = Well->Width / 2;
                    State->TetroPos->Y = State->TetroShadowPos->Y = Well->Height - 1;
                    
                    // TODO(annad): Write special function for loading tetros. mb memcpy?..
                    i8Vec2 TetroDefault[MAX_TETRO_SIZE] = {
                        {0, 1}, {1, 1}, {2, 1}, {1, 0}
                    };
                    
                    memcpy((void*)(&State->TetroContentBuffers[0]), (void*)TetroDefault, sizeof(i8Vec2) * MAX_TETRO_SIZE);
                    memcpy((void*)(&State->TetroContentBuffers[1]), (void*)TetroDefault, sizeof(i8Vec2) * MAX_TETRO_SIZE);
                    State->TetroContent = (i8Vec2*)(&State->TetroContentBuffers[0]);
                    State->TetroShadowContent = (i8Vec2*)(&State->TetroContentBuffers[1]);
                    
                    DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroContent, State->TetroPos);
                    
                    State->TetroState = TETRO_STATE_IN_PROGRESS;
                    break;
                }
                
                case TETRO_STATE_IN_PROGRESS:
                {
                    assert(State->TetroPos->Y >= 0);
                    
                    i8Vec2 MoveVector = {0, -1};
                    *State->TetroShadowPos = *State->TetroPos;
                    MoveTetro(Well, State->TetroShadowPos, MoveVector);
                    
                    if(CheckTetroCollide(Well, State->TetroContent, State->TetroShadowPos))
                    {
                        State->TetroState = TETRO_STATE_FALL;
                    }
                    else
                    {
                        DrawTetro(Well, BLOCK_STATE_EMPTY, State->TetroContent, State->TetroPos);
                        AddressSwap((void**)&State->TetroPos, (void**)&State->TetroShadowPos);
                        DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroContent, State->TetroPos);
                        break;
                    }
                }
                
                case TETRO_STATE_FALL:
                {
                    DrawTetro(Well, BLOCK_STATE_FILLED, State->TetroContent, State->TetroPos);
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
    }
    
    RenderWell(Buffer, Well);
    
    // DrawRectangle(Buffer, State->PosX, State->PosY, State->PlayerSize, State->PlayerSize, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    
    DEBUG_CheckAllPositions(Buffer, Time, Well->CellSideSize);
    DEBUG_DrawGrid(Buffer, Well->CellSideSize);
    // DrawRectangle(Buffer, 0, Buffer->Height, 50, 50, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_DrawELT(Buffer);
    
    {
        if(CheckTetroControl(Well, State->TetroContent, State->TetroPos))
        {
            // NOTE(annad): Input handling.
            if(Input->PressedKey == KEY_UP)
            {
                memcpy((void*)State->TetroShadowContent, (void*)State->TetroContent, sizeof(i8Vec2) * MAX_TETRO_SIZE);
                RotateTetro(Well, State->TetroShadowContent);
                
                if(!CheckTetroCollide(Well, State->TetroShadowContent, State->TetroPos))
                {
                    DrawTetro(Well, BLOCK_STATE_EMPTY, State->TetroContent, State->TetroPos);
                    AddressSwap((void**)&State->TetroContent, (void**)&State->TetroShadowContent);
                    DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroContent, State->TetroPos);
                }
            }
            else if(Input->PressedKey == KEY_DOWN)
            {
                assert(State->TetroPos->Y >= 0);
                
                i8Vec2 MoveVector = {0, -1};
                *State->TetroShadowPos = *State->TetroPos;
                MoveTetro(Well, State->TetroShadowPos, MoveVector);
                
                if(CheckTetroCollide(Well, State->TetroContent, State->TetroShadowPos))
                {
                    State->TetroState = TETRO_STATE_FALL;
                }
                else
                {
                    DrawTetro(Well, BLOCK_STATE_EMPTY, State->TetroContent, State->TetroPos);
                    AddressSwap((void**)&State->TetroPos, (void**)&State->TetroShadowPos);
                    DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroContent, State->TetroPos);
                }
            }
            else if(Input->PressedKey == KEY_LEFT)
            {
                i8Vec2 MoveVector = {-1, 0};
                *State->TetroShadowPos = *State->TetroPos;
                MoveTetro(Well, State->TetroShadowPos, MoveVector);
                
                if(!CheckTetroCollide(Well, State->TetroContent, State->TetroShadowPos))
                {
                    DrawTetro(Well, BLOCK_STATE_EMPTY, State->TetroContent, State->TetroPos);
                    AddressSwap((void**)&State->TetroPos, (void**)&State->TetroShadowPos);
                    DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroContent, State->TetroPos);
                }
            }
            else if(Input->PressedKey == KEY_RIGHT)
            {
                i8Vec2 MoveVector = {1, 0};
                *State->TetroShadowPos = *State->TetroPos;
                MoveTetro(Well, State->TetroShadowPos, MoveVector);
                
                if(!CheckTetroCollide(Well, State->TetroContent, State->TetroShadowPos))
                {
                    DrawTetro(Well, BLOCK_STATE_EMPTY, State->TetroContent, State->TetroPos);
                    AddressSwap((void**)&State->TetroPos, (void**)&State->TetroShadowPos);
                    DrawTetro(Well, BLOCK_STATE_TETRO, State->TetroContent, State->TetroPos);
                }
            }
            else if(Input->PressedKey == KEY_SPACE)
            {
                State->Pause = !State->Pause;
                __debugbreak();
            }
        }
    }
    
    DEBUG_BoolInScreen(Buffer, State->BoolState);
}

