/* 
Author: github.com/annadostoevskaya
File: game.h
Date: August 6th 2022 3:35 pm 

Description: <empty>
*/

#ifndef GAME_H
#define GAME_H

#include "platform_game.h"

#include "mmath.h"
#include "base_types.h"
#include "sdl_game.h"
#include "game_well.h"
#include "game_tetro.h"
#include "game_charbmp.h"

//
// string.h
//

#define STR_BUFFER_SIZE 16

struct str_buf
{
    u8 Buf[STR_BUFFER_SIZE];
    size_t Size;
};

//
// string.h
//

struct midi
{
    i64 Hz;
    i64 HzAccum;
    i64 Duration;
    r32 Volume;
};

struct game_state
{
    b16 Initialized;
    b16 Fail;
    
    u32 MetaPixelSize;
    
    u64 Score;
    u64 Record;
    
    well Well;
    tetro Tetro;
    
    char_bmp_buf CharBmps;
    str_buf String;
    
    midi Midi;
};

#endif //GAME_H

