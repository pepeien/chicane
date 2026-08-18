@echo off
setlocal EnableExtensions

set "SCRIPT_DIR=%~dp0"
cd /d "%SCRIPT_DIR%\.."

where clang-format >nul 2>&1
if errorlevel 1 (
    echo clang-format was not found in PATH.
    exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -Command "Get-ChildItem -Path . -Recurse -File | Where-Object { $_.Extension -match '^\.(c|cpp|h|hpp)$' -and $_.FullName -notmatch '[\\/]Vendor[\\/]' } | ForEach-Object { & clang-format -i -- $_.FullName }"

if errorlevel 1 (
    echo Format failed.
    exit /b 1
)

echo Format complete.
exit /b 0
