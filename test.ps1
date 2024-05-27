
# CONFIG
$includes = @(
    "src/",
    "libs/glad/include/"
)

$folders = @(
    "./src",
    "./glad/src",
    "./glfw/src"
)

$objdir = ".\obj\"
# END CONFIG


# 
# Don't touch anything under here 
# unless you know what you're doing
# 

$additionalincludes = ""
$linkfiles = ""

foreach( $include in $includes )
{
    $additionalincludes += " -I" + $include
}

write-output( $additionalincludes )

foreach( $folder in $folders )
{
    Get-ChildItem "./src" -Recurse -Filter *.cpp |
    ForEach-Object {
        $fullpath = $_.FullName
        $outname = $_.Basename + ".o"
        $linkfiles += $objdir + $outname + " "

        $cmd = "em++ " + $fullpath + $additionalincludes + " -c -o " + $objdir + $outname

        write-output( $cmd )
        iex $cmd
    }

    Get-ChildItem "./libs" -Recurse -Filter *.c |
    ForEach-Object {
        $fullpath = $_.FullName
        $outname = $_.Basename + ".o"
        $linkfiles += $objdir + $outname + " "

        $cmd = "emcc " + $fullpath + $additionalincludes + " -c -o " + $objdir + $outname

        write-output( $cmd )
        iex $cmd
    }
}



$linkcmd = "em++ " + $linkfiles + " -o bin/test.html"
write-output( $linkcmd )
pause