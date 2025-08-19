@echo off

SET VcvarsallPath="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"

SET BuildType=Debug

SET BuildFolderPath=..\build
SET MainTranslationUnit=..\code\win32_main.cpp
SET MapFileName=win32_main.map

where /q cl
IF %ERRORLEVEL% == 1 (call %VcvarsallPath% x64)

SET DefaultCompilerOpts=/fp:fast /GR- /Oi /nologo /EHa- /D_CRT_SECURE_NO_WARNINGS /GS- /Gs9999999
SET DebugCompilerOpts=/WX /W4 /wd4201 /wd4100 /wd4189 /wd4505 /wd4456
SET DebugCompilerOpts=/MTd /Od /diagnostics:column /WL /FC /Z7 %DebugCompilerOpts% %DefaultCompilerOpts%
SET DebugCompilerOpts=/DAPP_INTERNAL=1 %DebugCompilerOpts%
SET ReleaseCompilerOpts=/MT /w /O2 %DefaultCompilerOpts%
SET ReleaseCompilerOpts=/DAPP_INTERNAL=0 %ReleaseCompilerOpts%
REM /GL (оптимизация всей программы)

SET DefaultLinkerOpts=/incremental:no /opt:ref /SUBSYSTEM:console /NODEFAULTLIB /STACK:0x100000,0x100000
SET DefaultLinkerOpts=/ENTRY:mainCRTStartup %DefaultLinkerOpts%
REM SET DefaultLinkerOpts=/ENTRY:WinMainCRTStartup %DefaultLinkerOpts%
REM /LTCG (оптимизация всей программы) /VERBOSE (печать сообщений о ходе выполнения)

SET DefaultLinkerLibs=kernel32.lib User32.lib

IF NOT EXIST %BuildFolderPath% mkdir %BuildFolderPath%
pushd %BuildFolderPath%

IF /i %BuildType%==Debug (
del *.pdb >NUL 2>NUL
echo WAITING FOR PDB > lock.tmp
del lock.tmp
cl %DebugCompilerOpts% %MainTranslationUnit% /Fm%MapFileName% /link %DefaultLinkerOpts% %DefaultLinkerLibs%
)

IF /i %BuildType%==Release (
cl %ReleaseCompilerOpts% %MainTranslationUnit% /link %DefaultLinkerOpts% %DefaultLinkerLibs%
del *.pdb >NUL 2>&1
del *.map >NUL 2>&1
del *.obj >NUL 2>&1
)

popd