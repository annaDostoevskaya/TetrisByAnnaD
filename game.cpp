/* 
Author: github.com/annadostoevskaya
File: game.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include "game.h"
// DEV: #include <assert.h>

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

// NOTE(annad): Maybe we must remove this and use stdlib of C?.. idk...
#include "mmath.cpp"
#include "algs.cpp"

#include "game_well.cpp"
#include "game_tetro.cpp"
#include "game_charbmp.cpp"

extern "C" GAME_UPDATE_SOUND_BUFFER(UpdateSoundBuffer)
{
    // DEV: assert(sizeof(game_state) < Memory->PermanentStorageSize);
    game_state *State = (game_state *)Memory->PermanentStorage;
    midi *Midi = &(State->Midi);
    
    if(Midi->Duration > 0)
    {
        for(i32 i = 0; i < SoundBuffer->StreamLen; i++)
        {
            r64 CycleFreq = 2 * MMATH_PI / SoundBuffer->Frequency;
            SoundBuffer->Stream[i] = (i16)(((r32)MAX_I16_VALUE * Midi->Volume) * Sin(CycleFreq * (r64)Midi->HzAccum));
            Midi->HzAccum += Midi->Hz;
        }
        
        Midi->Duration -= (i64)(1 / (r64)(SoundBuffer->Frequency / (r64)SoundBuffer->StreamLen) * 1000.0f);
    }
    else
    {
        for(i32 i = 0; i < SoundBuffer->StreamLen; i++)
        {
            SoundBuffer->Stream[i] = 0;
        }
    }
    
    if(Midi->Duration < 0)
    {
        Midi->HzAccum = 0;
    }
}

extern "C" GAME_UPDATE_AND_RENDER(UpdateAndRender)
{
    // DEV: assert(sizeof(game_state) < Memory->PermanentStorageSize);
    
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
    
    UpdateTetro(State, Time->dt);
    UpdateWell(Well);
    RenderWell(Buffer, Well);
    
    {
        // NOTE(annad): Update Input.
        // We check for collision because the Tetrimino update happens every 500 milliseconds.
        if(!CheckCollideTetro(Well, Tetro->Content, Tetro->Pos) && Tetro->State == TETRO_STATE_IN_PROGRESS)
        {
            if(Input->PressedKey != KEY_NOTHING)
            {
                if(Input->PressedKey == KEY_ROTATE_RIGHT || Input->PressedKey == KEY_ROTATE_LEFT)
                {
                    if(Tetro->Type != TETRO_O) // NOTE(annad): Absolutely legal crutch.
                    {
                        // DEV: static_assert(sizeof(Tetro->ShadowContent) == sizeof(Tetro->Content));
                        // DEV: static_assert(sizeof(Tetro->ShadowContent) == SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                        SIMDMemoryCopy((void*)Tetro->ShadowContent, (void*)Tetro->Content, SIZEOF_TETRO_BLOCK * MAX_TETRO_SIZE);
                        i32 *RotateMatrix = NULL;
                        
                        if(Input->PressedKey == KEY_ROTATE_RIGHT)
                        {
                            i32 RightRotateMatrix[2][2] = {{0, 1}, {-1, 0}};
                            RotateMatrix = (i32*)(&RightRotateMatrix);
                        }
                        else
                        {
                            i32 LeftRotateMatrix[2][2] = {{0, -1},{1, 0}};
                            RotateMatrix = (i32*)(&LeftRotateMatrix);
                        }
                        
                        RotateTetro(Well, Tetro->ShadowContent, (RotateMatrix));
                        if(!CheckCollideTetro(Well, Tetro->ShadowContent, Tetro->Pos))
                        {
                            UpdateTetroContent(Well, Tetro);
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
    
    // NOTE(annad): Print Score
    SIMDMemoryCopy((void*)String->Buf, (void*)"SCORE: ", 7);
    UIToStr(&(String->Buf[7]), 9, State->Score);
    r32 ScorePosX = ((r32)(Buffer->Width / 12) / (r32)Buffer->Width);
    r32 ScorePosY = ((r32)(Buffer->Height - Buffer->Height / 5) / (r32)Buffer->Height);
    r32 Scale = 0.004f;
    DisplayString(Buffer, CharBmpBuf, String, ScorePosX, ScorePosY, Scale);
    
    // NOTE(annad): Print Record
    SIMDMemoryCopy((void*)String->Buf, (void*)"RECORD: ", 7);
    UIToStr(&(String->Buf[7]), 9, State->Record);
    r32 RecordPosX = ((r32)(Buffer->Width / 12) / (r32)Buffer->Width);
    r32 RecordPosY = ((r32)(Buffer->Height - Buffer->Height / 4) / (r32)Buffer->Height);
    
    DisplayString(Buffer, CharBmpBuf, String, RecordPosX, RecordPosY, Scale);
}

