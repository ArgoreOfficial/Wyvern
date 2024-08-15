# The Wyvern Game Engine
<sup>Copyright © 2023-2024 Argore</sup>

|                | SDL2 | GLFW | OpenGL | GLES |
| :-----------:  | :-: | :-: | :-: | :-: |
| **Windows**    | ✔️ | ✔️ | ✔️ | ✔️ |
| **Emscripten** | ✔️ | ❌ | ❌ | ✔️ |
| **Linux**      | ❔ | ❔ | ❔ | ❔ |
| **macOS**      | ❔ | ❔ | ❔ | ❔ |
| **Consoles***  | ❌ | ❌ | ❌ | ❌ |

<sup>*work in progress</sup>

## Features
todo :(
## Planned Features
> The code is more what you'd call 'guidelines' than actual rules.

~ Captain Barbossa, ca.1730.  
aka these are things that I _want_ to implement, not a timeline

---
* Direct3D and Vulkan Backend
* (Homebrew) Console support, such as Xbox/Xbox 360, PlayStation®3, etc.
  * a large portion of the configurable pipeline in Wyvern is to allow for easy implementation of more involved platforms, although this is not a priority
* Proper Editor (Likely using wxWidgets or Qt), and through that:
  * and through that, SceneGraphs, Prefabs, runtime Materials, and so on.
* Built-in Physics engine, likely Bullet, PhysX, or Havok. Potentially a custom one but unlikely 
* More versatile materials and graphics
  * Currently only basic shading exists and Material properties have to be hardcoded in C++ 

## Building
Wyvern uses the free and open-source build tool [xmake](https://github.com/xmake-io/xmake).  
[xmake installation guide](https://xmake.io/#/guide/installation)  

### Visual Studio Solution
Run `build.bat`  
The Visual Studio solution will be located in build/vsxmake...

### Command Line
```
xmake f -p <platform> -m <mode>
```
`<platform>` is windows, linux, or wasm  
`<mode>` is Debug, Release, or Package

```
xmake
```

## Building WASM
Wyvern supports WASM through SDL and OpenGL ES. Building is done using xmake like normal, but requires a few extra steps.  
Emscripten installation guide can be found [here]()

### msys2 & mingw64
### Installing Dependencies
1. **In the msys2 terminal, make sure mingw64 is properly installed**
```
pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
```

<br>

2. **Start mingw64 terminal as administrator**
```
pacman -S mingw-w64-x86_64-xmake
```

<br>

```
emsdk activate
```

<br>

```
xmake l detect.sdks.find_emsdk
```
the path to EMSDK should be printed

<br>

```
where git
```
<details>
<summary>INFO: Could not find files for the given pattern(s).</summary>

```
export PATH=$PATH:<path/to/Git/cmd>  
```
where `<path/to/Git/cmd>` is the path to your Git installation
</details>

<br>  
<br>

```
xrepo install -v -p wasm libsdl
```
```
xrepo install -v -p wasm assimp
```

<br>

### Building
in Wyvern root start cmd, then activate emsdk and build xmake like normal

```
emsdk activate
```
```
xmake f -p wasm -m Package
```
```
xmake --rebuild
```
<br>

there should now be an html file in `game/`

```
emrun game/Wyvern.html
```

## Random links
don't mind these
### editor stuff 
https://www.youtube.com/watch?v=_2FMt_0wVMM  
https://www.wxwidgets.org  
https://github.com/SonyWWS/ATF/wiki
