
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
)

$folders = @(
    "source",
    "libs\glad\src"
)

# real/path@virtual/path
$embeds = @(
    "game/res/frag.glsl@res/frag.glsl",
    "game/res/vert.glsl@res/vert.glsl",
    "game/res/psq.wpr@res/psq.wpr"
)

$objdir = "build\wasm\"
$linkflags = "-sUSE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1"

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
    $embedfiles += " --preload-file " + $embed
}

If(!(test-path -PathType container "game")) { New-Item -ItemType Directory -Path "game" }
$linkcmd = "em++ " + $linkfiles + " -o game/" + $projname + ".js " + $linkflags + $embedfiles

Write-Host( "Linking..." )

Invoke-Expression $linkcmd

pause