![Framework Logo](git_images/logo.svg)

#
### A remake of the rapid-prototyping framework from 2D Boy.

- What is it?

    - This is a remake and slightly modified version of the framework used in the creation of [World of Goo](https://en.wikipedia.org/wiki/World_of_Goo).
    - All the stuff that 99.9% of 2D games have in common has been pulled out into this reusable framework. It can render 2D graphics, read input from a variety of devices, and play sounds. It includes a resource manager for loading/unloading resources, a persistence layer for saving game state, and even a file access abstraction. The included demo is a quick remake of the classic arcade game [Asteroids](https://en.wikipedia.org/wiki/Asteroids_(video_game)) made in 12 hours.

    ![Demo Screenshot](git_images/screenshot.png)

> "We hope this inspires people to play around with new ideas and saves them the annoyance of having to reinvent the wheel." 
> -2D Boy

### Misc Notes

- The font file format is that used by the Popcap
Framework, which comes with a font builder tool. 
- The resource manifest (resource.xml) also uses a simpliefied version of the format available in the Popcap Framework.
- There is no documentation for this framework yet. There are inline comments that should help you if you're looking at the innards.
- Persistence layer data is saved in `C:\Users\<username>\AppData\Local\Boy` in Windows, for Linux it will save on the directory the executable run as `save/`.

# How to build the demo (MSYS2, Ubuntu, and Linux Mint)

- First, we'll need to have a C++ compiler like `clang`, `make` and `cmake` installed and these libraries for it to build: `libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libcurl4-openssl-dev libopengl-dev`
    - It's also recommended to have `build-essential` installed.
    - `libopengl-dev` maybe not needed in MSYS2.
- Then open a terminal inside the main directory of the project and run this command: `cmake -B build -DCMAKE_BUILD_TYPE=Debug && cmake --build build -j8` 
    - Set -j for how many threads it uses during compilation, it will compile faster.
    - Default build is `Debug`, but it can be changed to these: `Release`, `RelWithDebInfo`, and `MinSizeRel`.

**Note that these compile instructions are subject to change, as this framework is still in development.**