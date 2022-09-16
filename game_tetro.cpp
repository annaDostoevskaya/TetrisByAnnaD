/* 
Author: github.com/annadostoevskaya
File: game_tetro.cpp
Date: August 20th 2022 0:48 am 

Description: <empty>
*/

#include "game_tetro.h"

#include <time.h>
#include <stdlib.h>

internal void InitTetro(tetro *Tetro)
{
    srand((unsigned int)time(NULL));
    
    Tetro->State = TETRO_STATE_SPAWN;
    
    i8Vec2 DefaultTetrominos[TETRO_TOTAL][MAX_TETRO_SIZE] = {
        {{0, 0}, {1, 0}, {0, 1}, {1, 1}}, // O?..
        {{0, 1}, {-1, 0}, {0, 0}, {1, 0}}, // T
        {{1, 1}, {-1, 0}, {0, 0}, {1, 0}}, // L
        {{-1, 1}, {-1, 0}, {0, 0}, {1, 0}}, // J
        {{0, 1}, {1, 1}, {0, 0}, {-1, 0}}, // Z
        {{-1, 1}, {0, 1}, {0, 0}, {1, 0}}, // S
        {{-1, 0}, {0, 0}, {1, 0}, {2, 0}} // I
    };
    
    // DEV: static_assert(sizeof(DefaultTetrominos) == (SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE * TETRO_TOTAL));
    // DEV: static_assert(sizeof(Tetro->Tetrominos) == sizeof(DefaultTetrominos));
    SIMDMemoryCopy((void*)(&Tetro->Tetrominos), (void*)(&DefaultTetrominos), sizeof(DefaultTetrominos));
    
    Tetro->Content = (i8Vec2*)(&Tetro->ContentBuffers[0]);
    Tetro->ShadowContent = (i8Vec2*)(&Tetro->ContentBuffers[1]);
    
    Tetro->Pos = (&Tetro->PosBuffers[0]);
    Tetro->ShadowPos = (&Tetro->PosBuffers[1]);
    
    tetro_type TypesBag[TETRO_TOTAL] = {
        TETRO_O, TETRO_T, TETRO_L,
        TETRO_J, TETRO_Z, TETRO_S, TETRO_I,
    };
    
    // DEV: static_assert(sizeof(Tetro->TypesBag) == sizeof(TypesBag)); 
    SIMDMemoryCopy((void*)(&Tetro->TypesBag), (void*)(&TypesBag), sizeof(TypesBag));
    Tetro->TypesBagSize = TETRO_TOTAL;
}

internal i64 GetRandomNumber(i64 Max)
{
    i64 Result = (rand() % Max);
    return Result;
}

internal tetro_type GetRandmonTetro(tetro_type *TetroTypesBag, u32 *TetroTypesBagSize)
{
    if(*TetroTypesBagSize == 0)
    {
        *TetroTypesBagSize = TETRO_TOTAL;
    }
    
    u32 RandomIndex = (u32)GetRandomNumber(*TetroTypesBagSize);
    tetro_type RandomType = TetroTypesBag[RandomIndex];
    TetroTypesBag[RandomIndex] = TetroTypesBag[*TetroTypesBagSize - 1];
    TetroTypesBag[*TetroTypesBagSize - 1] = RandomType;
    
    *TetroTypesBagSize -= 1;
    
    return RandomType;
}

internal void ChangeStateBlocksTetro(well *Well, block_state BlockState, i8Vec2 *TetroContent, i8Vec2 *TetroPos)
{
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        i16Vec2 GlobalPos = {};
        GlobalPos.X = TetroPos->X + CurrentPart->X;
        GlobalPos.Y = TetroPos->Y - CurrentPart->Y;
        SetWellBlockState(Well, BlockState, GlobalPos);
    }
}

internal void DrawTetro(well *Well, i8Vec2 *TetroContent, i8Vec2 *TetroPos)
{
    ChangeStateBlocksTetro(Well, BLOCK_STATE_TETRO, TetroContent, TetroPos);
}

internal void RemoveTetro(well *Well, i8Vec2 *TetroContent, i8Vec2 *TetroPos)
{
    ChangeStateBlocksTetro(Well, BLOCK_STATE_EMPTY, TetroContent, TetroPos);
}

internal void DropTetro(well *Well, i8Vec2 *TetroContent, i8Vec2 *TetroPos)
{
    ChangeStateBlocksTetro(Well, BLOCK_STATE_FILLED, TetroContent, TetroPos);
}

internal void MoveTetro(well *Well, i8Vec2 *TetroPos, i8Vec2 MoveVector)
{
    TetroPos->X += MoveVector.X;
    TetroPos->Y += MoveVector.Y;
}

internal void RotateTetro(well *Well, i8Vec2 *TetroContent, i32 *RotateMatrix)
{
    i32 Result[1][2] = {};
    
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        
        i32 Position[1][2] = {
            {CurrentPart->X, CurrentPart->Y}
        };
        
        MultiplyMatrices((i32*)Position, (i32*)RotateMatrix, 2, 2, 1, (i32*)Result);
        
        CurrentPart->X = (i8)Result[0][0];
        CurrentPart->Y = (i8)Result[0][1];
    }
}

internal b32 CheckCollideTetro(well *Well, i8Vec2 *TetroContent, i8Vec2 *TetroPos)
{
    for(u32 i = 0; i < MAX_TETRO_SIZE; i++)
    {
        i8Vec2 *CurrentPart = &TetroContent[i];
        i16Vec2 GlobalPos = {};
        GlobalPos.X = TetroPos->X + CurrentPart->X;
        GlobalPos.Y = TetroPos->Y - CurrentPart->Y;
        if(GlobalPos.Y < 0 || GlobalPos.X < 0 || 
           GlobalPos.X >= Well->Width || GlobalPos.Y >= Well->Height || 
           WellBlockIsFilled(Well, GlobalPos))
        {
            return true;
        }
    }
    
    return false;
}

internal void UpdateTetroPos(well *Well, tetro *Tetro)
{
    RemoveTetro(Well, Tetro->Content, Tetro->Pos);
    AddressSwap((void**)(&Tetro->Pos), (void**)(&Tetro->ShadowPos));
    DrawTetro(Well, Tetro->Content, Tetro->Pos);
}

internal void UpdateTetroContent(well *Well, tetro *Tetro)
{
    RemoveTetro(Well, Tetro->Content, Tetro->Pos);
    AddressSwap((void**)(&Tetro->Content), (void**)(&Tetro->ShadowContent));
    DrawTetro(Well, Tetro->Content, Tetro->Pos);
}

internal void UpdateTetro(game_state *State)
{
    tetro *Tetro = &(State->Tetro);
    well *Well = &(State->Well);
    midi *Midi = &(State->Midi);
    
    switch(Tetro->State)
    {
        case TETRO_STATE_SPAWN:
        {
            State->Score += MAX_TETRO_SIZE;
            
            Tetro->Pos->X = Tetro->ShadowPos->X = Well->Width / 2;
            Tetro->Pos->Y = Tetro->ShadowPos->Y = Well->Height - 1;
            
            Tetro->Type = GetRandmonTetro((tetro_type*)(&Tetro->TypesBag), &Tetro->TypesBagSize);
            
            // NOTE(saiel): memcpy() fast then copy a memory, because use SIMD. It's partly hardware thing.
            SIMDMemoryCopy((void*)(&Tetro->ContentBuffers[0]), (void*)(&Tetro->Tetrominos[Tetro->Type]), sizeof(i8Vec2) * MAX_TETRO_SIZE);
            SIMDMemoryCopy((void*)(&Tetro->ContentBuffers[1]), (void*)(&Tetro->ContentBuffers[0]), sizeof(i8Vec2) * MAX_TETRO_SIZE);
            
            if(CheckCollideTetro(Well, Tetro->Content, Tetro->Pos))
            {
                State->Fail = true;
                break;
            }
            
            DrawTetro(Well, Tetro->Content, Tetro->Pos);
            
            Tetro->State = TETRO_STATE_IN_PROGRESS;
            break;
        }
        
        case TETRO_STATE_IN_PROGRESS:
        {
            // DEV: assert(Tetro->Pos->Y >= 0);
            
            i8Vec2 MoveVector = {0, -1};
            *Tetro->ShadowPos = *Tetro->Pos;
            MoveTetro(Well, Tetro->ShadowPos, MoveVector);
            
            if(!CheckCollideTetro(Well, Tetro->Content, Tetro->ShadowPos))
            {
                UpdateTetroPos(Well, Tetro);
                break;
            }
            
            Tetro->State = TETRO_STATE_FALL;
            // NOTE(saiel): We can remove this, сause we're falling apart
            // break
        }
        
        case TETRO_STATE_FALL:
        {
            // NOTE(annad): audio
            Midi->Hz = 450;
            Midi->Duration = 25;
            Midi->Volume = 0.5f;
            
            DropTetro(Well, Tetro->Content, Tetro->Pos);
            Tetro->State = TETRO_STATE_SPAWN;
            break;
        }
    }
}

