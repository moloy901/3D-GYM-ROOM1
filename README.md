# 3D Gym Room

A 3D gym room scene built with C++ and OpenGL/GLUT. The project renders a gym environment with textured flooring, gym equipment, lighting controls, camera movement, and simple animations.

## Features

- 3D gym room model rendered with OpenGL
- Textured floor using `green.jpg`
- Animated fan, dumbbells, barbell, treadmill, and window
- Toggleable white and green lights
- Multiple preset camera views
- Keyboard controls for movement and interaction

## Project Structure

```text
.
|-- main.cpp                         # Main OpenGL source file
|-- stb_image.h                      # Image loading library
|-- green.jpg                        # Floor texture
|-- 3D_GYM_ROOM1.cbp                 # Code::Blocks project file
|-- seamless-green-grass-pattern/    # Additional texture/assets
|-- bin/                             # Build output
|-- obj/                             # Object files
```

## Requirements

- Code::Blocks with MinGW, or another C++ compiler setup
- OpenGL
- GLU
- freeglut

The Code::Blocks project is already configured to link:

- `freeglut`
- `opengl32`
- `glu32`
- `winmm`
- `gdi32`

## How to Build and Run

### Using Code::Blocks

1. Open `3D_GYM_ROOM1.cbp` in Code::Blocks.
2. Make sure MinGW and freeglut are installed.
3. Build the project.
4. Run the project from Code::Blocks.

### Using g++ on Windows

If your compiler and freeglut paths are configured, you can compile manually with:

```bash
g++ main.cpp -o 3D_GYM_ROOM1.exe -lfreeglut -lopengl32 -lglu32 -lwinmm -lgdi32
```

Run:

```bash
./3D_GYM_ROOM1.exe
```

Keep `green.jpg` in the same directory as the executable when running, because the program loads it at startup.

## Controls

| Key | Action |
| --- | --- |
| `1` - `6` | Toggle individual dumbbell animations |
| `B` | Toggle barbell animation |
| `7` | Toggle white light |
| `8` | Toggle green light |
| `P` | Toggle fan animation |
| `O` | Open/close window |
| `W` | Move camera forward |
| `S` | Move camera backward |
| `A` | Move camera left |
| `F` | Move camera right |
| `Q` | Move camera up |
| `E` | Move camera down |
| `9` | Switch to dumbbell view |
| `0` | Switch to bench press view |
| `R` | Reset to normal view |
| `Esc` | Exit |

## Notes

- If `green.jpg` cannot be loaded, the floor will use a fallback green color.
- The project uses `stb_image.h`, so no separate image loading library is required.
