::@ECHO OFF
COLOR 0A
SET BatchName=%~n0
SET BatchDir=%cd%\
PowerShell -NoProfile -ExecutionPolicy Bypass -Command "& '%BatchDir%script.ps1'";
Pause

