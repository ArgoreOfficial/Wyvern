@echo off
xmake f --vs=2022 -c
xmake project -k vsxmake -y -m "Debug,Release,Package" -a "x64,psvita" ./build