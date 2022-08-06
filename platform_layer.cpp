/* 
Author: github.com/annadostoevskaya
File: platform_layer.cpp
Date: August 6th 2022 3:26 pm 

Description: <empty>
*/

#include <windows.h>

#include "base_types.h"
#include "platform_layer.h"

#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 1920/2
#define SCREEN_HEIGHT 1080/2

int main(int Argc, char **Argv)
{
    // NOTE(annad): Init platform layer section.
    (void)Argc;
    (void)Argv;
    
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
    
    
    // NOTE(annad): Buffer for rendering to window.
    game_buffer GameBuffer = {};
    GameBuffer.Width = Buffer->w;
    GameBuffer.Height = Buffer->h;
    GameBuffer.Pitch = Buffer->pitch;
    GameBuffer.Memory = Buffer->pixels;
    
    
    // NOTE(annad): Keys states.
    u32 NumKeys = 0;
    const u8 *KeyboardState = SDL_GetKeyboardState((int*)&NumKeys);
    game_input GameInput = {};
    
    
    // NOTE(annad): Event for quit.
    SDL_Event Event = {};
    b32 Run = true;
    
    
    // NOTE(annad): Game Memory Section.
    game_memory GameMemory = {};
    
    
    // NOTE(annad): Setup timer and frame rate.
    u32 FrameRate = 30;
    r32 MillisecondsPerFrame = (1000.0f/(float)FrameRate);
    
    game_time GameTime = {};
    GameTime.BeginTime = SDL_GetTicks();
    GameTime.EndTime = GameTime.BeginTime;
    GameTime.dt = GameTime.EndTime - GameTime.BeginTime;
    
    while(Run)
    {
        // TODO(annad): Cleaning buffer, It's normal?... I don't know.
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
        
        // NOTE(annad): Main.
        UpdateAndRender(&GameBuffer, &GameInput, &GameMemory, &GameTime);
        
        
        SDL_UpdateWindowSurface(Window);
        
        GameTime.EndTime = SDL_GetTicks();
        GameTime.dt = GameTime.EndTime - GameTime.BeginTime;
        
        // char Buffer[256];
        while((r32)GameTime.dt < MillisecondsPerFrame)
        {
            // sprintf(Buffer, "WHILE GAMETIME DT: %d!\n", GameTime.dt);
            // OutputDebugString(Buffer);
            
            // TODO(annad): Sleep and sleep resolution.
            GameTime.EndTime = SDL_GetTicks();
            GameTime.dt = GameTime.EndTime - GameTime.BeginTime;
        }
        
        GameTime.dtSeconds = (float)GameTime.dt/1000.0f;
        // r32 FPS = 1000.0f/(float)GameTime.dt;
        
        
        // sprintf(Buffer, "%f FPS\n", FPS);
        // OutputDebugString(Buffer);
        
        GameTime.BeginTime = GameTime.EndTime;
    }
    
    SDL_FreeSurface(Buffer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    
    return 0;
}