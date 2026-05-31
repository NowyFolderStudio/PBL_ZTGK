@echo off
set /p COMP_NAME="Enter component name (e.g., NewComponent): "

:: %~dp0 checking if the script is run from the correct directory
cmake -DCOMP_NAME=%COMP_NAME% -P "%~dp0make_component.cmake"

echo.
pause