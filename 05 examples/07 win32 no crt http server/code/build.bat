@echo off
SET BuildType=debug

SET VcvarsallPath="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
where /q cl
IF %ERRORLEVEL% == 1 (call %VcvarsallPath% x64)

SET LinkerLibs=kernel32.lib User32.lib Ws2_32.lib
SET CompilerOpts=/fp:fast /GR- /Oi /nologo /EHa- /D_CRT_SECURE_NO_WARNINGS /GS- /Gs9999999
SET LinkerOpts=/incremental:no /opt:ref /SUBSYSTEM:console /ENTRY:mainCRTStartup /NODEFAULTLIB /STACK:0x100000,0x100000 

SET BuildFolderPath=..\build
IF NOT EXIST %BuildFolderPath% mkdir %BuildFolderPath%
pushd %BuildFolderPath%

SET DebugCompilerOpts=/WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /wd4456 /Od /diagnostics:column /WL /FC /Z7 %CompilerOpts%
IF /i %BuildType%==Debug (
cl %DebugCompilerOpts% ..\code\win32_server.cpp /Fmwin32_server.map /link %LinkerOpts% %LinkerLibs%
)

SET ReleaseCompilerOpts=/MT /w /O2 %CompilerOpts%
IF /i %BuildType%==Release (
cl %ReleaseCompilerOpts% ..\code\win32_server.cpp /link %LinkerOpts% %LinkerLibs%
del *.pdb >NUL 2>&1
del *.map >NUL 2>&1
del *.obj >NUL 2>&1
)

popd