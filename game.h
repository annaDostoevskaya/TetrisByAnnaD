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

#define CHR_BMP_SIZE 17

struct game_state
{
#if 0    
#ifdef _GAME_INTERNAL
    // NOTE(annad): Just for test, see DEBUG_GetInfo()
    bool stub;
    b32 BoolState;
#endif
#endif
    
    b16 Initialized;
    b16 Pause;
    b32 Fail;
    
    u32 MetaPixelSize;
    well Well;
    tetro Tetro;
    
    u64 Score;
    u64 Record;
    
    u8 StrBuffer[16];
    
    b8 BeepFlag;
    
    u64 ChrBmp[CHR_BMP_SIZE];
    u64 ChrBmpSize;
};

#endif //GAME_H

