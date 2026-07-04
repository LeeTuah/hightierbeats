#!/bin/bash
mkdir -p builds

if [ ! -f builds/audio.o ]; then
    echo "Building Miniaudio library..."
    g++ -c src/audio.cpp -o builds/audio.o -Iinclude -Isrc -O2
fi

if [ ! -f builds/glad.o ]; then
    echo "Building ImGui and GLAD libraries..."
    cd builds
    g++ -c ../src/glad.c ../src/imgui*.cpp -I../src -I../include -O2
    cd ..
fi

echo "Compiling and linking HighTierBeats..."
g++ -std=c++23 src/main.cpp builds/*.o -o src/HighTierBeats.exe -Iinclude -Isrc $(pkg-config --cflags --static --libs freetype2 glfw3) -static -static-libgcc -static-libstdc++ -lopengl32 -lgdi32 -luser32 -lkernel32 -lwinmm -lole32

echo "Build Complete!"
cd src
./HighTierBeats.exe