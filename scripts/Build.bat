@echo off
set "CFLAGS=/nologo /Zi /c /FC /I C:\Home\System\Resources\Include"
set "LFLAGS=/NOLOGO /DEBUG:FULL /INCREMENTAL:NO /OUT:"debug.exe" /LIBPATH:"C:\Home\System\Resources\Libraries""

del /q .\bin\*
del /q .\build\*

pushd .\build
cl %CFLAGS% ..\code\main.cpp
popd

pushd .\bin
link %LFLAGS% ..\build\main.obj user32.lib gdi32.lib kernel32.lib \GLFW\glfw3.lib
popd
