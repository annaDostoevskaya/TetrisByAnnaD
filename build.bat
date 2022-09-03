@echo off

utils\ctime.exe -begin tetris.ctm

set optimization_flags=-Ox -O2 -Ob2 -wd4711
set opts_dev=-D_GAME_INTERNAL -Zi
set opts_warnings=-Wall -WX -W3 -wd4018 -wd5045 -wd4996
set opts=-FC -GR- -EHa- -nologo %opts_warnings%  -D_GAME_WIN32 %optimization_flags%

set code=%cd%
set lib_path=%code%\thirdparty\libs
set include_path=%code%\thirdparty\includes

if not exist build mkdir build

pushd build

cl %opts% %code%\sdl_game.cpp -Fesdl_game.exe %lib_path%\SDL2\SDL2.lib %lib_path%\SDL2\SDL2main.lib -I%include_path% shell32.lib /link /SUBSYSTEM:WINDOWS

cl %opts% %code%\game.cpp -Fegame.dll -I%include_path% /LD /link /SUBSYSTEM:WINDOWS -EXPORT:UpdateAndRender -EXPORT:UpdateSoundBuffer

popd

utils\ctime.exe -end tetris.ctm %errorlevel%

