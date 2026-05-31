@echo off
REM Build script for Calendar System backend
REM Usage: build.bat [cli|server|all]

set GXX=g++
set CXXFLAGS=-std=c++17 -g
set INCLUDE=-Iinclude
set SRCDIR=src
set BINDIR=bin

if not exist %BINDIR% mkdir %BINDIR%

if /I "%1"=="server" goto build_server
if /I "%1"=="cli" goto build_cli
if /I "%1"=="all" goto build_all
if /I "%1"=="" goto build_all

:build_cli
echo Building CLI...
%GXX% %CXXFLAGS% %INCLUDE% ^
    %SRCDIR%\main.cpp ^
    %SRCDIR%\event.cpp ^
    %SRCDIR%\calendar.cpp ^
    %SRCDIR%\file_manager.cpp ^
    %SRCDIR%\ds_utility.cpp ^
    %SRCDIR%\linked_list.cpp ^
    %SRCDIR%\bst.cpp ^
    %SRCDIR%\min_heap.cpp ^
    %SRCDIR%\hash_table.cpp ^
    -o %BINDIR%\calendar_cli.exe
if %ERRORLEVEL%==0 (echo CLI built: %BINDIR%\calendar_cli.exe) else (echo CLI build failed)
if not "%1"=="all" exit /b %ERRORLEVEL%

:build_server
echo Building Server...
%GXX% %CXXFLAGS% %INCLUDE% ^
    %SRCDIR%\server.cpp ^
    %SRCDIR%\event.cpp ^
    %SRCDIR%\calendar.cpp ^
    %SRCDIR%\file_manager.cpp ^
    %SRCDIR%\ds_utility.cpp ^
    %SRCDIR%\linked_list.cpp ^
    %SRCDIR%\bst.cpp ^
    %SRCDIR%\min_heap.cpp ^
    %SRCDIR%\hash_table.cpp ^
    -o %BINDIR%\calendar_server.exe ^
    -lws2_32
if %ERRORLEVEL%==0 (echo Server built: %BINDIR%\calendar_server.exe) else (echo Server build failed)
if not "%1"=="all" exit /b %ERRORLEVEL%

:build_all
call :build_cli
call :build_server
echo All builds complete.
exit /b 0
