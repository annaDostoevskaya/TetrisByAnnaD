/* 
Author: github.com/annadostoevskaya
File: game.h
Date: August 6th 2022 3:35 pm 

Description: <empty>
*/

#ifndef GAME_H
#define GAME_H

#include "platform_game.h"

#include "algs.h"
#include "mmath.h"
#include "base_types.h"
#include "sdl_game.h"
#include "well.h"
#include "tetro.h"
#include "charbmp.h"

#define STR_BUFFER_SIZE 16

struct str_buf
{
    u8 Buf[STR_BUFFER_SIZE];
    size_t Size;
};

struct game_state
{
    b16 Initialized;
    b16 Fail;
    
    u32 MetaPixelSize;
    
    well Well;
    tetro Tetro;
    
    u64 Score;
    u64 Record;
    
    b8 BeepFlag;
    
    char_bmp_buf CharBmps;
    str_buf String;
    
    u64 SoundHz;
    u64 SoundDur;
};

#endif //GAME_H

