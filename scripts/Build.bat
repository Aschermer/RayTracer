@echo off
set "CFLAGS=/nologo /Zi /c /FC /EHsc /I C:\Home\System\Resources\Include /I C:\VulkanSDK\1.4.321.1\Include"
set "LFLAGS=/NOLOGO /DEBUG:FULL /INCREMENTAL:NO /OUT:"debug.exe" /LIBPATH:"C:\Home\System\Resources\Libraries" /LIBPATH:"C:\VulkanSDK\1.4.321.1\Lib""

del /q .\bin\*
del /q .\build\*

pushd .\build
cl %CFLAGS% ..\code\main.cpp
popd

pushd .\bin
link %LFLAGS% ..\build\main.obj user32.lib gdi32.lib kernel32.lib shell32.lib \GLFW\glfw3_mt.lib vulkan-1.lib
popd
