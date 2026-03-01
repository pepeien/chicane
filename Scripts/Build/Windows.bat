@echo off

set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%\Windows"

docker compose --verbose up --build