& "xmake" f -y --vs=2022
& "xmake" project -k vsxmake -y -m "debug,release,package" -a "x64" ./build

if( $LASTEXITCODE -lt 0 )
{
    write-host "Press any key to continue..."
    [void][System.Console]::ReadKey($true)
}
else {
    Get-ChildItem "$(Get-Item .)\build\vsxmake2022" -Filter *.sln |
    ForEach-Object {
        write-host ("Created shortcut to $($_.BaseName).sln")
        $WshShell = New-Object -COMObject WScript.Shell
        $Shortcut = $WshShell.CreateShortcut("$(Get-Item .)\_$($_.BaseName).sln.lnk")
        $Shortcut.TargetPath = "$(Get-Item .)\build\vsxmake2022\$($_.BaseName).sln"
        $Shortcut.Save()
    }
}
