--[[

    Copyright (C) 2023-2024 Argore 

]]--

PROJECT_NAME = "Sandbox"

set_project( PROJECT_NAME )
set_version( "0.0.1" )

if not is_mode( "Debug" ) then
    set_runtimes( "MT" )
else
    set_runtimes( "MTd" )
end

set_allowedarchs( "x64", "x86", "x86_64", "wasm32", "psvita", "arm_3ds" )

includes( "xmake/platforms.lua" )
init_platform()

includes( "source/Engine" )
includes( "source/App" )

-- your .lnk file
local your_shortcut_name = "your_shortcut.lnk"      

-- target (file or folder) with full path
local your_target_filespec = [["D:\Dev\wyvern\build\vsxmake2022\Sandbox.sln"]]

local ps = io.popen("powershell -command -", "w")
ps:write("$ws = New-Object -ComObject WScript.Shell;$s = $ws.CreateShortcut('"..your_shortcut_name.."');$s.TargetPath = '"..your_target_filespec.."';$s.Save()")
ps:close()