# SDLOpenGLBase
A graphical application framework built using SDL2 and the OpenGL graphics API. This is very far from completion, meaning most of the code is temporary, but I am working on it actively. Despite that, I will probably only push major changes to Github, so updates will likely be rare.

Features that exist currently:

* Custom mathematics library supporting 2, 3 and 4 dimensional vectors/matrices and quaternions.
* Custom memory management system based off low-level system calls with support for several different allocators.
* Rigid body physics engine that uses sequential impulse to solve constraints and a dynamic AABB tree for spacial partioning into islands.
* Skeletal animation system supporting non-uniform scaling, frame/animation interpolation and capable of layering multiple animations simultaneously.
* Dynamic and programmable Source-like particle system.
* Custom font format based off OpenType, designed for fast and efficient rendering. Supports BMP and MSDF (multi-channel signed distance field) images.
* GUI system supporting basic panels and text rendering.
* Quake/Source-like console and command system.
* Quake/Source-like input manager allowing strings of console commands to be bound to keyboard and mouse buttons.

Features that need to be refined:

* There are still some joints, constraints and collider types left to implement in the physics engine, and a lot of it needs more rigorous testing.
* The particle system is still missing a few features, such as implementations for more types of initializers, operators and constraints.
* The current GUI system is extremely basic, there's a lot of missing functionality and a lot of the code is temporary.
* Textures support mipmapping, but it hasn't been tested. Smooth scrolling and programmable texture animations still need to be implemented.
* File loading sucks, binary file formats would be nice for pretty much everything.
* Mathematics functions are used fairly naively at the moment.

Features that haven't been started:

* Audio system.
* Scene management system with support for multiple cameras and independently renderable regions.
* Decals.
* SIMD for mathematics functions.
* Thread pool system and multithreading support for memory managers.
* Featherstone method for things like inverse kinematics.
* Maybe remove dependence on SDL2. This would make cross-platform support a pain, though.
* LODs for models.

Note: In a future commit I'll probably include some of my sample resources so you can actually see what's happening. At the moment, feel free to download the binary if you want to be treated to a black screen.