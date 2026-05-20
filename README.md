# OpenGL Chess Scene
## Game Engine Foundations

### Group Members
- VJ Daniel Uy
- John Templonuevo
- Ayub Abdelgawad
- Liyyu Satuhati
- Kaung Khant San

---

## Project Description
This project is a simple OpenGL chess-themed scene created using C++, GLFW, GLEW, and OpenGL.

The purpose of the project is to demonstrate understanding of the OpenGL rendering pipeline, rendering primitives, buffer objects, and collaborative software development practices.

The application renders a basic chess environment composed of multiple geometric primitives. Chess pieces are constructed procedurally using OpenGL primitive shapes including polygons, triangles, and rectangles.

---

## Features
- GLFW window creation
- GLEW initialization
- OpenGL rendering loop
- VAO and VBO implementation
- Vertex and Fragment shaders
- Procedurally generated primitives
- Chess-themed scene composition
- Multiple renderable objects
- Simple object organization structure

---

## Technologies Used
- C++
- OpenGL
- GLFW
- GLEW
- Visual Studio 2026
- GitHub

---

## Folder Structure

```txt
ProjectRoot/
│
├── Dependencies/
│   ├── glew
│   └── glfw
├── Assets/
├── Source/
│   ├── Pieces/
│   │   ├── pawn.cpp
│   │   ├── pawn.h
│   │   ├── rook.cpp
│   │   ├── rook.h
│   │   ├── bishop.cpp
│   │   ├── bishop.h
│   │   ├── horse.cpp
│   │   ├── horse.h
│   │   ├── king.cpp
│   │   ├── king.h
│   │   ├── queen.cpp
│   │   └── queen.h
│   ├── Scene/
│   │   ├── chessBoard.cpp
│   │   └── chessBoard.h
│   ├── Graphics/
│   │   ├── renderer.cpp
│   │   ├── renderer.h
│   │   ├── shader.cpp
│   │   └── shader.h
│   └── main.cpp
├── README.md
├── group3.sln
└── .gitignore
```

---

## Instructions

### Requirements
- Visual Studio 2026
- OpenGL compatible GPU
- GLFW
- GLEW

### Dependencies Setup

#### Linker Input
Add the following libraries:
```txt
opengl32.lib
glfw3.lib
glew32s.lib
user32.lib
gdi.lib
shell32.lib
```

#### Include Directories
```txt
Dependencies/glew/include
Dependencies/glfw/include
```

#### Library Directories
```txt
Dependencies/glew/lib
Dependencies/glfw/lib
```

## Screenshots
