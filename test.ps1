
# CONFIG
$includes = @(
    "src\",
    "libs\glad-es\include\"
)

$folders = @(
    "src",
    "libs\glad-es\src"
    # "libs\glfw\src"
)

$embeds = @(
    "res/basic_gles2.frag",
    "res/basic_gles2.vert"
)

$objdir = ".\obj\"


# END CONFIG


# 
# Don't touch anything under here 
# unless you know what you're doing
# 



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
        Write-Host( $cmd )
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

# create link (*.o) flags
$linkfiles = ""
foreach( $folder in $folders )
{
    $linkfiles += compileDir $folder "em++" *.cpp $additionalincludes
    $linkfiles += compileDir $folder "emcc" *.c $additionalincludes
}

# create embed/preload flags
$embedfiles = ""
foreach( $embed in $embeds )
{
    $embedfiles += " --preload-file " + $embed
}

Write-Host( $additionalincludes )

Write-Host( "" )
$linkcmd = "em++ " + $linkfiles + " -o bin/test.html -sUSE_GLFW=3 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1" + $embedfiles
Write-Host( $linkcmd )
Invoke-Expression $linkcmd
pause