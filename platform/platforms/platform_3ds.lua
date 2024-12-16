includes "../toolchains/3ds_arm-none-eabi.lua" -- 3ds toolchain

rule("3ds.picasso")
    set_extensions( ".pica" )
    
    on_build_file(function (target, sourcefile, opt)
        import("core.project.depend")
        
        -- make sure build directory exists
        os.mkdir(target:targetdir())
        
        local DEVKITARM = "C:/devkitPro/devkitARM" -- os.getenv( "DEVKITARM" )
        local filename = path.basename(sourcefile)
        local shbinFile = "build/" .. filename .. ".shbin"
        local headerFile = "include/" .. filename .. ".h"
        local tmpFile = "build/tmp/" .. filename .. ".shbin.tmp"
        local oFile = shbinFile .. ".o"

        print( sourcefile .. " > " .. headerFile )
        os.vrunv( "C:/devkitPro/tools/bin/picasso", { "-o", shbinFile, sourcefile})
        os.vrunv( "C:/devkitPro/tools/bin/bin2s", {
                "-a 4",
                "-H", headerFile,
                shbinFile
            }, 
            table.join(opt, {stdout = tmpFile})
            )
        os.vrunv( DEVKITARM .."/bin/arm-none-eabi-gcc", {
                "-x", "assembler-with-cpp",
                "-g",
                "-march=armv6k", "-mtune=mpcore", "-mfloat-abi=hard", "-mtp=soft",
                "-c", tmpFile,
                "-o", oFile
            })
        table.insert(target:objectfiles(), oFile)
    end)
rule_end()

rule("3ds.package")
    after_build(function(target)
        local DEVKITARM = "C:/devkitPro/devkitARM" -- os.getenv( "DEVKITARM" )
        os.vrunv(DEVKITARM .."/bin/arm-none-eabi-gcc-nm", { 
            "-CSn", target:targetdir() .. "/3DS.elf"
            }, {stdout = target:targetdir() .. "/3DS.lst"} )

        os.vrunv("C:/devkitPro/tools/bin/3dsxtool", { 
            target:targetdir() .. "/3DS.elf",
            target:targetdir() .. "/3DS.3dsx",
            -- "--smdh=/d/dev/3ds/3ds.smdh"
            })
    end)
rule_end()

function load_platform_3ds()
    --add_requires( "glm" )
    --"glfw", "libsdl", "assimp"
    print("load 3ds")
    set_toolchains( "arm-none-eabi" )
end

function target_platform_3ds( target )
    local root = "../"

    print("target 3ds")
    
    -- add supports
    target:add( "deps", "GLAD" )
    --import(root.."platform.support.glm"   )(target)
    --import(root.."platform.support.glfw"  )(target)
    --import(root.."platform.support.assimp")(target)
    --import(root.."platform.support.libsdl")(target) 

    
end