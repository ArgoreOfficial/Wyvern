@echo off

set i=%1
set tmp=%2
set o=%3
set FILES=

for /F "tokens=* USEBACKQ" %%F IN ( `dir %i%\*.cpp /b /s` ) do ( 
    call :BuildFile %%F %tmp%
)
goto Link

:BuildFile
echo %~dp2%~n1.o
set FILES=%~dp2%~n1.o %FILES%
call emcc %1 -c -I src -I libs\glad\include -o %~dp2\%~n1.o
goto :eof

:Link
echo Linking %FILES%
call emcc %FILES% -o %o%