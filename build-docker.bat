@echo off
REM ============================================================================
REM KontolOS Docker Build Script
REM ============================================================================

echo.
echo ========================================
echo  KontolOS Docker Build
echo ========================================
echo.

REM Check for Docker
where docker >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] Docker not found. Please install Docker Desktop.
    pause
    exit /b 1
)

REM Check if Docker is running
docker info >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [!] Docker is not running. Please start Docker Desktop.
    pause
    exit /b 1
)

echo [*] Building Docker image (first time may take a few minutes)...
docker build -t kontolos-builder .
if %ERRORLEVEL% NEQ 0 (
    echo [!] Failed to build Docker image
    pause
    exit /b 1
)

echo.
echo [*] Compiling KontolOS...
docker run --rm -v "%cd%":/src -e DOCKER_BUILD=1 kontolos-builder make clean docker-build
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [!] Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo  Build Complete!
echo ========================================
echo.
echo Output: build\kontolos.img
echo.
echo To run the OS:
echo   qemu-system-i386 -fda build\kontolos.img
echo.
echo Or run: run.bat
echo.
pause
