@REM Author: github.com/annadostoevskaya
@REM File: build.bat
@REM August 6th 2022 3:38 pm 
@REM 
@REM Description: 
@REM This is a script file for building the "TetrisByAnnaD" project.
@REM In order to run the program, next to the executable file must be DLL - SDL.DLL, 
@REM which is located in the ./thirdparty/libs/SDL2.
@REM Put the library next to the executable after compilation.


@echo off

utils\ctime.exe -begin tetris.ctm

set SRC=%cd%
set LIB_PATH=%SRC%\thirdparty\libs
set INC_PATH=%SRC%\thirdparty\includes
set DEV_FLAGS=-D_GAME_INTERNAL -Zi -Wall -WX -W3 -wd4018 -wd5045 -wd4996

set OUTPUT_DIR=build
set O_FLAGS=-Ox -O2 -Ob2 -wd4711
set SDL_FLAGS=%LIB_PATH%\SDL2\SDL2.lib %LIB_PATH%\SDL2\SDL2main.lib
set OPTS=-FC -GR- -EHa- -nologo -D_GAME_WIN32 %DEV_FLAGS%

if NOT exist %OUTPUT_DIR% (
  mkdir %OUTPUT_DIR%
)

pushd %OUTPUT_DIR%

cl %OPTS% %SRC%\sdl_game.cpp -Fesdl_game.exe %SDL_FLAGS% -I%INC_PATH% shell32.lib^
  /link /SUBSYSTEM:WINDOWS

cl %OPTS% %SRC%\game.cpp -Fegame.dll -I%INC_PATH% -LD^
  /link /SUBSYSTEM:WINDOWS -EXPORT:UpdateAndRender -EXPORT:UpdateSoundBuffer

popd

utils\ctime.exe -end tetris.ctm %errorlevel%

