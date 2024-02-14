@echo off

if exist .git\ (
  goto :submodule_update
)

where /q git
if errorlevel 1 (
    echo Couldn't find git. Make sure it is installed and placed in PATH.
    explorer https://git-scm.com/downloads
    pause
    exit /B
)

:git_init
echo Updating git
git clone --bare https://github.com/ArgoreOfficial/Wyvern.git .git
git init
git pull
git reset HEAD
git submodule init
git submodule update --recursive
git checkout master

:submodule_update
echo Updating submodule
git submodule init
git submodule update

:premake_create 
echo Creating project
cd tools
cd premake
premake5.exe vs2022