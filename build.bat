@echo off
if not exist builds mkdir builds

if not exist builds\audio.o (
    echo Building Miniaudio library...
    g++ -c src\audio.cpp -o builds\audio.o -Iinclude -Isrc -O2
)

if not exist builds\glad.o (
    echo Building ImGui and GLAD libraries...
    cd builds
    g++ -c ..\src\glad.c ..\src\imgui*.cpp -I..\src -I..\include -O2
    cd ..
)

echo Compiling and linking HighTierBeats...
g++ -std=c++23 src\main.cpp builds\*.o -o src\HighTierBeats.exe -Iinclude -Isrc -static -static-libgcc -static-libstdc++ -lglfw3 -lfreetype -lopengl32 -lgdi32 -luser32 -lkernel32

echo Build Complete!

cd src
.\HighTierBeats.exe