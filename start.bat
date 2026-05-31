@echo off
REM ============================================
REM  Calendar System - Fullstack Launcher
REM ============================================
echo.
echo   Starting Calendar System...
echo   Backend:  http://localhost:8080
echo   Frontend: http://localhost:5173
echo.

REM Start the backend server in a new window
start "Calendar Backend" cmd /c "cd /d %~dp0backend && bin\calendar_server.exe"

REM Wait a moment for the server to start
timeout /t 2 /nobreak > nul

REM Start the frontend dev server in a new window
start "Calendar Frontend" cmd /c "cd /d %~dp0frontend && npm run dev"

echo.
echo   Both servers are starting up!
echo   Frontend will open at http://localhost:5173
echo.
pause
