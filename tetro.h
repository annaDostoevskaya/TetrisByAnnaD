/* 
Author: github.com/annadostoevskaya
File: tetro.h
Date: August 20th 2022 0:48 am 

Description: <empty>
*/

#ifndef TETRO_H
#define TETRO_H


enum tetro_state
{
    TETRO_STATE_SPAWN = 0,
    TETRO_STATE_IN_PROGRESS,
    TETRO_STATE_FALL,
    
    TETRO_STATE_TOTAL
};

#define SIZEOF_TETRO_BLOCK sizeof(i8Vec2)
#define MAX_TETRO_SIZE 4

enum tetro_type
{
    // NOTE(saiel): Use enum classes.
    TETRO_O = 0,
    TETRO_T,
    TETRO_L,
    TETRO_J,
    TETRO_Z,
    TETRO_S,
    TETRO_I,
    
    TETRO_TOTAL,
    // NOTE(saiel): TETRO_UNDEFINED or TETRO_INVALID for invalid states, for example.
};

struct tetro
{
    i8Vec2 Tetrominos[TETRO_TOTAL][MAX_TETRO_SIZE];
    tetro_state State;
    tetro_type Type;
    tetro_type TypesBag[TETRO_TOTAL];
    u32 TypesBagSize;
    
    // TODO(saiel): This can be two arrays, without pointers.
    i16Vec2 *Pos;
    i16Vec2 *ShadowPos;
    i16Vec2 PosBuffers[2];
    
    // TODO(saiel): This can be two arrays, without pointers.
    i8Vec2 *Content;
    i8Vec2 *ShadowContent;
    i8Vec2 ContentBuffers[2][MAX_TETRO_SIZE];
    
    u32 DownTime;
    u32 AccumTime;
};

void DrawTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos);
void RemoveTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos);
void DropTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos);

void MoveTetro(well *Well, i16Vec2 *TetroPos, i8Vec2 MoveVector);
void RotateTetro(well *Well, i8Vec2 *TetroContent);
b32 CheckCollideTetro(well *Well, i8Vec2 *TetroContent, i16Vec2 *TetroPos);

void UpdateTetroPos(well *Well, tetro *Tetro);
void UpdateTetroContent(well *Well, tetro *Tetro);

// TODO(annad): I don't know for sure if this is part of the api or not?
void UpdateTetro(well *Well, tetro *Tetro, game_time *Time);

#endif //TETRO_H

