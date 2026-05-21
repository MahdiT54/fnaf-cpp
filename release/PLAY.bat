@echo off
cd /d "%~dp0"
set TERM=xterm-256color
set TERMINFO=%~dp0share\terminfo
console_fnaf.exe
echo.
echo Game closed. Press any key to exit.
pause >nul
