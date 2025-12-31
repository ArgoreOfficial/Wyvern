
<p align="center">
  <img width="256" height="256" src="resources/icon.png">
</p>
<p align="center"><sup>Logo by Kurenai_Chi </sup></p>
<h1 align="center">Wyvern</h1>

<sup>Copyright © 2023-2025 Argore</sup>

Wyvern is my hobby C++ game engine. Currently going through a rewrite, it will serve the basis for my bird flying game and future projects.

## Building
Wyvern uses the free and open-source build system [xmake](https://github.com/xmake-io/xmake).  
[xmake installation guide](https://xmake.io/#/guide/installation)  

### Visual Studio Solution
Run the `generate-project-files.ps1` script to generate a Visual Studio 2026 solution.  
The default name will be `_Game.sln`, which can be configured at the top of the `xmake.lua` file.

### Command Line
```
xmake f -p <platform> -m <mode>
```
`<platform>` currently only windows  
`<mode>` Debug | Release | Package  

```
xmake
```
The executable will be created inside `game/`, alongside dlls and the game data folder

## Usage
> TODO
