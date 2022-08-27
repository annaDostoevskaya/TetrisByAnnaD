/* 
Author: github.com/annadostoevskaya
File: game.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "sdl_game.h"
#include "game.h"

#include <assert.h>

//
// draw.cpp
//

internal void DrawRectangle(game_buffer *Buffer, 
                            i32 PosX, i32 PosY, u32 W, u32 H,
                            r32 Red, r32 Green, r32 Blue)
{
    // TODO(annad): Add cliping...
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

#include "well.cpp"
#include "tetro.cpp"

internal void IToStr(u8 *StrBuf, u32 StrLength, u64 Integer)
{
    for(i32 i = 0; i < StrLength; i++)
    {
        // NOTE(annad): Gift from Saiel, Thanks.
        u64 Digit = Integer % 10;
        Integer /= 10;
        StrBuf[StrLength - 1 - i] = (u8)(Digit + '0');
    }
}

internal void DisplayString(game_buffer *Buffer, const u8 *Str, u32 StrLength, u32 X, u32 Y, u32 Size)
{
    u32 *Pixels = (u32 *)Buffer->Memory;
    
    for(u8 i = 0; i < StrLength; i++)
    {
        u8 Symbol = Str[i];
        u8 *CharBmp = NULL;
        
        if(Symbol == '\0')
        {
            break;
        }
        
        switch(Symbol)
        {
            // TODO(annad): Convert this arrays[8] to one u64 integer!!! and add hash map for accessing and get elements without switch!!!
            case 'S': {
                u8 S[8] = {0xf0, 0x90, 0x80, 0xf0, 0x10, 0x90, 0xf0};
                CharBmp = (u8*)(&S);
                break;
            }
            
            case 'C': {
                u8 C[8] = {0xf0, 0x80, 0x80, 0x80, 0x80, 0x80, 0xf0};
                CharBmp = (u8*)(&C);
                break;
            }
            
            case 'O': {
                u8 O[8] = {0xf0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xf0};
                CharBmp = (u8*)(&O);
                break;
            }
            
            case 'R': {
                u8 R[8] = {0xf0, 0x90, 0x90, 0xf0, 0xc0, 0xe0, 0xb0};
                CharBmp = (u8*)(&R);
                break;
            }
            
            case 'E': {
                u8 E[8] = {0xf0, 0x80, 0x80, 0xe0, 0x80, 0x80, 0xf0};
                CharBmp = (u8*)(&E);
                break;
            }
            
            case '1': {
                u8 _1[8] = {0x40, 0xc0, 0x40, 0x40, 0x40, 0x40, 0xe0};
                CharBmp = (u8*)(&_1);
                break;
            }
            
            case '2': {
                u8 _2[8] = {0x60, 0x90, 0x10, 0x20, 0x40, 0x80, 0xf0};
                CharBmp = (u8*)(&_2);
                break;
            }
            
            case '3': {
                u8 _3[8] = {0x60, 0x90, 0x10, 0x60, 0x10, 0x90, 0x60};
                CharBmp = (u8*)(&_3);
                break;
            }
            
            case '4': {
                u8 _4[8] = {0x10, 0x30, 0x50, 0x90, 0xf0, 0x10, 0x10};
                CharBmp = (u8*)(&_4);
                break;
            }
            
            case '5': {
                u8 _5[8] = {0xf0, 0x90, 0x80, 0xe0, 0x10, 0x10, 0xe0};
                CharBmp = (u8*)(&_5);
                break;
            }
            
            case '6': {
                u8 _6[8] = {0xf0, 0x90, 0x80, 0xf0, 0x90, 0x90, 0xf0};
                CharBmp = (u8*)(&_6);
                break;
            }
            
            case '7': {
                u8 _7[8] = {0xf0, 0x90, 0x10, 0x20, 0x40, 0xe0, 0x40};
                CharBmp = (u8*)(&_7);
                break;
            }
            
            case '8': {
                u8 _8[8] = {0xf0, 0x90, 0x90, 0xf0, 0x90, 0x90, 0xf0};
                CharBmp = (u8*)(&_8);
                break;
            }
            
            case '9': {
                u8 _9[8] = {0xf0, 0x90, 0x90, 0xf0, 0x10, 0x90, 0xf0};
                CharBmp = (u8*)(&_9);
                break;
            }
            
            case '0': {
                u8 _0[8] = {0xf0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xf0};
                CharBmp = (u8*)(&_0);
                break;
            }
            
            case ':': {
                u8 Colon[8] = {0x40, 0x40, 0x0, 0x0, 0x0, 0x40, 0x40};
                CharBmp = (u8*)(&Colon);
                break;
            }
            
            case ' ': {
                u8 Space[8] = {0x0};
                CharBmp = (u8*)(&Space);
                break;
            }
            
            case 'D': {
                u8 D[8] = {0xe0, 0x90, 0x90, 0x90, 0x90, 0x90, 0xe0};
                CharBmp = (u8*)(&D);
                break;
            }
            
            default: {
                u8 Default[8] = {0x0};
                CharBmp= (u8*)(&Default);
                break;
            }
        }
        
        for(i8 j = 0; j < 8; j++)
        {
            for(u8 k = 0; k < 8; k++)
            {
                if(CharBmp[j] & (1 << k))
                {
                    // TODO(annad): Pass Colors.
                    u32 LetterSpace = 8 * Size;
                    u32 LetterShift = i * LetterSpace;
                    u32 BitShiftX = X - (k * Size);
                    u32 BitShiftY = Y - (j * Size);
                    u32 MetaPixel = (Buffer->Height / 70) / 7;
                    // u32 GlobalX = MetaPixel * (BitShiftX + LetterShift);
                    // u32 GlobalY = MetaPixel * (BitShiftY);
                    u32 GlobalX = MetaPixel * (BitShiftX + LetterShift);
                    u32 GlobalY = MetaPixel * (BitShiftY);
                    
                    // DrawRectangle(Buffer, GlobalX, GlobalY, Size * MetaPixel, Size * MetaPixel, 1.0f, 1.0f, 1.0f);
                    DrawRectangle(Buffer, GlobalX, GlobalY, MetaPixel * Size, MetaPixel * Size, 1.0f, 1.0f, 1.0f);
                }
            }
        }
    }
}

extern "C" GAME_UPDATE_AND_RENDER(UpdateAndRender)
{
    assert(sizeof(game_state) < Memory->PermanentStorageSize);
    
    // NOTE(annad): Init.
    // NOTE(saiel): Objectively, this UB.
    game_state *State = (game_state *)Memory->PermanentStorage;
    
    well *Well = &State->Well;
    tetro *Tetro = &State->Tetro;
    
    if(State->Initialized != (b32)true)
    {
        
#if 0        
#ifdef _GAME_INTERNAL
        State->BoolState = 1;
#endif
#endif
        
        State->MetaPixelSize = Buffer->Height / 70;
        State->Fail = false;
        
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
        static_assert(sizeof(DefaultTetrominos) == (SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE * TETRO_TOTAL));
        static_assert(sizeof(Tetro->Tetrominos) == sizeof(DefaultTetrominos));
        memcpy((void*)(&Tetro->Tetrominos), (void*)(&DefaultTetrominos), sizeof(DefaultTetrominos));
        
        Tetro->Content = (i8Vec2*)(&Tetro->ContentBuffers[0]);
        Tetro->ShadowContent = (i8Vec2*)(&Tetro->ContentBuffers[1]);
        
        Tetro->Pos = (&Tetro->PosBuffers[0]);
        Tetro->ShadowPos = (&Tetro->PosBuffers[1]);
        
        Tetro->DownTime = 500;
        Tetro->AccumTime = Tetro->DownTime;
        
        tetro_type TypesBag[TETRO_TOTAL] = {
            TETRO_O, TETRO_T, TETRO_L,
            TETRO_J, TETRO_Z, TETRO_S, TETRO_I,
        };
        
        static_assert(sizeof(Tetro->TypesBag) == sizeof(TypesBag));
        memcpy((void*)(&Tetro->TypesBag), (void*)(&TypesBag), sizeof(TypesBag));
        Tetro->TypesBagSize = TETRO_TOTAL;
        
        State->Pause = false;
        State->Initialized = true;
        State->Score = 0;
    }
    
    // DEBUG_CheckWell(Well, Time);
    
    if(State->Fail)
    {
        for(i16 i = 0; i < (sizeof(Well->Field) / sizeof(Well->Field[0])); i++)
        {
            Well->Field[i] = BLOCK_STATE_EMPTY;
        }
        
        if(State->Score > State->Record)
        {
            State->Record = State->Score;
        }
        
        State->Score = 0;
        State->Fail = false;
    }
    
    if(State->Pause == (b32)false)
    {
        UpdateTetro(State, Well, Tetro, Time);
    }
    
    {
        // NOTE(annad): Check player win (update well)
        
        // TODO(annad): 
        // If it is relevant, you need to put this data in bit fields and use 
        // two bytes for everything. Only then will the size of the well 
        // possibly become static.
        
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
    
    {
        // NOTE(annad): Check player fail.
        block_state *Field = (block_state*)(&Well->Field);
        
        for(i16 X = 0; X < WELL_WIDTH; X++)
        {
            // NOTE(annad): Check just most high row!
            if(Field[X] == BLOCK_STATE_FILLED)
            {
                State->Fail = true;
            }
        }
    }
    
    RenderWell(Buffer, Well);
    
    
#if 0    
#ifdef _GAME_INTERNAL
    // DrawRectangle(Buffer, State->PosX, State->PosY, State->PlayerSize, State->PlayerSize, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_CheckAllPositions(Buffer, Time, Well->CellSideSize);
    DEBUG_DrawGrid(Buffer, Well->CellSideSize);
    // DrawRectangle(Buffer, 0, Buffer->Height, 50, 50, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_DrawELT(Buffer);
#endif
#endif
    
    {
        if(!CheckCollideTetro(Well, Tetro->Content, Tetro->Pos))
        {
            if(Input->PressedKey != KEY_NOTHING)
            {
                // NOTE(annad): Input handling.
                if(Input->PressedKey == KEY_UP)
                {
                    if(Tetro->Type != TETRO_O) // NOTE(annad): Absolutely legal crutch.
                    {
                        // TODO(annad): Check this static assert...?..
                        static_assert(sizeof(Tetro->ShadowContent) == sizeof(Tetro->Content));
                        static_assert(sizeof(Tetro->ShadowContent) == SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
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
                    // TODO(saiel): All except KEY_SPACE must be handle in pause check.
                    // State->Pause = !State->Pause;
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
            }
            
#if 0            
#ifdef _GAME_INTERNAL
            if(CheckCollideTetro(Well, Tetro->Content, Tetro->Pos))
            {
                State->BoolState = 1;
            }
            else
            {
                State->BoolState = 0;
            }
            
            DEBUG_BoolInScreen(Buffer, State->BoolState);
#endif
#endif
            
        }
    }
    
    // Initialize mb?..
    memcpy((void*)State->StrBuffer, (void*)"SCORE: ", 7);
    
    IToStr(&State->StrBuffer[7], 9, State->Score);
    
    /*     
        u32 ScorePosX = (Buffer->Width / 2) / 3 - ((Buffer->Width / 2) / 6);
        u32 ScorePosY = Buffer->Height - (Buffer->Height / 3) + (Buffer->Height / 6);
         */
    
    u32 FontPixel = (State->MetaPixelSize) / 7; // 2
    u32 MetaFontWidth = Buffer->Width / FontPixel;
    u32 MetaFontHeight = Buffer->Height / FontPixel;
    
    u32 ScorePosX = (MetaFontWidth / 2) / 3 - ((MetaFontWidth / 2) / 6);;
    u32 ScorePosY = MetaFontHeight - (MetaFontHeight / 3) + (MetaFontHeight / 6);
    
    DisplayString(Buffer, State->StrBuffer, 16, ScorePosX, ScorePosY, 2);
    
    // record
    memcpy((void*)State->StrBuffer, (void*)"RECORD: ", 7);
    
    IToStr(&State->StrBuffer[7], 9, State->Record);
    
    u32 RecordPosX = (MetaFontWidth / 2) / 3 - ((MetaFontWidth / 2) / 6);
    u32 RecordPosY = MetaFontHeight - (MetaFontHeight / 3) + (MetaFontHeight / 8);
    
    DisplayString(Buffer, State->StrBuffer, 16, RecordPosX, RecordPosY, 2);
}
