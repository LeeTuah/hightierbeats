#!/bin/bash

mkdir -p builds

if [ ! -f "builds/audio.o" ]; then
    echo "Building Miniaudio library..."
    g++ -c src/audio.cpp -o builds/audio.o -Iinclude -Isrc -O2
fi

if [ ! -f "builds/glad.o" ] || [ ! -f "builds/imgui.o" ]; then
    echo "Building ImGui and GLAD libraries..."
    cd builds
    g++ -c ../src/glad.c ../src/imgui*.cpp -I../src -I../include -O2
    cd ..
fi

echo "Compiling and linking HighTierBeats..."
g++ -std=c++23 src/main.cpp builds/*.o -o src/HighTierBeats -Iinclude -Isrc -lglfw -lGL -lfreetype -lm -lpthread -ldl

echo "Build Complete!"

cd src
./HighTierBeats