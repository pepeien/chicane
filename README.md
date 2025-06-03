<p align="center">
    <img src="https://raw.githubusercontent.com/pepeien/chicane/master/.github/images/logo.svg" alt="Chicane logo" />
</p>

<p align="center">
    <span>Chicane is a very naive C++17 cross plataform and graphics API independent graphics engine.</span>
</p>

## Features

- Multiplataform (Window, Linux);
- Shadow mapping;
- Custom Asset system (Box): 
    - Audio (.wav);
    - Fonts (.ttf);
    - 3D Models (.obj);
    - Textures (.png).
- Custom UI system (Grid);
    - Button;
    - Container;
    - Popup;
    - Text;
    - Input;
    - View.
- Graphics APIs support:
    - Vulkan.

## Compiling
This project uses CMake to generate and build files, it should pretty straight forward both on linux and windows platforms.

### Compilers

- Windows - (MinGW, Visual Studio 19 or 22);
- Linux - (GCC or G++).

### Requeriments

- [CMake](https://cmake.org/download) (^=3.16.0);
- [Python 3](https://www.python.org/downloads/);
- [Vulkan SDK](https://www.lunarg.com/vulkan-sdk/) (^=1.3.269).

## Examples
If you desire to check some examplary code, this repository hosts a sample located at `Sources/Sample`.

## Documentation
Further documentation is hosted at [Chicane Engine Documentation](https://chicane.erickfrederick.com), available in English (en-us), Japanese (ja-jp) and Brazillian Portuguese (pt-br).
