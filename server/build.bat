@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"

cd /d "%~dp0"

if not exist build mkdir build

cl /EHsc /std:c++17 /MT ^
main.cpp src\mongoose.c lib\sqlite\sqlite3.c ^
/Fo:build\ ^
/Fe:app.exe ^
ws2_32.lib user32.lib shell32.lib advapi32.lib