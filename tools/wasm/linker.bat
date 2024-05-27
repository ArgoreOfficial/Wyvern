@echo off

set in=%1
set out=%2

set FILES=
FOR /F "tokens=* USEBACKQ" %%F IN ( `dir %in%\*.o /b` ) DO ( call :AddFileToLinker %%F )
echo %FILES%
call emcc %FILES% -o %o%

:AddFileToLinker
set FILES=%1 %FILES%


