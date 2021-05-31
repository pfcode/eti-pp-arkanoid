# eti-pp-arkanoid
Clone of a popular 2D game "Arkanoid", made as a project for Basics of Programming 
on ETI (https://eti.pg.edu.pl) during the first semester in 2014.

### Dependencies
To compile on Windows:
* MSVC compiler with CMake support
* SDL2 development library for Visual C++ (https://www.libsdl.org/download-2.0.php)

To compile on Linux (and other *nixes):
* G++ compiler (4.9.2 tested)
* libsdl2-devel package (or similar, depending on distro that you use)

### Project structure
* `src/` - source code of the game
* `res/` - graphics and a level.txt file
* `lib/` - a place for SDL2 library distribution when using MSVC 

In `src/defines.h` you can change a lot of parameters defining behaviour of the game, including speed parameters,
position of elements, limits for amount of entities visible simultaneously, paths to resources, etc.

The `res/levels.txt` file, as well as other resources referenced in `src/defines.h`, are required for the game to run.

### License
This project is distributed under the MIT license.