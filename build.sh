#!/bin/bash -v
#
# Triclysm Build Script
#
g++ -O3 -Wall -c src/TCCube.cpp -o src/TCCube.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c src/TCAnim.cpp -o src/TCAnim.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c src/TCAnimLua.cpp -o src/TCAnimLua.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c src/TCDriver.cpp -o src/TCDriver.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c src/drivers/netdrv.cpp -o src/drivers/netdrv.o `pkg-config --cflags sdl lua5.1`

g++ -O3 -Wall -c src/console.cpp -o src/console.o `pkg-config --cflags sdl`
g++ -O3 -Wall -c src/console_commands.cpp -o src/console_commands.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c src/format_conversion.cpp -o src/format_conversion.o `pkg-config --cflags sdl`

g++ -O3 -Wall -fpermissive -c src/render.cpp -o src/render.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c src/events.cpp -o src/events.o `pkg-config --cflags sdl lua5.1`
g++ -O3 -Wall -c src/main.cpp -o src/main.o `pkg-config --cflags sdl lua5.1`

g++ -O3 -Wall -o triclysm src/*.o src/drivers/netdrv.o `pkg-config --libs lua5.1 sdl SDL_net gl glu`

# Remove temp. files
rm src/*.o
rm src/drivers/*.o
