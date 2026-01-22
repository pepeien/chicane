@echo off

set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%\.."

doxygen -q Doxyfile
python Scripts/Documentation.py ^
-i .docs/xml ^
-o .docs/references ^
-d Includes/Chicane/ ^
-n Chicane ^
-s CHICANE_BOX,CHICANE_CORE,CHICANE_GRID,CHICANE_KERB,CHICANE_RENDERER,CHICANE_RUNTIME,CHICANE_SCREECH