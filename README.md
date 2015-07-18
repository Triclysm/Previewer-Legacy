# Triclysm

Triclysm is an environment for developing and controlling animations on real-world three dimensional displays (LED cubes).  Triclysm, itself written in C++ (using SDL/OpenGL) provides a 3D preview of the LED cube itself and various debugging interfaces for developing animations.  Furthermore, it allows you to stream animations in real-time over any interface (e.g. your home network over Wi-Fi, [example video](https://www.youtube.com/watch?v=aIJNYIJbbjw)) to a physical LED cube, and future work will also include a music visualizer.

Triclysm works on Windows, Linux, and OSX, and requires SDL and OpenGL support.  Firmware for Atmel microcontrollers will be provided in the future, as well as hardware schematics for building a physical LED cube (more information coming soon).

-------

## Compiling

To compile Triclysm, use the script `./build.sh`.  Triclysm requires the following libraries to compile:

 - SDL
 - SDL_net
 - OpenGL
 - Lua 5.1

Once the prerequisites are obtained, you can build Triclysm by calling the build script.  Triclysm can then be launched directly from the executable (e.g. `./triclysm`).

## Quickstart

To change the window resolution, or toggle fullscreen mode, modify the `config.tcs` file.  Once Triclysm is open, the default config file will load the `sendplane` animation.  Hit `f` to show/hide the current framerate.

Hit the tilde key to open/close the console.  In the console, type `help [command]` for help with particular commands (e.g. `help loadanim` or `help bind`).  Some example commands to try:

Some example commands to try:

 - `loadanim fillplane`
 - `cubesize 6x8x10`
 - `showfps`
 - `showaxis`
 - `quit`

You can bind console commands to keys as well with the `bind` command.  See the `config.tcs` file for examples, as well as the default key configuration.  To quit Triclysm, hit the escape key *twice*, or use the `quit` console command.  You can also bind a key of your preference to the quit command if you prefer (e.g. `bind q quit`).

-------



-------

### Screenshots

[<img src="https://github.com/Triclysm/Previewer-Legacy/raw/resources/screenshots/tc-1.png" alt="" width="450" />](https://github.com/Triclysm/Previewer-Legacy/raw/resources/screenshots/tc-1.png)

[<img src="https://github.com/Triclysm/Previewer-Legacy/raw/resources/screenshots/tc-2.png" alt="" width="450" />](https://github.com/Triclysm/Previewer-Legacy/raw/resources/screenshots/tc-2.png)

[<img src="https://github.com/Triclysm/Previewer-Legacy/raw/resources/screenshots/tc-3.png" alt="" width="450" />](https://github.com/Triclysm/Previewer-Legacy/raw/resources/screenshots/tc-3.png)


-------

This project is released under the BSD 2-Clause license; see the included LICENSE file for details.

Copyright (C) 2011-2015 Brandon Castellano, Ryan Mantha.
All rights reserved.
