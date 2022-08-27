/* 
Author: github.com/annadostoevskaya
File: sdl_game.h
Date: August 6th 2022 6:23 pm 

Description: <empty>
*/

#ifndef SDL_GAME_H
#define SDL_GAME_H

#include "base_types.h"

#define PERMANENT_STORAGE_SIZE 4096

struct game_memory
{
    u8 PermanentStorage[PERMANENT_STORAGE_SIZE]; // NOTE(annad): We have to fit in 4KB
    u64 PermanentStorageSize;
    
    b32 Run;
};

struct game_time 
{
    u32 BeginTime;
    u32 EndTime;
    u32 dt;
    r32 dtSeconds;
};

enum Keys 
{
    KEY_NOTHING = 0,
    
    KEY_DOWN,
    KEY_UP,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_SPACE,
    
    KEY_TOTAL
};

struct game_input 
{
    b32 Changed;
    Keys PressedKey;
};

struct game_buffer 
{
    u32 Width;
    u32 Height;
    u32 Pitch;
    void *Memory;
};

#define GAME_UPDATE_AND_RENDER(name) void name(game_buffer *Buffer, \
game_input *Input, \
game_memory *Memory, \
game_time *Time)

typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

struct game
{
    game_update_and_render *UpdateAndRender;
};

extern "C" GAME_UPDATE_AND_RENDER(UpdateAndRender);

#endif //SDL_GAME_H
