
<p align="center">
  <img width="256" height="256" src="resources/icon.png">
</p>
<p align="center"><sup>Logo by Kurenai_Chi </sup></p>
<h1 align="center">Wyvern</h1>

<sup>Copyright © 2023-2025 Argore</sup>

| Platform         | Support  | Comment |
| :--------------: | :------: | :-----: |
| **Windows**      | Full     |         |
| **Emscripten**   | Outdated |         |
| **Linux**        | No       |         |
| **macOS**        | No       |         |
| **PSVita**       | Partial  | Only licensed <br> developers |
| **Consoles***    | No       |         |

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
* Proper Editor (Likely using wxWidgets or Qt), and through that:
  * and through that, SceneGraphs, Prefabs, runtime Materials, and so on.
* More versatile materials  

## Building
Wyvern uses the free and open-source build tool [xmake](https://github.com/xmake-io/xmake).  
[xmake installation guide](https://xmake.io/#/guide/installation)  

### Visual Studio Solution
Run the `generate-project-files.ps1` script. It will generate a Visual Studio 2022 solution inside `build/vsxmake2022/`

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

## Random links
don't mind these
### editor stuff 
https://www.youtube.com/watch?v=_2FMt_0wVMM  
https://www.wxwidgets.org  
https://github.com/SonyWWS/ATF/wiki
