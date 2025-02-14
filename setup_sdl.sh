#!/bin/bash

# Create directories
mkdir -p resource/frameworks/temp
cd resource/frameworks/temp

# Download SDL2
curl -LO https://github.com/libsdl-org/SDL/releases/download/release-2.30.0/SDL2-2.30.0.tar.gz

# Extract
tar xzf SDL2-2.30.0.tar.gz
cd SDL2-2.30.0

# Configure and build
./configure --prefix=$(pwd)/build
make

# Install to local build directory
make install

# Copy the shared library to frameworks directory
cp build/lib/libSDL2-2.0.so.0 ../../libSDL2.so
cp build/lib/libSDL2-2.0.so.0 ../../libSDL2-2.0.so.0

# Create symbolic link
cd ../..
ln -sf libSDL2-2.0.so.0 libSDL2.so

# Clean up
rm -rf temp

cd ../..