@echo off

set opts=-FC -GR- -EHa- -nologo -Zi 

set code=%cd%
set lib_path=%code%\thirdparty\libs
set include_path=%code%\thirdparty\includes

pushd build
clang-cl %opts% %code%\main.cpp -Femain %lib_path%\SDL2\SDL2.lib %lib_path%\SDL2\SDL2main.lib -I%include_path% shell32.lib /link /SUBSYSTEM:WINDOWS
popd
