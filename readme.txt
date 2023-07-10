Author: github.com/annadostoevskaya
File: readme.txt
Date: September 4st 2022 5:15 pm

Description:
TetrisByAnnad or tetris-minimal, is a small project 
developed by Me within a month to test the abilities 
received during 0 - 42 lessons of Casey Muratori, 
HandmadeHero.

Here I write a platform layer based on SDL 2.0 from 
scratch, build the architecture of the game, and 
also use some specific technologies, such as 
"SIMD" for my own implementation of "std::memcpy", 
or a "Taylor Series" for sine calculcation.

The logic that Handmade Hero had was saved here. All 
files starting with "sdl_*" refer to the platform 
layer written in SDL 2.0 and contain code that needs 
to be rewritten for the other platforms. All files 
starting with "game_*" contain platform-independent 
code that must not be modified during porting.

The "base_algs.cpp" file and "base_math.cpp" contain extremely 
specific code, and if the "base_math.cpp" is just slow and 
there is a better alternative (the standard library, 
"std::sin" function is faster then "Sin", I checked), 
then the "algs.cpp" file contains code that depends 
on the processor architecture (SSE/AVX) and compiler 
intrinsics. I left it only because it is a valuable 
experience. The "debug.cpp" file contains trash code 
for debugging, just ignore it.

Also, it's worth mentioning that the entire codebase 
has been reviewed by a C++ person, github.com/saiel, 
thanks to him for the advice. Ask if you have any 
questions. 

Good luck!

