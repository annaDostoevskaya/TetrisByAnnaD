/* 
Author: github.com/annadostoevskaya
File: sdl_game.cpp
Date: August 6th 2022 3:26 pm 

Description: <empty>
*/

// TODO(annad): Sound!

#ifdef _GAME_INTERNAL
#ifdef _GAME_WIN32
#include <windows.h>
#endif // _GAME_WIN32
#ifdef _GAME_LINUX
void OutputDebugString(const char *DebugString) {}
#endif // _GAME_LINUX
#include <stdio.h>
#include <math.h>
#endif // _GAME_INTERNAL

#ifndef _GAME_INTERNAL
void OutputDebugString(const char *) {}
#endif

#include "base_types.h"
#include "platform_game.h"
#include "sdl_game.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#define SCREEN_WIDTH 1920/2
#define SCREEN_HEIGHT 1080/2

#define FREQ 44100
#define SAMPLES 2048

void MixAudio(void *Udata, Uint8 *Stream, int StreamLen)
{
    sdl_mix_audio_user_data *SDLMixAudioUserData = (sdl_mix_audio_user_data *)Udata;
    game_memory *GameMemory = SDLMixAudioUserData->GameMemory;
    game_update_sound_buffer *GameUpdateSoundBuffer = SDLMixAudioUserData->GameUpdateSoundBuffer;
    
    game_sound_buffer GameSoundBuffer = {}; 
    GameSoundBuffer.Stream = (i16*)Stream;
    GameSoundBuffer.StreamLen = StreamLen / 2;
    GameSoundBuffer.Frequency = FREQ;
    GameSoundBuffer.Samples = SAMPLES;
    
    GameUpdateSoundBuffer(GameMemory, &GameSoundBuffer);
}

int main(int Argc, char **Argv)
{
    // NOTE(annad): Init platform layer section.
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0)
    {
        OutputDebugString("Failed initialize SDL!");
        return -1;
    }
    
    SDL_Window *Window = NULL;
    SDL_Surface *ScreenBuffer = NULL;
    
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
    
    ScreenBuffer= SDL_GetWindowSurface(Window);
    if(ScreenBuffer == NULL)
    {
        OutputDebugString("Surface could not be created!");
        return -1;
    }
    
    // NOTE(annad): Buffer for rendering to window.
    game_screen_buffer GameScreenBuffer = {};
    GameScreenBuffer.Width = ScreenBuffer->w;
    GameScreenBuffer.Height = ScreenBuffer->h;
    GameScreenBuffer.Pitch = ScreenBuffer->pitch;
    GameScreenBuffer.Memory = ScreenBuffer->pixels;
    
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
    
    // NOTE(annad): Load game code.
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
        OutputDebugString("Can't load \"UpdateAndRender\" from DLL file!");
        return -1;
    }
    
    Game.UpdateSoundBuffer = (game_update_sound_buffer*)SDL_LoadFunction(GameDLLHandle, "UpdateSoundBuffer");
    if(!Game.UpdateSoundBuffer)
    {
        OutputDebugString("Can't load \"UpdateSoundBuffer\" DLL file!");
        return -1;
    }
    
    // NOTE(annad): Game Memory Section.
    game_memory GameMemory = {};
    GameMemory.PermanentStorageSize = PERMANENT_STORAGE_SIZE;
    
    //
    // sdl_audio
    //
    
    SDL_AudioSpec AudioFormat;
    
    AudioFormat.freq = FREQ; // freq is samples per seconds.
    AudioFormat.format = AUDIO_S16; // sample is 16 bit, dependency format.
    AudioFormat.channels = 1; // 16 bit per channel.
    AudioFormat.samples = SAMPLES;
    
    sdl_mix_audio_user_data SDLMixAudioUserData = {};
    SDLMixAudioUserData.GameUpdateSoundBuffer = Game.UpdateSoundBuffer;
    SDLMixAudioUserData.GameMemory = &GameMemory;
    
    AudioFormat.callback = MixAudio;
    AudioFormat.userdata = (void*)(&SDLMixAudioUserData);
    
    if(SDL_OpenAudio(&AudioFormat, NULL) < 0)
    {
        OutputDebugString("Unable to open audio");
        return -1;
    }
    
    SDL_PauseAudio(0);
    
    //
    // sdl_audio
    //
    
    b32 Run = true;
    while(Run)
    {
        // NOTE(annad): Cleaning all data from previous frame.
        SDL_FillRect(ScreenBuffer, NULL, SDL_MapRGB(ScreenBuffer->format, 0, 0, 0));
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
                        case SDLK_DOWN:
                        {
                            GameInput.PressedKey = KEY_MOVE_DOWN;
                            break;
                        }
                        
                        case SDLK_LEFT:
                        {
                            GameInput.PressedKey = KEY_MOVE_LEFT;
                            break;
                        }
                        
                        case SDLK_RIGHT:
                        {
                            GameInput.PressedKey = KEY_MOVE_RIGHT;
                            break;
                        }
                        
                        case SDLK_z:
                        {
                            GameInput.PressedKey = KEY_ROTATE_LEFT;
                            break;
                        }
                        
                        case SDLK_x:
                        {
                            GameInput.PressedKey = KEY_ROTATE_RIGHT;
                            break;
                        }
#ifdef _GAME_INTERNAL
                        case SDLK_SPACE:
                        {
                            GameInput.PressedKey = KEY_SPACE;
                            break;
                        }
#endif
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
        Game.UpdateAndRender(&GameScreenBuffer, &GameInput, &GameMemory, &GameTime);
        
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
    SDL_FreeSurface(ScreenBuffer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    
    return 0;
}