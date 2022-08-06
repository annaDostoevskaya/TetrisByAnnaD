@echo off

utils\ctime.exe -begin tetris.ctm

set opts=-FC -GR- -EHa- -nologo -Zi -Wall -WX -W3 -wd4018 -wd5045 -wd4996

set code=%cd%
set lib_path=%code%\thirdparty\libs
set include_path=%code%\thirdparty\includes

pushd build
cl %opts% %code%\game.cpp -Fegame %lib_path%\SDL2\SDL2.lib %lib_path%\SDL2\SDL2main.lib -I%include_path% shell32.lib /link /SUBSYSTEM:WINDOWS
popd

utils\ctime.exe -end tetris.ctm %errorlevel%