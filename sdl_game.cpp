/* 
Author: github.com/annadostoevskaya
File: sdl_game.cpp
Date: August 6th 2022 3:26 pm 

Description: <empty>
*/

// TODO(annad): Sound!

#ifdef _GAME_INTERNAL
#include <windows.h>
#include <stdio.h>
#endif

#include "base_types.h"
#include "sdl_game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080

/* 
// TODO(annad): Audio, but later...
#define NUM_SOUNDS 1

const u32 SOUND_BUFFER_SIZE = 512;

void MixAudio(void *Udata, Uint8 *stream, int len)
{
    
}
 */

int main(int Argc, char **Argv)
{
    // NOTE(annad): Init platform layer section.
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
    {
        OutputDebugString("Failed initialize SDL!");
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
        OutputDebugString("Window could not be created!");
        return -1;
    }
    
    Buffer = SDL_GetWindowSurface(Window);
    if(Buffer == NULL)
    {
        OutputDebugString("Surface could not be created!");
        return -1;
    }
    
    // NOTE(annad): Buffer for rendering to window.
    game_buffer GameBuffer = {};
    GameBuffer.Width = Buffer->w;
    GameBuffer.Height = Buffer->h;
    GameBuffer.Pitch = Buffer->pitch;
    GameBuffer.Memory = Buffer->pixels;
    
    // NOTE(annad): Keys states.
    game_input GameInput = {};
    // NOTE(annad): Event for quit.
    SDL_Event Event = {};
    
    // NOTE(annad): Setup timer and frame rate.
    u32 FrameRate = 30;
    r32 MillisecondsPerFrame = (1000.0f/(float)FrameRate);
    
    game_time GameTime = {};
    GameTime.BeginTime = SDL_GetTicks();
    GameTime.EndTime = GameTime.BeginTime;
    GameTime.dt = GameTime.EndTime - GameTime.BeginTime;
    
    //
    // sdl_audio
    //
    
    /*     
    // TODO(annad): Audio, but later...
        SDL_AudioSpec AudioFormat;
        
        AudioFormat.freq = 48000; // freq is samples per seconds.
        AudioFormat.format = AUDIO_S16; // sample is 16 bit, dependency format.
        AudioFormat.channels = 1; // 16 bit per channel.
        AudioFormat.samples = SOUND_BUFFER_SIZE; 
        AudioFormat.callback = MixAudio;
        AudioFormat.userdata = (void*)(&AudioFormat);
        
        if(SDL_OpenAudio(&AudioFormat, NULL) < 0)
        {
            OutputDebugString("Unable to open audio");
            return -1;
        }
        
        SDL_PauseAudio(0);
         */
    
    //
    // sdl_audio
    //
    
    game Game = {};
    void *GameDLLHandle = SDL_LoadObject("./game.dll");
    if(!GameDLLHandle)
    {
        OutputDebugString("Can't load DLL file!");
        return -1;
    }
    
    Game.UpdateAndRender = (game_update_and_render*)SDL_LoadFunction(GameDLLHandle, "UpdateAndRender");
    
    if(!Game.UpdateAndRender)
    {
        OutputDebugString("Can't load DLL file!");
        return -1;
    }
    
    // NOTE(annad): Game Memory Section.
    game_memory GameMemory = {};
    GameMemory.PermanentStorageSize = PERMANENT_STORAGE_SIZE;
    
    b32 Run = true;
    while(Run)
    {
        // NOTE(annad): Cleaning all data from previous frame.
        SDL_FillRect(Buffer, NULL, SDL_MapRGB(Buffer->format, 0, 0, 0));
        GameInput.PressedKey = KEY_NOTHING;
        
        // NOTE(annad): Events.
        while(SDL_PollEvent(&Event))
        {
            switch(Event.type)
            {
                // NOTE(annad): Alt+F4 work too.
                case SDL_QUIT: 
                {
                    Run = false;
                    break;
                }
                
                case SDL_KEYDOWN:
                {
                    switch(Event.key.keysym.sym)
                    {
                        case SDLK_UP:
                        {
                            GameInput.PressedKey = KEY_UP;
                            break;
                        }
                        
                        case SDLK_DOWN:
                        {
                            GameInput.PressedKey = KEY_DOWN;
                            break;
                        }
                        
                        case SDLK_LEFT:
                        {
                            GameInput.PressedKey = KEY_LEFT;
                            break;
                        }
                        
                        case SDLK_RIGHT:
                        {
                            GameInput.PressedKey = KEY_RIGHT;
                            break;
                        }
                        
                        case SDLK_SPACE:
                        {
                            GameInput.PressedKey = KEY_SPACE;
                            break;
                        }
                        
                        default:
                        {
                            GameInput.PressedKey = KEY_NOTHING;
                            break; 
                        }
                    }
                    
                    break;
                }
                
                default:
                {
                    // NOTE(annad): Nothing...
                    break;
                }
            }
        }
        
        // NOTE(annad): Main.
        Game.UpdateAndRender(&GameBuffer, &GameInput, &GameMemory, &GameTime);
        
        SDL_UpdateWindowSurface(Window);
        
        // NOTE(annad): Timers.
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
    
    SDL_UnloadObject(GameDLLHandle);
    SDL_CloseAudio();
    SDL_FreeSurface(Buffer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    
    return 0;
}