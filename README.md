# The Wyvern Game Engine
<sup>Copyright (C) 2023-2024 Argore</sup>

## Building
Wyvern uses the free and open-source build system [xmake](https://github.com/xmake-io/xmake).  

### Visual Studio Setup
1. Follow the [xmake installation guide](https://xmake.io/#/guide/installation)  
2. Run `build.bat`  
3. The Visual Studio solution will be located in build/vsxmake...

### Linking ASSIMP 
A static build of [Assimp]() is provided

1. extract `assimp-static.7z` somewhere
2. change ASSIMP_STATIC_ROOT in `xmake.lua` to point to that folder (default `D:/SDK/assimp-static/`)

this dependency will be removed sometime in the future, but for now this works

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
xmake f -p wasm 
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
