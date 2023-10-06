@echo off

set /p "class=Class: "
set /p "namespace=Namespace: "
set "indent=%namespace%"
if not "%namespace%" equ "" set "indent=	"

(
    echo #pragma once
    echo.
    if not "%namespace%" equ "" echo namespace %namespace%
    if not "%namespace%" equ "" echo {
    echo %indent%class %class%
    echo %indent%{
    echo %indent%public:
    echo %indent%	%class%(^)^;
    echo %indent%	~%class%(^)^;
    echo %indent%}^;
    if not "%namespace%" equ "" echo }
)> "%class%.h"

(
    echo #include "%class%.h"
    if not "%namespace%" equ "" echo using namespace %namespace%;
    echo.
    echo %class%::%class%(^)
    echo {
    echo.
    echo }
    echo.
    echo %class%::~%class%(^)
    echo {
    echo.
    echo }
)> "%class%.cpp"