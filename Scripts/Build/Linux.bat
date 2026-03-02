@echo off

set SCRIPT_DIR=%~dp0
cd /d "%SCRIPT_DIR%\Linux"

docker compose --verbose up --build