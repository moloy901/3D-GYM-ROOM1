# 3D Gym Room

An interactive 3D gym room built with C++ and OpenGL/GLUT. The project renders a textured room, gym equipment, animated people, lighting, shadows, sound, and movable camera views.

## Features

- Textured floor and three textured walls
- 3D dumbbell rack, bench press, treadmill, pull-up bar, fans, door, and window
- Animated dumbbells, barbell, pull-up person, treadmill runner, fans, door, and window
- Bench press plate sound effect
- Toggleable white and green lights
- Material shininess, transparent glass, mirror shine, and ground shadows
- Wall poster, gym title, analog clock, tube light, and light indicators
- Multiple preset camera views
- Keyboard controls for movement and interaction

## Project Structure

```text
.
|-- main.cpp              # Main OpenGL source code
|-- stb_image.h           # Image loading library
|-- green.jpg             # Floor texture
|-- front.png             # Front wall texture
|-- wall.png              # Left wall texture
|-- right_wall.png        # Right wall texture
|-- plates.mp3            # Bench press sound
|-- 3D_GYM_ROOM1.cbp      # Code::Blocks project file
|-- bin/                  # Build output
|-- obj/                  # Object files
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
.\3D_GYM_ROOM1.exe
```

Keep these runtime assets beside the executable:

- `green.jpg`
- `front.png`
- `wall.png`
- `right_wall.png`
- `plates.mp3`

## Controls

| Key | Action |
| --- | --- |
| `1` - `6` | Toggle individual dumbbell animations |
| `B` | Toggle barbell animation with `plates.mp3` sound |
| `U` | Toggle pull-up animation |
| `7` | Toggle white light |
| `8` | Toggle green light |
| `P` | Turn the ceiling fans ON/OFF |
| `T` | Toggle treadmill and runner animation |
| `D` | Open/close door |
| `O` | Open/close window |
| `W` | Move camera forward |
| `S` | Move camera backward |
| `A` | Move camera left |
| `F` | Move camera right |
| `Q` | Move camera up |
| `E` | Move camera down |
| `Up Arrow` | Look up |
| `Down Arrow` | Look down |
| `Left Arrow` | Look left |
| `Right Arrow` | Look right |
| `9` | Switch to dumbbell view |
| `0` | Switch to bench press view |
| `V` | Switch to treadmill view |
| `N` | Switch to opposite treadmill view |
| `R` | Reset to normal view |
| `Esc` | Exit |

## Main Functions

The source contains 40 user-defined functions, grouped by purpose:

| Group | Important functions | Purpose |
| --- | --- | --- |
| Camera | `setNormalView`, `setDumbbellView`, `setBenchPressView`, `setTreadmillView` | Select preset camera positions |
| Basic drawing | `drawCube`, `drawCylinder`, `drawLimbBetween` | Create reusable 3D shapes and body limbs |
| Texture and light | `loadTexture`, `setupLights`, `setMaterialShininess` | Load images and configure lighting/materials |
| Room | `drawRoom`, `drawDoor`, `drawWindow`, `drawMirror`, `drawTubeLight` | Render the gym environment |
| Equipment | `drawDumbbellRack`, `drawBenchPress`, `drawTreadmill`, `drawPullUpBar`, `drawFan` | Render gym machines and equipment |
| People | `drawManOnBench`, `drawPullUpMan`, `drawTreadmillRunner` | Render animated human models |
| GLUT callbacks | `display`, `update`, `keyboard`, `specialKeyboard`, `reshape` | Render frames, update animation, and handle input |
| Program setup | `init`, `main` | Initialize OpenGL and start the GLUT loop |

## How It Works

1. `main()` creates the GLUT window and registers callback functions.
2. `init()` enables depth testing, loads textures, and sets the background color.
3. `display()` positions the camera and draws the complete gym scene.
4. `update()` changes animation values approximately every 16 milliseconds.
5. Keyboard callbacks toggle animations, lights, doors, windows, and camera views.

## Notes

- If `green.jpg` cannot be loaded, the floor will use a fallback green color.
- Missing wall textures use fallback wall colors.
- Texture images are loaded by `stb_image.h`.
- `plates.mp3` is played through the Windows multimedia library.
- The program uses double buffering and depth testing for smoother 3D rendering.
