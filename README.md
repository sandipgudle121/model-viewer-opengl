# Model Viewer Tool

A standalone Win32/OpenGL-based 3D Model Viewer written in modern C++.

This project is being developed as a graphics programming and engine architecture portfolio project. The application uses the Win32 API for window management, OpenGL 4.6 for rendering, Assimp for model importing, and ImGui for runtime tools and debugging.

## Features

### Rendering

* OpenGL 4.6 Core Profile
* Shader-based rendering pipeline
* Vertex Array Objects (VAO)
* Vertex Buffer Objects (VBO)
* Element Buffer Objects (EBO)
* Wireframe rendering mode
* Depth testing
* Viewport resizing

### Model Loading

* OBJ support
* FBX support
* GLTF support
* GLB support
* Assimp integration
* Automatic mesh processing

### Camera System

* Orbit camera
* FPS camera
* Mouse look
* Zoom controls
* Keyboard navigation

### Editor Tools

* Dear ImGui integration
* Docking interface
* Scene hierarchy
* Model statistics panel
* Material inspector
* Renderer debug information

### File Handling

* Drag & Drop model loading
* Asset browser
* Runtime model switching

---

## Project Structure

```text
ModelViewer/

├── Assets/
│   ├── Models/
│   ├── Textures/
│   └── Shaders/
│
├── Source/
│   ├── Main.cpp
│
│   ├── Core/
│   │   ├── Application.cpp
│   │   ├── Application.h
│   │   ├── Window.cpp
│   │   ├── Window.h
│   │   ├── Logger.cpp
│   │   └── Logger.h
│   │
│   ├── Renderer/
│   │   ├── Renderer.cpp
│   │   ├── Renderer.h
│   │   ├── Shader.cpp
│   │   ├── Shader.h
│   │   ├── Mesh.cpp
│   │   └── Mesh.h
│   │
│   ├── Scene/
│   │   ├── Camera.cpp
│   │   ├── Camera.h
│   │   ├── Model.cpp
│   │   └── Model.h
│   │
│   ├── Importer/
│   │   ├── ModelLoader.cpp
│   │   └── ModelLoader.h
│   │
│   └── UI/
│       ├── ImGuiLayer.cpp
│       └── ImGuiLayer.h
│
├── External/
├── Build/
├── Build.bat
└── README.md
```

---

## Third-Party Libraries

### OpenGL

Graphics rendering API.

### GLEW

OpenGL extension loading library.

### Assimp

Open Asset Import Library used for importing 3D model formats.

### Dear ImGui

Immediate mode graphical user interface library.

---

## Build Requirements

* Windows 10 / Windows 11
* Visual Studio 2022 Community
* Windows SDK
* OpenGL
* GLEW
* Assimp

---

## Build

Open a Developer Command Prompt for Visual Studio and run:

```bat
Build.bat
```

The executable will be generated in the project output directory.

---

## Current Progress

* [x] Win32 Application Framework
* [x] OpenGL Context Creation
* [x] Shader Compilation System
* [x] RGB Triangle Rendering
* [x] Modular Project Architecture
* [x] GLEW Integration
* [x] Camera System
* [x] Assimp Model Loading
* [x] Wireframe Mode
* [ ] GLTF Rendering
* [ ] ImGui Docking
* [ ] Material Inspector
* [ ] Model Statistics
* [ ] Drag & Drop Support

---

## Learning Goals

* Modern OpenGL Rendering
* Graphics Engine Architecture
* Asset Import Pipelines
* Scene Management
* Camera Systems
* Editor Tool Development
* GPU Programming
* Real-Time Rendering Techniques

---

## Screenshots

Screenshots and demonstrations will be added as development progresses.

---

## Author

Sandip Gudle

Graphics Programming • OpenGL • C++ • Win32 SDK
