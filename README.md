# LevelEditor

A level editor designed to be integrated with ~~Unity~~ Godot (or any other engine), such that you can export files from the level editor to the engine and avoid having to set up lots of finickey in-engine editor tools. The premise is similar to the Rain World level editor - easy controls but with an extremely high quality graphical output.

---

### Feature list (so far)
#### Geometry:
- Easy and quick geometry editing, with tiles being able to slant at 45 degrees.
- Saving the level and loading it again to a simple file format.
- Exporting the geometry as a collection of continous edges.

#### Materials and Rendering
- Allows for editing the materials of geometry
- (Rendering WIP)

#### Game Logic
- Allows for placing basic 'entities' that can be given names and have different functionalities, e.g. cameras.
- Allows for named 'trigger areas' that define a rectangular area

---

### Installation Instructions

So long as you have [vcpkg](https://vcpkg.io/) installed and integrated with Visual Studio, you should be able to easily clone this repository and open it in Visual Studio. vcpkg should handle installing any packages you need for you.
In case not, the used packages are:
- ImGui
- stb libraries
- GLES and GLFW for openGL rendering
