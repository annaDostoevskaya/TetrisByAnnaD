/* 
Author: github.com/annadostoevskaya
File: well.cpp
Date: August 13th 2022 10:49 pm 

Description: <empty>
*/

#include "well.h"

#define BORDER_COLOR 0.93f
#define INTERNAL_FIELD_COLOR 0.0f
#define FILLED_BLOCKS_COLOR 0.93f

internal void RenderWell(game_buffer *Buffer, well *Well)
{
    // NOTE(annad): Borders it's just black rectangle in white rectangle!
    DrawRectangle(Buffer, 
                  Well->Pos.X, Well->Pos.Y, 
                  (Well->FullWidth) * Well->CellSideSize, (Well->FullHeight) * Well->CellSideSize,
                  BORDER_COLOR, BORDER_COLOR, BORDER_COLOR);
    
    DrawRectangle(Buffer, 
                  Well->FieldPos.X, Well->FieldPos.Y, 
                  Well->Width * Well->CellSideSize, Well->Height * Well->CellSideSize,
                  INTERNAL_FIELD_COLOR, INTERNAL_FIELD_COLOR, INTERNAL_FIELD_COLOR);
    
    for (u32 Y = 0; Y < Well->Height; Y++)
    {
        u32 CellPosY = Well->FieldPos.Y - Y * Well->CellSideSize;
        for (u32 X = 0; X < Well->Width; X++)
        {
            if (Well->Field[Y * Well->Width + X])
            {
                u32 CellPosX = Well->FieldPos.X + X * Well->CellSideSize;
                
                DrawRectangle(Buffer, 
                              CellPosX, CellPosY,
                              Well->CellSideSize, Well->CellSideSize,
                              FILLED_BLOCKS_COLOR, FILLED_BLOCKS_COLOR, FILLED_BLOCKS_COLOR);
            }
        }
    }
}

inline void SetWellBlockState(well *Well, blocks_states State, i16 PosX, i16 PosY)
{
    assert(PosX < Well->Width);
    assert(PosY < Well->Height);
    assert(PosX >= 0 && PosY >= 0);
    
    PosY = Well->Height - PosY - 1;
    Well->Field[PosY * Well->Width + PosX] = State;
}

inline b32 WellBlockIs(well *Well, blocks_states State, i16 PosX, i16 PosY)
{
    assert(PosX < Well->Width);
    assert(PosY < Well->Height);
    assert(PosX >= 0 && PosY >= 0);
    
    PosY = Well->Height - PosY - 1;
    return Well->Field[PosY * Well->Width + PosX] == State;
}

inline b32 WellBlockIsFilled(well *Well, i16 PosX, i16 PosY)
{
    return WellBlockIs(Well, BLOCK_STATE_FILLED, PosX, PosY);
}
