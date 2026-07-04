# HighTierBeats

<p align="center">
  <img src="assets/htb_logo.png" alt="HighTierBeats Logo" width="350">
</p>

HighTierBeats is a rhythm game built in OpenGL and C++, intended as a passion project for the Hack Club in Macondo.

Refer to the following links for more info on the project

- [How to Play the Game](assets/htb_showcase.mp4)
- [How to Use the Level Editor](assets/mapmaker_showcase.mp4)
- [Screenshots of the project](markdowns/screenshots.md)

## Tech Stack

1. Language: C++
2. Graphics API: OpenGL Version 3.3 Core
3. Libraries: GLFW, GLAD, GLM, freetype2, stb_image.h, miniaudio.h and ImGui

## Gameplay Mechanics

- **Moving the shield orthogonally**: `WASD` or Arrow Keys
- **Moving the shield diagonally**: Press two keyboard input at the same time (eg. `W + D` to face the shield top-right)
- **Menu Navigation**: `W, S` or Arrow Keys
- **Select an Item**: `ENTER` or `SPACE`
- **Pause Game/Back**: `ESC`

# How to Run

> [!IMPORTANT]
> **I have pre-compiled libraries for both Windows and Linux on my releases page. In case your program is crashing/bugging/not responding properly, ONLY THEN try to compile the project.**

## Windows

To run the program on Windows, you have to install the [MSYS2](https://www.msys2.org/) compiler on your system. The [MSYS2](https://www.msys2.org/) page has a tutorial for installing their tool, so you should not face any issues.

After installing, open MSYS2 UCRT64 on your pc.
Then, run the following command to download all the libraries that will be needed by the program.

```
pacman -S --needed mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-pkg-config mingw-w64-ucrt-x86_64-glfw mingw-w64-ucrt-x86_64-freetype mingw-w64-ucrt-x86_64-harfbuzz mingw-w64-ucrt-x86_64-glm mingw-w64-ucrt-x86_64-nlohmann-json
```

After doing that, close the UCRT64 compiler. In the releases section, I have a zip file linked called glad_includes.zip. Download it on your pc.
Then, navigate to the following folder using file explorer:

```
C:\msys64\ucrt64\include
```

Paste and extract the zip file there. Then delete the zip file.
Now, open the MSYS2 UCRT64 terminal again, and navigate to the location where you cloned the repository.

eg. the repo is in your C: drive, then

```
cd /c
cd /path/to/the/repo/hightierbeats/
```

Now, run the following command:

```
bash build_windows.sh
```
It will take ~1 minute for the entire program to compile. And then, it should run.

## Linux

First, install the required libraries using the following command:

Debian/Ubuntu (and all OSes similar to it):
```
sudo apt update && sudo apt install -y build-essential pkg-config libglfw3-dev libfreetype-dev libharfbuzz-dev libgl1-mesa-dev libasound2-dev
```

Fedora (and all OSes similar to it):
```
sudo dnf install -y gcc-c++ pkgconf-pkg-config glfw-devel freetype-devel harfbuzz-devel mesa-libGL-devel alsa-lib-devel
```

Arch (and all OSes similar to it):
```
sudo pacman -S --needed base-devel pkgconf glfw-x11 freetype2 harfbuzz mesa alsa-lib
```

Then, go to the releases section, download the glad_includes.zip file, and extract it in your includes directory (/usr/includes for system-wide installation, might vary based on your system.)

Then, navigate to the cloned repository folder, and then run the following command:
```
bash build.sh
```

It will take a good minute to compile everything, and then the program should run itself automatically.