/* 
Author: github.com/annadostoevskaya
File: main.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include <windows.h>

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef float r32;
typedef double r64;

typedef u32 b32;

#define internal static
#define globalv static
#define localv static

#define SCREEN_WIDTH 1920/2
#define SCREEN_HEIGHT 1080/2


enum tetrominoes 
{
    TETROMINO_STRAIGHT = 0,
    TETROMINO_SQUARE,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_SKEW,
    
    TETROMINO_TOTAL
};

struct game_state 
{
    b32 Initialized;
    u32 PosX;
    u32 PosY;
    
    void *WellMemory;
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
    KEY_NOTHING,
    KEY_DOWN,
    KEY_UP,
    KEY_LEFT,
    KEY_RIGHT,
    
    KEY_TOTAL
};

struct game_input 
{
    Keys PressedKey;
};

struct game_buffer 
{
    u32 Width;
    u32 Height;
    u32 Pitch;
    void *Memory;
};

#define WHITE_COLOR_RGB 0.93f
#define GRID_COLOR_HEX 0xFF111111 

#define WELL_WIDTH 9
#define WELL_HEIGHT 16

// globalv u32 CellSide = 10 * 3; // TODO(annad): Grid System

internal void DrawRectangle(game_buffer *Buffer, i32 X, i32 Y, i32 W, i32 H,
                            r32 Red, r32 Green, r32 Blue)
{
    // NOTE(annad): Reverse buffer.
    Y = Buffer->Height - Y;
    
    // TODO(annad): Grid Rendering.
    /*
    if(X % CellSide != 0)
    {
        X += X % CellSide;
    }
    
    if(Y % CellSide != 0)
    {
        Y += Y % CellSide;
    }
    */
    if(W < 0)
    {
        W = 0;
    }
    
    if(W > Buffer->Width)
    {
        W = W % Buffer->Width;
    }
    
    if(X < 0)
    {
        X = 0;
    }
    
    if(Y < 0)
    {
        Y = 0;
    }
    
    if(X + W < 0)
    {
        X = 0;
    }
    
    if(X + W > Buffer->Width)
    {
        X = Buffer->Width - W;
    }
    
    if(H < 0)
    {
        H = 0;
    }
    
    if(H > Buffer->Height)
    {
        H = H % Buffer->Height;
    }
    
    if(H > Buffer->Height)
    {
        H = 0;
    }
    
    if(Y + H < 0)
    {
        Y = 0;
    }
    
    if(Y + H > Buffer->Height)
    {
        
        Y = Buffer->Height - H;
    }
    
    
    u32 *Pixels = (u32 *)Buffer->Memory;
    
    u32 Color = (0xFF000000 | // A R G B
                 ((u32)(Blue * 255.0f) << 0) |
                 ((u32)(Green * 255.0f) << 8) | 
                 ((u32)(Red * 255.0f) << 16));
    
    for(i32 i = Y; i < Y + H; i += 1)
    {
        for(i32 j = X; j < X + W; j += 1)
        {
            Pixels[i * Buffer->Width + j] = Color;
        }
    }
}

internal void DrawBaseGrid(game_buffer *Buffer, u32 CellWidth, u32 CellHeight)
{
    u32 *Pixels = (u32 *)Buffer->Memory;
    
    for(i32 Y = 0; Y < Buffer->Height; ++Y)
    {
        for(i32 X = 0; X < Buffer->Width; ++X)
        {
            if(X % CellWidth == 0 || Y % CellHeight == 0)
            {
                Pixels[Y * Buffer->Width + X] = GRID_COLOR_HEX;
            }
        }
    }
}


internal void DrawWell(game_buffer *Buffer, 
                       u32 WellPositionX, u32 WellPositionY,
                       u32 WellWidth, u32 WellHeight,
                       u32 WellCellSizeSide)
{
    for(i32 Y = 0; Y < WellHeight; ++Y)
    {
        u32 DrawPosY = WellPositionY + Y * WellCellSizeSide;
        
        for(i32 X = 0; X < WellWidth; ++X)
        {
            if(X == 0 || Y == 0 || X == WellWidth - 1)
            {
                u32 DrawPosX = WellPositionX + X * WellCellSizeSide;
                
                DrawRectangle(Buffer, 
                              DrawPosX, DrawPosY, 
                              WellCellSizeSide, WellCellSizeSide,
                              WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB);
            }
        }
    }
}


internal void UpdateAndRender(game_buffer *Buffer, 
                              game_input *Input,
                              game_state *State,
                              game_time *Time)
{
    u32 MetaPixelSize = 10; // NOTE(annad): Pixels size. 1 cube is 50px.
    
    // NOTE(annad): Draw Well.
    u32 WellWidth = WELL_WIDTH + 2;
    u32 WellHeight = WELL_HEIGHT + 2;
    u32 WellCellSide = MetaPixelSize * 3;
    u32 WellPositionX = Buffer->Width / 2 - (WellWidth * WellCellSide / 2);
    u32 WellPositionY = 0;
    
    DrawWell(Buffer,
             WellPositionX, WellPositionY,
             WellWidth, WellHeight,
             WellCellSide);
    
    // NOTE(annad): Init.
    if(State->Initialized != true)
    {
        State->PosX = Buffer->Width / 2;
        State->PosY = Buffer->Height / 2;
        
        globalv u8 Mem[WELL_WIDTH * WELL_HEIGHT];
        State->WellMemory = (void*)Mem;
        
        State->Initialized = true;
    }
    
    {
        // NOTE(annad): Input handling.
        if(Input->PressedKey == KEY_UP)
        {
            State->PosY += 500 * Time->dtSeconds;
        }
        else if(Input->PressedKey == KEY_DOWN)
        {
            State->PosY -= 500 * Time->dtSeconds;
        }
        else if(Input->PressedKey == KEY_LEFT)
        {
            State->PosX -= 500 * Time->dtSeconds;
        }
        else if(Input->PressedKey == KEY_RIGHT)
        {
            State->PosX += 500 * Time->dtSeconds;
        }
    }
    
    // NOTE(annad): Draw Grid.
    u32 CellWidth = MetaPixelSize * 3;
    u32 CellHeight = MetaPixelSize * 3;
    
    DrawBaseGrid(Buffer, CellWidth, CellHeight);
    
    DrawRectangle(Buffer, State->PosX , State->PosY, MetaPixelSize * 3, MetaPixelSize * 3, WHITE_COLOR_RGB, WHITE_COLOR_RGB, WHITE_COLOR_RGB);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        printf("Failed initialize SDL. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    SDL_Window *Window = NULL;
    SDL_Surface *Buffer = NULL;
    
    Window = SDL_CreateWindow("Tetris by Anna Dostoevskaya", 
                              SDL_WINDOWPOS_UNDEFINED, 
                              SDL_WINDOWPOS_UNDEFINED, 
                              SCREEN_WIDTH, 
                              SCREEN_HEIGHT, 
                              SDL_WINDOW_SHOWN);
    if(Window == NULL)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    
    Buffer = SDL_GetWindowSurface(Window);
    if(Buffer == NULL)
    {
        printf("Surface could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    game_buffer GameBuffer = {};
    GameBuffer.Width = Buffer->w;
    GameBuffer.Height = Buffer->h;
    GameBuffer.Pitch = Buffer->pitch;
    GameBuffer.Memory = Buffer->pixels;
    
    // NOTE(annad): Keys states.
    u32 NumKeys = 0;
    const u8 *KeyboardState = SDL_GetKeyboardState((int*)&NumKeys);
    game_input GameInput = {};
    
    SDL_Event Event = {};
    
    b32 Run = true;
    
    game_state GameState = {};
    
    // NOTE(annad): Time. 60FPS.
    u32 FrameRate = 30;
    r32 MillisecondsPerFrame = (1000.0f/(float)FrameRate);
    
    char StringBuffer[256];
    
    game_time GameTime = {};
    GameTime.BeginTime = SDL_GetTicks();
    GameTime.EndTime = GameTime.BeginTime;
    GameTime.dt = GameTime.EndTime - GameTime.BeginTime;
    
    while(Run)
    {
        // TODO(annad): It's normal?... I don't know.
        SDL_FillRect(Buffer, NULL, SDL_MapRGB(Buffer->format, 0, 0, 0));
        
        // NOTE(annad): Events.
        SDL_PollEvent(&Event);
        
        switch(Event.type)
        {
            // NOTE(annad): Alt+F4 work too.
            case SDL_QUIT: 
            {
                Run = false;
                break;
            }
            
            default:
            {
                ; // NOTE(annad): Nothing...
            }
        }
        
        
        if(KeyboardState[SDL_SCANCODE_UP] == 1)
        {
            GameInput.PressedKey = KEY_UP;
        }
        else if(KeyboardState[SDL_SCANCODE_DOWN])
        {
            GameInput.PressedKey = KEY_DOWN;
        }
        else if(KeyboardState[SDL_SCANCODE_LEFT] == 1)
        {
            GameInput.PressedKey = KEY_LEFT;
        }
        else if(KeyboardState[SDL_SCANCODE_RIGHT])
        {
            GameInput.PressedKey = KEY_RIGHT;
        }
        else
        {
            GameInput.PressedKey = KEY_NOTHING;
        }
        
        UpdateAndRender(&GameBuffer, &GameInput, &GameState, &GameTime);
        
        SDL_UpdateWindowSurface(Window);
        
        GameTime.EndTime = SDL_GetTicks();
        GameTime.dt = GameTime.EndTime - GameTime.BeginTime;
        while(GameTime.dt < MillisecondsPerFrame)
        {
            // TODO(annad): Sleep and sleep resolution.
            GameTime.EndTime = SDL_GetTicks();
            GameTime.dt = GameTime.EndTime - GameTime.BeginTime;
        }
        
        GameTime.dtSeconds = (float)GameTime.dt/1000.0f;
        
        GameTime.BeginTime = GameTime.EndTime;
    }
    
    SDL_FreeSurface(Buffer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    
    return 0;
}