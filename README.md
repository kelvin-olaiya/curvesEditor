# CurvesEditor

OpenGL editor for Hermite interpolating and Bézier approximating curves.

Runs on Windows, macOS and Linux. It uses [GLFW](https://www.glfw.org/) for the window,
[glad](https://github.com/Dav1dde/glad) to load OpenGL 3.3 core, [GLM](https://github.com/g-truc/glm)
for math and [Dear ImGui](https://github.com/ocornut/imgui) for the right-click menu.

## Requirements

- CMake ≥ 3.16 and a C++17 compiler (MSVC, Clang or GCC)
- An internet connection on the first configure: GLFW, GLM and ImGui are downloaded automatically
  (glad is already in `third_party/`)

On Linux you also need the headers GLFW builds against:

```sh
# Debian/Ubuntu
sudo apt install build-essential cmake libgl-dev libx11-dev libxrandr-dev libxinerama-dev \
  libxcursor-dev libxi-dev libwayland-dev libxkbcommon-dev wayland-protocols
```

## Build & run

```sh
cmake -S . -B build
cmake --build build --config Release
```

Then run it from the directory holding the executable (the shaders are copied there and loaded by
relative path):

```sh
cd build && ./CurvesEditor                 # macOS / Linux
cd build\Release && CurvesEditor.exe       # Windows (Visual Studio generator)
```

## Usage

- **Right click**: menu (interaction mode, Hermite / Bézier, export)
- **Left click**: insert / select / delete a control point, depending on the mode
- **Drag** (in "Sposta" mode): move the selected point
- **t/T, b/B, c/C** (with "modifica tangenti" on and a point selected): tension, bias, continuity
- **Export** writes the control points to `output.txt` in the working directory
