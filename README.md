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

## Random links
don't mind these
### editor stuff 
https://www.youtube.com/watch?v=_2FMt_0wVMM  
https://www.wxwidgets.org  
https://github.com/SonyWWS/ATF/wiki
