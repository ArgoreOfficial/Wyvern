$platforms = "x64"

$enable_3ds  = $false
$enable_psp2 = $false

if( $enable_psp2 -and (Test-Path xmake/psvita/load.lua ) -and (Test-Path env:SCE_PSP2_SDK_DIR) ) {
    $platforms += ",psvita"
}

if( $enable_3ds -and (Test-Path xmake/arm_3ds/load.lua ) -and (Test-Path env:DEVKITARM) ) {
    $platforms += ",arm_3ds"
}

& "xmake" f -y --vs=2022
& "xmake" project -k vsxmake -y -m "Debug,Debug-nomt,Release,Package" -a $platforms ./build

if( $LASTEXITCODE -lt 0 )
{
    write-host "Press any key to continue..."
    [void][System.Console]::ReadKey($true)
}
else {
    Get-ChildItem "$(Get-Item .)\build\vsxmake2022" -Filter *.sln |
    ForEach-Object {
        write-host ($_.BaseName)
        $WshShell = New-Object -COMObject WScript.Shell
        $Shortcut = $WshShell.CreateShortcut("$(Get-Item .)\$($_.BaseName).sln.lnk")
        $Shortcut.TargetPath = "$(Get-Item .)\build\vsxmake2022\$($_.BaseName).sln"
        $Shortcut.Save()
    }
}
