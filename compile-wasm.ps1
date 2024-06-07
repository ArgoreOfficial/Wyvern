
#########################
#                       #
#  Build Configuration  #
#                       #
#########################

$projname = "Wyvern"

$includes = @(
    "source\Engine",
    "libs\glad\include\",
    "libs\glm\"
    # "libs\imgui"
)

$folders = @(
    "source",
    "libs\glad\src"
    # "libs\imgui"
)


$embeds = @(
    'res/shaders/deferred.wshader',
    'res/shaders/sky.wshader',
    'res/shaders/phong.wshader',

    'res/shaders/source/deferred_vs.glsl',
    'res/shaders/source/deferred_fs.glsl',
    'res/shaders/source/sky_vs.glsl',
    'res/shaders/source/sky_fs.glsl',
    'res/shaders/source/phong_vs.glsl',
    'res/shaders/source/phong_fs.glsl',

    'res/meshes/monke.glb.wpr',
    'res/meshes/skysphere.glb.wpr',
    
    'res/materials/defaultSky.wmat',
    'res/materials/phong.wmat',
    
    'res/textures/autumn_field_puresky.png',
    'res/textures/uv.png'
)

$objdir = "build\wasm\"
$linkflags = "-sUSE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1 -sALLOW_MEMORY_GROWTH"

# END CONFIG

########################################
#                                      #
#   Don't touch anything under here    #
#   unless you know what you're doing  #
#                                      #
########################################


function compileDir($f, $emc, $ext, $incl) 
{
    $links = ""

    Get-ChildItem $f -Recurse -Filter $ext |
    ForEach-Object {
        $fullpath = $_.FullName
        $outname = $_.Basename + ".o"
        $fobjdir = $objdir + $f + "\"
        
        If(!(test-path -PathType container $fobjdir))
        {
            New-Item -ItemType Directory -Path $fobjdir
        }
        
        
        $cmd = $emc + " " + $fullpath + $incl + " -c -o " + $fobjdir + $outname
        $objfile = $fobjdir + $outname + " "
        $links += $objfile
        Write-Host( $outname )
        Invoke-Expression $cmd
    }

    return $links
}

# create additional includes -I flags
$additionalincludes = ""
foreach( $include in $includes )
{
    $additionalincludes += " -I" + $include
}

Write-Host( "Compiling..." )

# create link (*.o) flags
$linkfiles = ""
foreach( $folder in $folders )
{
    $linkcpp = compileDir $folder "em++" *.cpp $additionalincludes
    $linkc   = compileDir $folder "emcc" *.c $additionalincludes

    $linkfiles += $linkcpp + $linkc
}

# create embed/preload flags
$embedfiles = ""
foreach( $embed in $embeds )
{
    $embedfiles += " --preload-file " + "game/" + $embed + "@" + $embed
}

If(!(test-path -PathType container "game")) { New-Item -ItemType Directory -Path "game" }
$linkcmd = "em++ " + $linkfiles + " -o game/" + $projname + ".js " + $linkflags + $embedfiles

Write-Host( "Linking..." )

Invoke-Expression $linkcmd

pause