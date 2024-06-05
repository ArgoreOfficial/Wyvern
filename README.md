# ASSIMP 
I don't like DLLs so ASSIMP has been statically linked, currently hardcoded in the Premake build script  
to get working, extract the `assimp-static.7z` somewhere  
then change the ASSIMP_STATIC_ROOT variable in `tools/premake/premake5.lua` to point to that folder

this dependency will be removed sometime in the future, replaced with a custom model parser

## Random links
### editor stuff 
https://www.youtube.com/watch?v=_2FMt_0wVMM  
https://www.wxwidgets.org  
https://github.com/SonyWWS/ATF/wiki
