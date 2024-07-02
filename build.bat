@echo off
xmake f --vs=2022
xmake project -k vsxmake -y -m "Debug,Release,Package" -a "x64,x86" ./build