# Spiegel

A Basic Game Engine written in C++.

It is a learning project followed by the tutorial of [The Cherno’s Game Engine Series](https://www.youtube.com/user/TheChernoProject) on YouTube and [LearnOpenGL](https://learnopengl.com/) created by Joey de Vries.

## Features

- [X] 2D Rendering API (OpenGL)
  - [X] 2D Batch Rendering (Sprites, Textures, Circles, Lines, etc.)
  - [X] 2D Physics (Box2D)
  - [X] Text Rendering (Use FreeType to import .ttf fonts)
- [X] Basic Light (Directional, Point, Spot)
- [X] Basic Scripting (C++)
  - [X] Accept User Input To Control Entities
- [X] Editor
  - [X] Content Browser
  - [X] Scene Hierarchy Panel to Create and Manage Entities
  - [X] Use Gizmos to Transform Entity
  - [X] Relationship between Entities
  - [X] Drag and Drop to Set Textures
  - [X] Scene Serialization and Deserialization

## TODO

- [ ] Advanced Lighting
  - [ ] Deferred Shading
  - [ ] Shadows
- [ ] Asset Manager
  - [ ] Refactor Material System
- [ ] Project Serialization and Deserialization
- [ ] An Audio System
- [ ] Import A 3D Physics Engine
- [ ] Use Assimp To Load Models (Use Mesh Class to Handle this)
- [ ] A Better Scripting System
- [ ] A Better Way to Get Sprites from a Sprite Sheet

## Dependencies

- [assimp](https://github.com/assimp/assimp)
- [Box2D](https://box2d.org/)
- [entt](https://github.com/skypjack/entt)
- [freetype](https://www.freetype.org/)
- [GLFW](https://www.glfw.org/)
- [glad](https://github.com/Dav1dde/glad)
- [GLM](https://glm.g-truc.net/0.9.8/index.html)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [ImGui](https://github.com/ocornut/imgui)
- [spdlog](https://github.com/gabime/spdlog)
- [stb_image](https://github.com/nothings/stb/blob/master/stb_image.h)
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)
