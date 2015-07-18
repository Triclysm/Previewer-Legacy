#!/bin/sh

#
# Triclysm Previewer - Build Script
#

# Note that this will (eventually) be replaced by a proper Makefile!

echo "Building Triclysm Previewer..."

CC="g++"
# Suppress warnings for release build.
#CFLAGS="-O3 -Wall"
CFLAGS="-O3"
CINCLUDE=`pkg-config --cflags sdl lua5.1`
CLIBS=`pkg-config --libs sdl SDL_net gl glu lua5.1`

# Build individual object files.
$CC $CFLAGS -c src/main.cpp -o src/main.o $CINCLUDE
$CC $CFLAGS -c src/events.cpp -o src/events.o $CINCLUDE
$CC $CFLAGS -fpermissive -c src/render.cpp -o src/render.o $CINCLUDE

$CC $CFLAGS -c src/console.cpp -o src/console.o $CINCLUDE
$CC $CFLAGS -c src/console_commands.cpp -o src/console_commands.o $CINCLUDE
$CC $CFLAGS -c src/format_conversion.cpp -o src/format_conversion.o $CINCLUDE

$CC $CFLAGS -c src/TCCube.cpp -o src/TCCube.o $CINCLUDE
$CC $CFLAGS -c src/TCAnim.cpp -o src/TCAnim.o $CINCLUDE
$CC $CFLAGS -c src/TCAnimLua.cpp -o src/TCAnimLua.o $CINCLUDE
$CC $CFLAGS -c src/TCDriver.cpp -o src/TCDriver.o $CINCLUDE

$CC $CFLAGS -c src/drivers/netdrv.cpp -o src/drivers/netdrv.o $CINCLUDE

# Remove old binary, and link object files into new binary.
rm -f ./triclysm
$CC $CFLAGS -o triclysm src/*.o src/drivers/netdrv.o $CLIBS

# Remove temporary object files.
rm -f src/*.o
rm -f src/drivers/*.o

# Indicate if build was successful or not (if final binary exists).
echo " "
if [ -f ./triclysm ]; then
    echo "Build SUCCESS, executable built successfully!"
    echo "(you can start Triclysm by running ./triclysm)"
else
    echo "Build FAILED, check above output for errors and try again."
fi
echo " "
