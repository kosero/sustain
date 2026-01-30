# Sustain Engine

A 2D game engine I'm building with [SDL3](https://www.libsdl.org/) and C for my personal game projects. I'm using [LuaJIT](https://luajit.org/) for scripting because, well, who wants to recompile everything just to tweak a value?

## What's cooking

Here's what I'm planning to add (when I get around to it):

- [ ] Box2D integration for physics (because making my own physics engine sounds like a nightmare)
- [ ] Animated sprites and tilemap support
- [ ] Particle system (for all those explosion effects)
- [ ] UI system for menus and HUD
- [ ] Sound system
- [ ] Networking support (maybe... eventually... if I'm feeling ambitious)

## Building this thing

Clone the repo:

```sh
git clone https://github.com/kosero/sustain.git
cd sustain
```

Build it:

```sh
mkdir build && cd build
CC=gcc CXX=g++ cmake ..
make
```

---

**Note:** This is a solo project, so don't expect AAA-level polish. I'm just building what I need for my games.
