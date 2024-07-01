@echo off
xmake f --vs=2022
xmake project -k vsxmake -y -m "Debug, Debug-WinXP,Release,Package" -a "x64,x86" ./build
pause