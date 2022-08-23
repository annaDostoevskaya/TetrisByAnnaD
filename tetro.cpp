/* 
Author: github.com/annadostoevskaya
File: tetro.cpp
Date: August 20th 2022 0:48 am 

Description: <empty>
*/

#include "tetro.h"

//
// matrix.cpp
//

void MultiplyMatrices(i32 *M1, i32 *M2, 
                      u32 RowsAndColumnSizeForM1AndM2,  // NOTE(annad): Rows size is Colum and Column size is Rows?... idk...
                      u32 ColumnsSizeM1, u32 RowsSizeM2, i32 *Dest)
{
    // TODO(annad): We must rewrite it.
    for(u32 i = 0; i < ColumnsSizeM1; i++)
    {
        for(u32 j = 0; j < RowsSizeM2; j++)
        {
            Dest[i * RowsSizeM2 + j] = 0;
            for(u32 k = 0; k < RowsAndColumnSizeForM1AndM2; k++)
            {
                Dest[i * RowsSizeM2 + j] += (M1[i * RowsAndColumnSizeForM1AndM2 + k] * 
                                             M2[k * RowsAndColumnSizeForM1AndM2 + j]);
            }
        }
    }
}

//
// matrix.cpp
//

//
// algs.cpp
//

void AddressSwap(void **Addr1, void **Addr2)
{
    void *temp = *Addr1;
    *Addr1 = *Addr2;
    *Addr2 = temp;
}

//
// algs.cpp
//

//
// rand.cpp
//

u32 GetRandomNumber(u32 Min, u32 Max)
{
    localv u32 Index = 0;
    localv u32 RADNMON_NUMBERS[100] = {
        3, 1, 6, 4, 4, 3, 
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

//
// rand.cpp
//

void ChangeStateBlocksTetro(well *Well, block_state BlockState, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
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
        {1, 0, 0},
        {0, 1, 0},
        {MoveVector.X, MoveVector.Y, 1}
    };
    
    i32 Result[1][3] = {};
    
    MultiplyMatrices((i32*)Position, (i32*)TranstionMatrix, 3, 1, 3, (i32*)Result);
    
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
        
        MultiplyMatrices((i32*)Position, (i32*)RotateMatrix, 2, 1, 2, (i32*)Result);
        
        CurrentPart->X = (i8)Result[0][0];
        CurrentPart->Y = (i8)Result[0][1];
    }
}

b32 CheckCollideTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos)
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
    // NOTE(saiel): Move this to top.
    if(Tetro->AccumTime < Tetro->DownTime)
    {
        Tetro->AccumTime += Time->dt;
        return;
    }
    
    Tetro->AccumTime = 0;
    
    switch(Tetro->State)
    {
        // TODO(annad): This should be done better.
        case TETRO_STATE_SPAWN:
        {
            Tetro->Pos->X = Tetro->ShadowPos->X = Well->Width / 2;
            Tetro->Pos->Y = Tetro->ShadowPos->Y = Well->Height - 1;
            
            // TODO(annad): Radmon.
            // Tetro->Type = (tetro_type)GetRandomNumber(0, (u32)TETRO_TOTAL);
            Tetro->Type = (tetro_type)6;
            
            // TODO(saiel): We must do more strong typizatino for this and remove pointers, then remove memcpy()
            // NOTE(saiel): memcpy() fast then copy a memory, because use SIMD. It's partly hardware thing.
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
            // NOTE(saiel): We can remove this, сause we're falling apart
            // break
        }
        
        case TETRO_STATE_FALL:
        {
            DropTetro(Well, Tetro->Content, Tetro->Pos);
            Tetro->State = TETRO_STATE_SPAWN;
            break;
        }
        
#ifdef _GAME_INTERNAL
        default:
        {
            assert(1 != 1);
            break;
        }
#endif
    }
}
