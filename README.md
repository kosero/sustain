# What is Sustain Engine?

A 2D game engine I wrote using [SDL](https://www.libsdl.org/) for my game projects.
I used [LuaJIT](https://luajit.org/) to write the script.

## Todo

Here are some planned features for Sustain Engine:

- [ ] Integrate Box2D for 2D physics
- [ ] Animated sprite and tilemap support
- [ ] Particle system
- [ ] UI system for game menus and HUD
- [ ] Networking support (planned for later)
- [ ] Sound system

## Build

- Clone and changed the repository:

```sh
git clone https://github.com/kosero/sustain.git
cd sustain
```

- Build the engine:

```sh
mkdir build && cd build
cmake .. && make
```
