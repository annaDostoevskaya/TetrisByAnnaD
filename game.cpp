/* 
Author: github.com/annadostoevskaya
File: game.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "game.h"

#include <assert.h>

#ifdef _GAME_INTERNAL
#include <math.h>
#endif

//
// draw.cpp
//

internal void DrawRectangle(game_screen_buffer *Buffer, 
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

inline r64 Remainder(r64 Dividend, r64 Divisor)
{
    return Dividend - (r64)((u64)((Dividend / Divisor) * Divisor));
}

internal u64 GetCharBitmap(u64 *Storage, u64 StorageSize, u8 Chr)
{
    r64 Rem = Remainder(Chr * 0.6180339887, 1.0f);
    u64 BaseIndex = (u64)floor((r64)StorageSize * Rem);
    
    u64 ChrBmp = 0x0;
    u64 Mask = 0xFF;
    
    i16 ShiftIndex = 0;
    for(ShiftIndex = 0; BaseIndex + ShiftIndex < StorageSize; ShiftIndex++)
    {
        ChrBmp = Storage[BaseIndex + ShiftIndex];
        if(Chr == (u8)(ChrBmp & Mask))
        {
            return ChrBmp;
        }
    }
    
    for(ShiftIndex = 0; ShiftIndex < BaseIndex; ShiftIndex++)
    {
        ChrBmp = Storage[ShiftIndex];
        if(Chr == (u8)(ChrBmp & Mask))
        {
            return ChrBmp;
        }
    }
    
    return 0x0;
}

internal void PushCharBitmap(u64 *Storage, u64 StorageSize, u64 ChrBmp)
{
    u64 Mask = 0xFF;
    u8 Chr = (u8)(ChrBmp & Mask);
    // NOTE(annad): Thank you uncle Knuth...
    r64 Rem = Remainder(Chr * 0.6180339887, 1.0f);
    u64 BaseIndex = (u64)floor((r64)StorageSize * Rem);
    
    i16 ShiftIndex = 0;
    for(ShiftIndex = 0; BaseIndex + ShiftIndex < StorageSize; ShiftIndex++)
    {
        if(!Storage[BaseIndex + ShiftIndex])
        {
            Storage[BaseIndex + ShiftIndex] = ChrBmp;
            return;
        }
    }
    
    for(ShiftIndex = 0; ShiftIndex < BaseIndex; ShiftIndex++)
    {
        if(!Storage[ShiftIndex])
        {
            Storage[ShiftIndex] = ChrBmp;
            return;
        }
    }
    
    assert(1 != 1);
    return;
}

#define CHAR_BITMAP_C 0xf08080808080f043
#define CHAR_BITMAP_D 0xe09090909090e044
#define CHAR_BITMAP_E 0xf08080e08080f045
#define CHAR_BITMAP_O 0xf09090909090f04f
#define CHAR_BITMAP_R 0xb0e0c0f09090f052
#define CHAR_BITMAP_S 0xf09010f08090f053
#define CHAR_BITMAP_1 0xe040404040c04031
#define CHAR_BITMAP_2 0xf080402010906032
#define CHAR_BITMAP_3 0x6090106010906033
#define CHAR_BITMAP_4 0x1010f09050301034
#define CHAR_BITMAP_5 0xe01010e08090f035
#define CHAR_BITMAP_6 0xf09090f08090f036
#define CHAR_BITMAP_7 0x40e040201090f037
#define CHAR_BITMAP_8 0xf09090f09090f038
#define CHAR_BITMAP_9 0xf09010f09090f039
#define CHAR_BITMAP_0 0xf09090909090f030
#define CHAR_BITMAP_COLON 0x404000000040403a
// #define CHAR_BITMAP_SPACE 0x000000000000000
// #define CHAR_BITMAP_DEFAULT 0x0000000000000000

internal void DisplayString(game_state *State, game_screen_buffer *Buffer, const u8 *Str, u32 StrLength, u32 X, u32 Y, u32 Size)
{
    u32 *Pixels = (u32 *)Buffer->Memory;
    
    for(u8 i = 0; i < StrLength; i++)
    {
        u8 Symbol = Str[i];
        u8 *CharBmp = NULL;
        
        if(Symbol == '\0' || Symbol == ' ')
        {
            continue;
        }
        // 0.6180339887 (Knuth)
        u64 ProxyChrBmp = GetCharBitmap(State->ChrBmp, 17, Symbol);
        CharBmp = (u8*)(&ProxyChrBmp);
        
        for(i8 j = 7; j > 0; j--)
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

#define PI 3.14159265359

globalv r64 V = 0;
globalv i32 GI = 0;
globalv i32 hz = 450;

extern "C" GAME_UPDATE_SOUND_BUFFER(UpdateSoundBuffer)
{
    assert(sizeof(game_state) < Memory->PermanentStorageSize);
    game_state *State = (game_state *)Memory->PermanentStorage;
    
    if(State->BeepFlag)
    {
        if(GI == SoundBuffer->StreamLen)
        {
            GI = 0;
        }
        
        while(GI < SoundBuffer->StreamLen)
        {
            SoundBuffer->Stream[GI] = (i16)(10000.0f * sin(V * 2 * PI / SoundBuffer->Frequency));
            V += hz;
            GI++;
        }
        
        State->BeepFlag = false;
    }
    else 
    {
        if(GI == SoundBuffer->StreamLen)
        {
            GI = 0;
        }
        
        while(GI < SoundBuffer->StreamLen)
        {
            SoundBuffer->Stream[GI] = 0;
            V += hz;
            GI++;
        }
    }
}

extern "C" GAME_UPDATE_AND_RENDER(UpdateAndRender)
{
    assert(sizeof(game_state) < Memory->PermanentStorageSize);
    __debugbreak();
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
        
        State->ChrBmpSize = CHR_BMP_SIZE;
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_C);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_D);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_E);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_O);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_R);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_S);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_1);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_2);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_3);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_4);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_5);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_6);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_7);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_8);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_9);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_0);
        PushCharBitmap(State->ChrBmp, State->ChrBmpSize, CHAR_BITMAP_COLON);
    }
    
    // DEBUG_CheckWell(Well, Time);
    
    char _StrBuf[256];
    sprintf(_StrBuf, "%d\n", (int)sizeof(Tetro->ContentBuffers));
    
    if(State->Fail)
    {
        for(i16 i = 0; i < (sizeof(Well->Field) / sizeof(Well->Field[0])); i++)
        {
            Well->Field[i] = BLOCK_STATE_EMPTY;
        }
        
        Tetro->State = TETRO_STATE_SPAWN;
        
        if(State->Score > State->Record)
        {
            State->Record = State->Score;
        }
        
        State->Score = 0;
        State->Fail = false;
        // TODO(annad): It can be done better. 
        // Here you are clearing the buffers because there 
        // is a bug that uses them between 500 milliseconds and the tetro control code.
        // 
        // Therefore, after the field is cleared, 
        // the previous block is drawn there again, 
        // taken from the buffer, by the tetromino control code.
        // ZEROING_MEMORY(Tetro->ContentBuffers, sizeof(Tetro->ContentBuffers));
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
    
    RenderWell(Buffer, Well);
    
    
#ifdef _GAME_INTERNAL
    // DrawRectangle(Buffer, State->PosX, State->PosY, State->PlayerSize, State->PlayerSize, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_CheckAllPositions(Buffer, Time, Well->CellSideSize);
    DEBUG_DrawGrid(Buffer, Well->CellSideSize);
    // DrawRectangle(Buffer, 0, Buffer->Height, 50, 50, META_PIXEL_COLOR, META_PIXEL_COLOR, META_PIXEL_COLOR);
    // DEBUG_DrawELT(Buffer);
#endif
    
    {
        if(!CheckCollideTetro(Well, Tetro->Content, Tetro->Pos) && Tetro->State == TETRO_STATE_IN_PROGRESS)
        {
            if(Input->PressedKey != KEY_NOTHING)
            {
                if(Input->PressedKey == KEY_ROTATE_RIGHT || Input->PressedKey == KEY_ROTATE_LEFT)
                {
                    if(Tetro->Type != TETRO_O) // NOTE(annad): Absolutely legal crutch.
                    {
                        // NOTE(annad): Input handling.
                        if(Input->PressedKey == KEY_ROTATE_RIGHT)
                        {
                            static_assert(sizeof(Tetro->ShadowContent) == sizeof(Tetro->Content));
                            static_assert(sizeof(Tetro->ShadowContent) == SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                            memcpy((void*)Tetro->ShadowContent, (void*)Tetro->Content, SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                            RotateTetro(Well, Tetro->ShadowContent);
                            if(!CheckCollideTetro(Well, Tetro->ShadowContent, Tetro->Pos))
                            {
                                UpdateTetroContent(Well, Tetro);
                            }
                        }
                        else if(Input->PressedKey == KEY_ROTATE_LEFT)
                        {
                            // TODO(annad): Rotate left, matrix rotation!
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
                }
#ifdef _GAME_INTERNAL
                else if(Input->PressedKey == KEY_SPACE)
                {
                    // TODO(saiel): All except KEY_SPACE must be handle in pause check.
                    // State->Pause = !State->Pause;
                    __debugbreak();
                }
#endif
                else
                {
                    i8Vec2 MoveVector = {0, 0};
                    
                    if(Input->PressedKey == KEY_MOVE_DOWN)
                    {
                        MoveVector = {0, -1};
                    }
                    else if(Input->PressedKey == KEY_MOVE_LEFT)
                    {
                        MoveVector = {-1, 0};
                    }
                    else if(Input->PressedKey == KEY_MOVE_RIGHT)
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
    
    DisplayString(State, Buffer, State->StrBuffer, 16, ScorePosX, ScorePosY, 2);
    
    // record
    memcpy((void*)State->StrBuffer, (void*)"RECORD: ", 7);
    
    IToStr(&State->StrBuffer[7], 9, State->Record);
    
    u32 RecordPosX = (MetaFontWidth / 2) / 3 - ((MetaFontWidth / 2) / 6);
    u32 RecordPosY = MetaFontHeight - (MetaFontHeight / 3) + (MetaFontHeight / 8);
    memcpy((void*)State->StrBuffer, (void*)"RECORD: ", 7);
    DisplayString(State, Buffer, State->StrBuffer, 16, RecordPosX, RecordPosY, 2);
}
