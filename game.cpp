/* 
Author: github.com/annadostoevskaya
File: game.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "base_types.h"
#include "game.h"
#include "sdl_game.h"

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

#include "sdl_game.cpp"
#include "well.cpp"

#ifdef _GAME_INTERNAL
#include "debug.cpp"
#include <string.h>
#endif

//
//
//

void AddressSwap(void **Addr1, void **Addr2)
{
    void *temp = *Addr1;
    *Addr1 = *Addr2;
    *Addr2 = temp;
}


u32 GetRandomNumber(u32 Min, u32 Max)
{
    localv u32 Index = 0;
    localv u32 RADNMON_NUMBERS[100] = {
        3, 1, 6, 5, 4, 3, 
        3, 4, 1, 4, 5, 5, 
        0, 2, 0, 0, 3, 5, 
        5, 1, 6, 6, 3, 5, 
        4, 2, 4, 1, 2, 6, 
        1, 6, 1, 3, 0, 4, 
        5, 2, 4, 6, 0, 2, 
        2, 2, 1, 5, 5, 3, 
        2, 6, 4, 3, 2, 3, 
        0, 4, 2, 0, 1, 0, 
        5, 5, 0, 5, 4, 6, 
        2, 1, 2, 6, 6, 5, 
        2, 6, 5, 0, 2, 1, 
        5, 1, 0, 1, 5, 1, 
        4, 2, 6, 2, 4, 4, 
        6, 0, 6, 2, 6, 2, 
        0, 6, 4, 3
    };
    
    Index++;
    if(Index == 100) 
        Index = 0;
    
    u32 Result = RADNMON_NUMBERS[Index];
    
    return Result;
}


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

//
//
//

void ChangeStateBlocksTetro(well *Well, block_state BlockState, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        i16 GlobalX = TetroPos->X + CurrentPart->X;
        i16 GlobalY = TetroPos->Y - CurrentPart->Y;
        SetWellBlockState(Well, BlockState, GlobalX, GlobalY);
    }
}

void DrawTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    ChangeStateBlocksTetro(Well, BLOCK_STATE_TETRO, TetroContent, TetroPos);
}

void RemoveTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    ChangeStateBlocksTetro(Well, BLOCK_STATE_EMPTY, TetroContent, TetroPos);
}

void DropTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    ChangeStateBlocksTetro(Well, BLOCK_STATE_FILLED, TetroContent, TetroPos);
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

b32 CheckCollideTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
{
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        i16 GlobalX = TetroPos->X + CurrentPart->X;
        i16 GlobalY = TetroPos->Y - CurrentPart->Y;
        if(GlobalY < 0 || GlobalX < 0 || 
           GlobalX >= Well->Width || GlobalY >= Well->Height || 
           WellBlockIsFilled(Well, GlobalX, GlobalY))
        {
            return true;
        }
    }
    
    return false;
}

void UpdateTetroPos(well *Well, tetro *Tetro)
{
    RemoveTetro(Well, Tetro->Content, Tetro->Pos);
    AddressSwap((void**)(&Tetro->Pos), (void**)(&Tetro->ShadowPos));
    DrawTetro(Well, Tetro->Content, Tetro->Pos);
}

void UpdateTetroContent(well *Well, tetro *Tetro)
{
    RemoveTetro(Well, Tetro->Content, Tetro->Pos);
    AddressSwap((void**)(&Tetro->Content), (void**)(&Tetro->ShadowContent));
    DrawTetro(Well, Tetro->Content, Tetro->Pos);
}

void UpdateTetro(well *Well, tetro *Tetro, game_time *Time)
{
    
    if(Tetro->AccumTime >= Tetro->DownTime)
    {
        switch(Tetro->State)
        {
            // TODO(annad): This should be done better.
            case TETRO_STATE_SPAWN:
            {
                Tetro->Pos->X = Tetro->ShadowPos->X = Well->Width / 2;
                Tetro->Pos->Y = Tetro->ShadowPos->Y = Well->Height - 1;
                
                // TODO(annad): Radmon.
                Tetro->Type = (tetro_type)GetRandomNumber(0, (u32)TETRO_TOTAL);
                
                memcpy((void*)(&Tetro->ContentBuffers[0]), (void*)(&Tetro->Tetrominos[Tetro->Type]), sizeof(i8Vec2) * MAX_TETRO_SIZE);
                memcpy((void*)(&Tetro->ContentBuffers[1]), (void*)(&Tetro->ContentBuffers[0]), sizeof(i8Vec2) * MAX_TETRO_SIZE);
                
                DrawTetro(Well, Tetro->Content, Tetro->Pos);
                
                Tetro->State = TETRO_STATE_IN_PROGRESS;
                break;
            }
            
            case TETRO_STATE_IN_PROGRESS:
            {
                assert(Tetro->Pos->Y >= 0);
                
                i8Vec2 MoveVector = {0, -1};
                *Tetro->ShadowPos = *Tetro->Pos;
                MoveTetro(Well, Tetro->ShadowPos, MoveVector);
                
                if(!CheckCollideTetro(Well, Tetro->Content, Tetro->ShadowPos))
                {
                    UpdateTetroPos(Well, Tetro);
                    break;
                }
                
                Tetro->State = TETRO_STATE_FALL;
                break;
            }
            
            case TETRO_STATE_FALL:
            {
                DropTetro(Well, Tetro->Content, Tetro->Pos);
                Tetro->State = TETRO_STATE_SPAWN;
                break;
            }
            
            default:
            {
                assert(1 != 1);
                break;
            }
        }
        Tetro->AccumTime = 0;
    }
    
    Tetro->AccumTime += Time->dt;
}

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
        
        // 
        //
        //
        
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
                    i8Vec2 ContentBuffer[MAX_TETRO_SIZE];
                    memcpy((void*)&ContentBuffer, (void*)Tetro->Content, SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                    // RotateTetro(Well, Tetro->ShadowContent);
                    
                    i32 RotateMatrix[2][2] = {
                        {0, -1},
                        {1, 0}
                    };
                    
                    i32 Result[1][2] = {};
                    
                    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
                    {
                        i8Vec2 *CurrentPart = &ContentBuffer[i];
                        
                        i32 Position[1][2] = {
                            {CurrentPart->X, CurrentPart->Y}
                        };
                        
                        MultiplyMatrixes((i32*)Position, (i32*)RotateMatrix, 2, 1, 2, (i32*)Result);
                        
                        CurrentPart->X = (i8)Result[0][0];
                        CurrentPart->Y = (i8)Result[0][1];
                    }
                    
                    if(!CheckCollideTetro(Well, (i8Vec2*)(&ContentBuffer), Tetro->Pos))
                    {
                        RemoveTetro(Well, Tetro->Content, Tetro->Pos);
                        memcpy((void*)Tetro->Content, (void*)&ContentBuffer, SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                        // AddressSwap((void**)(&Tetro->Content), (void**)(&Tetro->ShadowContent));
                        DrawTetro(Well, Tetro->Content, Tetro->Pos);
                        
                        // UpdateTetroContent(Well, Tetro);
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
                
                i16Vec2 BufferPos = *Tetro->Pos;
                
                i32 Position[1][3] = {
                    {BufferPos.X, BufferPos.Y, 1}
                };
                
                i32 TranstionMatrix[3][3] = {
                    1, 0, 0, 
                    0, 1, 0, 
                    MoveVector.X, MoveVector.Y, 1, 
                };
                
                i32 Result[1][3] = {};
                
                MultiplyMatrixes((i32*)Position, (i32*)TranstionMatrix, 3, 1, 3, (i32*)Result);
                
                BufferPos.X = (u16)Result[0][0];
                BufferPos.Y = (u16)Result[0][1];
                
                // MoveTetro(Well, Tetro->ShadowPos, MoveVector);
                
                if(!CheckCollideTetro(Well, Tetro->Content, &BufferPos))
                {
                    RemoveTetro(Well, Tetro->Content, Tetro->Pos);
                    // AddressSwap((void**)(&Tetro->Pos), (void**)(&Tetro->ShadowPos));
                    *Tetro->Pos = BufferPos;
                    DrawTetro(Well, Tetro->Content, Tetro->Pos);
                    // UpdateTetroPos(Well, Tetro);
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