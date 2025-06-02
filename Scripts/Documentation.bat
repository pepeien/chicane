@echo off

set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%\.."

doxygen -q Doxyfile
python Scripts/Documentation.py