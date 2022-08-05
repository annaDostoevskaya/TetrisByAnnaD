/* 
Author: github.com/annadostoevskaya
File: main.cpp
Date: August 1st 2022 9:09 pm 

Description: <empty>
*/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef uint32_t u32;
typedef u32 b32;
typedef float r32;

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 540


enum tetrominoes {
    TETROMINO_STRAIGHT = 0,
    TETROMINO_SQUARE,
    TETROMINO_T,
    TETROMINO_L,
    TETROMINO_SKEW,
    
    TETROMINO_TOTAL
};

struct game_state {
    b32 Initialied;
    u32 PosX;
    u32 PosY;
};

void DrawCube(SDL_Surface *Buffer, int32_t X, int32_t Y, int32_t W, int32_t H,
              r32 Red, r32 Green, r32 Blue)
{
    if(W < 0)
    {
        W = 0;
    }
    
    if(W > Buffer->w)
    {
        W = W % Buffer->w;
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
    
    if(X + W > Buffer->w)
    {
        X = Buffer->w - W;
    }
    
    if(H < 0)
    {
        H = 0;
    }
    
    if(H > Buffer->h)
    {
        H = H % Buffer->h;
    }
    
    if(H > Buffer->h)
    {
        H = 0;
    }
    
    if(Y + H < 0)
    {
        Y = 0;
    }
    
    if(Y + H > Buffer->h)
    {
        
        Y = Buffer->h - H;
    }
    
    
    u32 *Pixels = (u32 *)Buffer->pixels;
    
    u32 Color = (0xFF000000 | // A R G B
                 ((u32)(Blue * 255.0f) << 0) |
                 ((u32)(Green * 255.0f) << 8) | 
                 ((u32)(Red * 255.0f) << 16));
    
    for(int32_t i = Y; i < Y + H; i += 1)
    {
        for(int32_t j = X; j < X + W; j += 1)
        {
            Pixels[i * Buffer->w + j] = Color;
        }
    }
}

void UpdateAndRender(SDL_Surface *Buffer, 
                     SDL_Event *Event,
                     game_state *GameState)
{
    // TODO(annad): Timer and 30fps!!!!!!!!!!
    (void)Event;
    (void)Buffer;
    
    if(GameState->Initialied != true)
    {
        GameState->PosX = 0;
        GameState->PosY = 0;
        GameState->Initialied = true;
    }
    
    // tetrominoes base = TETROMINO_STRAIGHT;
    
    u32 cube = 50; // NOTE(annad): Pixels size. 1 cube is 50px.
    (void)cube;
    
    if(Event->type == SDL_KEYDOWN)
    {
        SDL_Keysym *Keysym = &(Event->key.keysym);
        
        if(Keysym->sym == SDLK_LEFT)
        {
            GameState->PosX -= 1; // TODO(annad): Vectors...
        }
        
        if(Keysym->sym == SDLK_RIGHT)
        {
            GameState->PosX += 1;
        }
        
        if(Keysym->sym == SDLK_UP)
        {
            GameState->PosY -= 1;
        }
        
        if(Keysym->sym == SDLK_DOWN)
        {
            GameState->PosY += 1;
        }
    }
    
    DrawCube(Buffer, GameState->PosX , GameState->PosY, 50, 50, 0.5f, 0.5f, 0.5f);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
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
    
    SDL_Event Event = {};
    
    b32 Run = true;
    
    game_state GameState = {};
    
    while(Run)
    {
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
        
        UpdateAndRender(Buffer, &Event, &GameState);
        
        SDL_UpdateWindowSurface(Window);
    }
    
    SDL_FreeSurface(Buffer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    
    return 0;
}