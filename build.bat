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
git clone --bare http://gitlab.psq.local/piostb/02-shadowmaps.git .git
git init
git pull
git reset HEAD
git submodule init
git submodule update --recursive
git checkout master
git lfs install
git lfs pull

:submodule_update
echo Updating submodules
if not exist libs\glfw\.git git submodule add https://github.com/glfw/glfw.git libs/glfw
if not exist libs\glm\.git git submodule add https://github.com/g-truc/glm.git libs/glm
git submodule init
git submodule update --recursive

:premake_create 
echo Creating project
cd tools
cd premake
premake5.exe vs2022