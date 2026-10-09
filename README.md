# CurvesEditor

A small interactive editor for 2D parametric curves, written in C++ and OpenGL.

Click to place control points and see the curve update live:

- **Hermite**: an interpolating curve through every point, with per-point tension, bias and continuity
- **Bézier**: an approximating curve evaluated with De Casteljau's algorithm

Control points can be exported to `output.txt`.

## Build

```sh
cmake -S . -B build && cmake --build build
cd build && ./CurvesEditor
```

Dependencies are fetched by CMake. On Linux, install the X11/Wayland dev headers GLFW needs first.

## Controls

| Input | Action |
| --- | --- |
| Right click | Menu: mode, curve type, export |
| Left click | Add, select or delete a point |
| Drag | Move the selected point |
| `t`/`T` `b`/`B` `c`/`C` | Tension, bias, continuity of the selected point (with tangent editing on) |
