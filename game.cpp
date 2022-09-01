/* 
Author: github.com/annadostoevskaya
File: game.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "game.h"
#include <assert.h>

// NOTE(annad): We need to define a header protocol

//
// draw.cpp
//

internal void DrawRectangle(game_screen_buffer *Buffer, 
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

#include "well.cpp"
#include "tetro.cpp"
#include "charbmp.cpp"

// NOTE(annad): Maybe we must remove this and use stdlib of C?.. idk...
#include "mmath.cpp"
#include "algs.cpp"

globalv r64 V = 0;
globalv i32 GI = 0;
globalv i32 hz = 450;

// TODO(annd): AUDIO, rafactoring,???????
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
            SoundBuffer->Stream[GI] = (i16)(10000.0f * Sin(V * 2 * MMATH_PI / SoundBuffer->Frequency));
            V += hz; // NOTE(annad): find out what omega (V = w) means in harmonic vibrations
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
    
    // NOTE(annad): Init.
    // NOTE(saiel): Objectively, this UB.
    game_state *State = (game_state *)Memory->PermanentStorage;
    
    well *Well = &(State->Well);
    tetro *Tetro = &(State->Tetro);
    char_bmp_buf *CharBmpBuf = &(State->CharBmps);
    str_buf *String = &(State->String);
    
    if(State->Initialized != (b32)true)
    {
        State->MetaPixelSize = Buffer->Height / 70;
        State->Fail = false;
        
        InitWell(Buffer, Well, State->MetaPixelSize);
        InitTetro(Tetro);
        
        // NOTE(annad): String initialize.
        String->Size = STR_BUFFER_SIZE;
        
        // NOTE(annad): CharBmpBuf initialize
        CharBmpBuf->Size = CHR_BMP_BUF_SIZE;
        
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_C);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_D);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_E);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_O);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_R);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_S);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_1);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_2);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_3);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_4);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_5);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_6);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_7);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_8);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_9);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_0);
        PushCharBitmap(CharBmpBuf, CHAR_BITMAP_COLON);
        
        State->Initialized = true;
        State->Score = 0;
    }
    
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
    }
    
    UpdateTetro(State, Well, Tetro, Time);
    UpdateWell(Well);
    RenderWell(Buffer, Well);
    
    {
        // update wat?... update fidback?..
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
                            SIMDMemoryCopy((void*)Tetro->ShadowContent, (void*)Tetro->Content, SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
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
                            SIMDMemoryCopy((void*)Tetro->ShadowContent, (void*)Tetro->Content, SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                            RotateTetro(Well, Tetro->ShadowContent);
                            if(!CheckCollideTetro(Well, Tetro->ShadowContent, Tetro->Pos))
                            {
                                UpdateTetroContent(Well, Tetro);
                            }
                        }
                    }
                }
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
        }
    }
    
    /*
     TODO(annad): Font...
In fact, it was at this moment that I realized that it was necessary 
to use a value in the range from 0 to 1 to position elements on the screen. 
Let's assume that this part of the system was written 
in those days when there was no FPU.
*/
    u32 FontPixel = (State->MetaPixelSize) / 7; // 2
    u32 MetaFontWidth = Buffer->Width / FontPixel;
    u32 MetaFontHeight = Buffer->Height / FontPixel;
    
    // NOTE(annad): Print Score
    SIMDMemoryCopy((void*)String->Buf, (void*)"SCORE: ", 7);
    UIToStr(&(String->Buf[7]), 9, State->Score);
    u32 ScorePosX = (MetaFontWidth / 2) / 3 - ((MetaFontWidth / 2) / 6);
    u32 ScorePosY = MetaFontHeight - (MetaFontHeight / 3) + (MetaFontHeight / 6);
    DisplayString(Buffer, CharBmpBuf, String, State->MetaPixelSize, ScorePosX, ScorePosY, 2);
    
    // NOTE(annad): Print Record
    SIMDMemoryCopy((void*)String->Buf, (void*)"RECORD: ", 7);
    UIToStr(&(String->Buf[7]), 9, State->Record);
    u32 RecordPosX = (MetaFontWidth / 2) / 3 - ((MetaFontWidth / 2) / 6);
    u32 RecordPosY = MetaFontHeight - (MetaFontHeight / 3) + (MetaFontHeight / 8);
    DisplayString(Buffer, CharBmpBuf, String, State->MetaPixelSize, RecordPosX, RecordPosY, 2);
}

