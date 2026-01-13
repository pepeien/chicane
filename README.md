<p align="center">
    <img src="https://raw.githubusercontent.com/pepeien/chicane/master/.github/images/logo.svg" alt="Chicane logo" />
</p>

<p align="center">
    <span>Chicane is a lightweight, cross-platform C++17 graphics engine with a focus on modularity and flexibility</span>
</p>

## Features
- Multiplataform:
    - Windows (^=10);
    - Linux.
- Asset system: 
    - Audio:
        - Waves Audio (.wav).
    - Fonts:
        - TrueType Font (.ttf).
    - 3D Models:
        - Wavefront (.obj).
    - Textures:
        - JPG (.jpg);
        - PNG (.png).
- UI system:
    - Button;
    - Container;
    - Popup;
    - Text;
    - Input;
    - View.
- Graphics API support:
    - OpenGL:
        - Shadow Mapping. 
    - Vulkan:
        - Shadow mapping.

## Compiling
This project uses CMake to generate and build files, it should pretty straight forward both on linux and windows platforms.

### Compilers
- Windows:
    - [MinGW](https://www.mingw-w64.org/downloads) (^=11.2);
    - [Visual Studio](https://visualstudio.microsoft.com/downloads) (^=19).
- Linux:
    - [MinGW](https://www.mingw-w64.org/downloads) (^=11.2);
    - [Clang](https://releases.llvm.org/download.html) (^=17);
    - [GCC](https://gcc.gnu.org/install/binaries.html) (^=14.20).

### Requeriments
- Build:
    - [CMake](https://cmake.org/download) (^=3.16.0).
- Script:
    - [Doxygen](https://www.doxygen.nl/download.html) (1.12.0);
    - [Python](https://www.python.org/downloads) (^=3.12.0).
- API:
    - [OpenGL](https://www.opengl.org/sdk) (^=4.5);
    - [Vulkan](https://www.lunarg.com/vulkan-sdk) (^=1.4.313.2).
- Code:
    - [clang-format](https://releases.llvm.org/download.html) (^=21).

## Examples
If you desire to check some examplary code, this repository hosts a sample located at `Sources/Sample`.

## Documentation
Further documentation is hosted at [Chicane Engine Documentation](https://chicane.erickcaldeira.com), available in:
- [English](https://chicane.erickcaldeira.com/en-us);
- [Japanese](https://chicane.erickcaldeira.com/ja-jp);
- [Portuguese](https://chicane.erickcaldeira.com/pt-br).
