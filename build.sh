#!/bin/bash -v

g++ -O3 -Wall -c TCCube.cpp -o TCCube.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c TCAnim.cpp -o TCAnim.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c TCAnimLua.cpp -o TCAnimLua.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c TCDriver.cpp -o TCDriver.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c drivers/netdrv.cpp -o drivers/netdrv.o `pkg-config --cflags sdl lua5.1`

g++ -O3 -Wall -c console.cpp -o console.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c console_commands.cpp -o console_commands.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c format_conversion.cpp -o format_conversion.o `pkg-config --cflags sdl`

g++ -O3 -Wall -fpermissive -c render.cpp -o render.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c events.cpp -o events.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c main.cpp -o main.o `pkg-config --cflags sdl lua5.1`

g++ -O3 -Wall -o triclysm *.o drivers/netdrv.o `pkg-config --libs lua5.1 sdl SDL_net gl glu`

rm *.o
rm drivers/*.o
