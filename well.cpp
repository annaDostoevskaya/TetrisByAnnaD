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
#define GRID_COLOR 0.1f

internal void InitWell(game_screen_buffer *Buffer, well *Well, u32 MetaPixelSize)
{
    Well->CellSideSize = MetaPixelSize * 3;
    Well->Width = WELL_WIDTH;
    Well->Height = WELL_HEIGHT;
    Well->FullWidth = Well->Width + 2;
    Well->FullHeight = Well->Height + 2;
    
    Well->Pos.X = Buffer->Width / 2 - (Well->FullWidth * Well->CellSideSize / 2);
    Well->Pos.Y = Well->FullHeight * Well->CellSideSize;
    
    Well->FieldPos.X = Well->Pos.X + Well->CellSideSize;
    Well->FieldPos.Y = Well->Pos.Y - Well->CellSideSize;
}

internal void RenderWell(game_screen_buffer *Buffer, well *Well)
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
    
    // NOTE(annad): Draw tetro field
    for(u32 Y = 0; Y < Well->Height; Y++)
    {
        u32 CellPosY = Well->FieldPos.Y - Y * Well->CellSideSize;
        for(u32 X = 0; X < Well->Width; X++)
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
    
    // NOTE(annad): Draw grid.
    for(i16 X = 0; X < Well->Width + 1; X++)
    {
        DrawRectangle(Buffer, 
                      Well->FieldPos.X + (Well->CellSideSize * X), Well->FieldPos.Y,
                      1, Well->Height * Well->CellSideSize,
                      GRID_COLOR, GRID_COLOR, GRID_COLOR);
    }
    
    for(i16 Y = 0; Y < Well->Height + 1; Y++)
    {
        DrawRectangle(Buffer, 
                      Well->FieldPos.X, Well->FieldPos.Y - (Well->CellSideSize * Y),
                      Well->Width * Well->CellSideSize, 1,
                      GRID_COLOR, GRID_COLOR, GRID_COLOR);
    }
}

inline void SetWellBlockState(well *Well, block_state State, i16Vec2 Pos)
{
    assert(Pos.X < Well->Width);
    assert(Pos.Y < Well->Height);
    assert(Pos.X >= 0 && Pos.Y >= 0);
    
    Pos.Y = Well->Height - Pos.Y - 1;
    Well->Field[Pos.Y * Well->Width + Pos.X] = State;
}

inline b32 WellBlockIs(well *Well, block_state State, i16Vec2 Pos)
{
    assert(Pos.X < Well->Width);
    assert(Pos.Y < Well->Height);
    assert(Pos.X >= 0 && Pos.Y >= 0);
    
    Pos.Y = Well->Height - Pos.Y - 1;
    return Well->Field[Pos.Y * Well->Width + Pos.X] == State;
}

inline b32 WellBlockIsFilled(well *Well, i16Vec2 Pos)
{
    return WellBlockIs(Well, BLOCK_STATE_FILLED, Pos);
}

internal void UpdateWell(well *Well)
{
    i16Vec2 BurnCursor = {0, 0};
    block_state *Field = (block_state*)(&Well->Field);
    
    for(BurnCursor.Y = 0; BurnCursor.Y < Well->Height; BurnCursor.Y++)
    {
        for(BurnCursor.X = 0; BurnCursor.X < Well->Width; BurnCursor.X++)
        {
            if(Field[BurnCursor.Y * Well->Width + BurnCursor.X] != BLOCK_STATE_FILLED)
                break;
        }
        
        if(BurnCursor.X == Well->Width)
        {
            for(i16 Y = BurnCursor.Y; Y > 0; Y--)
            {
                for(i16 X = 0; X < Well->Width; X++)
                {
                    Field[Y * Well->Width + X] = Field[(Y - 1) * Well->Width + X];
                }
            }
        }
    }
}
