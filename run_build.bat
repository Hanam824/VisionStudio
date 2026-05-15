@echo off
REM ═══════════════════════════════════════════════════════════════════════════════
REM  Vision Studio — Windows Build Wrapper
REM  Launches build.ps1 via PowerShell.
REM ═══════════════════════════════════════════════════════════════════════════════

echo === Vision Studio Build ===
echo.

REM Try pwsh (PowerShell 7+) first, fall back to powershell (5.1)
where pwsh >nul 2>&1
if %errorlevel% equ 0 (
    pwsh -ExecutionPolicy Bypass -File "%~dp0build.ps1"
) else (
    powershell -ExecutionPolicy Bypass -File "%~dp0build.ps1"
)

if %errorlevel% neq 0 (
    echo.
    echo BUILD FAILED — see errors above.
    pause
    exit /b 1
)
