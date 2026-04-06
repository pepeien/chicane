@echo off

set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%\.."

find . ^
-type d \( -name Vendor \) ^
-prune -o ^
-type f \( -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' \) ^
-exec clang-format ^
-i {} +