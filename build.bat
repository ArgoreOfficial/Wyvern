@echo off
xmake f --vs=2022
xmake project -k vsxmake -v -y -m "Debug,Release,Package" ./build