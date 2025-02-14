#!/bin/bash

set -x  # This will show us each command as it executes

# Create directories
mkdir -p resource/frameworks/temp
cd resource/frameworks/temp

# Download SDL2
curl -LO https://github.com/libsdl-org/SDL/releases/download/release-2.30.0/SDL2-2.30.0.tar.gz

# Extract
tar xzf SDL2-2.30.0.tar.gz
cd SDL2-2.30.0

# Configure and build with specific architecture flags
CFLAGS="-fPIC" ./configure --prefix=$(pwd)/build \
    --build=x86_64-linux-gnu \
    --enable-shared \
    --disable-static

make

# Let's see what we have after build
echo "=== Directory structure after build ==="
ls -la
find . -name "libSDL2*.so*"

# Install and check installation directory
make install
echo "=== Directory structure after install ==="
ls -la build/lib/

# Copy the library - adjusting path based on actual location
if [ -f "build/lib/libSDL2-2.0.so.0" ]; then
    cp build/lib/libSDL2-2.0.so.0 ../../libSDL2-2.0.so.0
elif [ -f ".libs/libSDL2-2.0.so.0" ]; then
    cp .libs/libSDL2-2.0.so.0 ../../libSDL2-2.0.so.0
else
    echo "ERROR: Cannot find SDL2 library. Here are all .so files:"
    find . -name "*.so*"
fi

cd ../..

# Create necessary symbolic links if the file exists
if [ -f "libSDL2-2.0.so.0" ]; then
    ln -sf libSDL2-2.0.so.0 libSDL2.so
fi

# Don't clean up yet so we can inspect
echo "=== Final frameworks directory ==="
ls -la

cd ../..