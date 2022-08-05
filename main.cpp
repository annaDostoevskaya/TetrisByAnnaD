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

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540


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
    b32 Initialied;
    u32 PosX;
    u32 PosY;
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

void DrawCube(game_buffer *Buffer, i32 X, i32 Y, i32 W, i32 H,
              r32 Red, r32 Green, r32 Blue)
{
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

void UpdateAndRender(game_buffer *Buffer, 
                     game_input *Input,
                     game_state *State,
                     game_time *Time)
{
    if(State->Initialied != true)
    {
        State->PosX = Buffer->Width / 2;
        State->PosY = Buffer->Height / 2;
        State->Initialied = true;
    }
    
    // tetrominoes base = TETROMINO_STRAIGHT;
    
    u32 MetaPixelSize = 10; // NOTE(annad): Pixels size. 1 cube is 50px.
    
    if(Input->PressedKey == KEY_UP)
    {
        State->PosY -= 500 * Time->dtSeconds;
    }
    else if(Input->PressedKey == KEY_DOWN)
    {
        State->PosY += 500 * Time->dtSeconds;
    }
    else if(Input->PressedKey == KEY_LEFT)
    {
        State->PosX -= 500 * Time->dtSeconds;
    }
    else if(Input->PressedKey == KEY_RIGHT)
    {
        State->PosX += 500 * Time->dtSeconds;
    }
    
    DrawCube(Buffer, State->PosX , State->PosY, MetaPixelSize, MetaPixelSize, 0.5f, 0.5f, 0.5f);
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
        SDL_FillRect(Buffer, NULL, SDL_MapRGB(Buffer->format, 255, 255, 255));
        
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