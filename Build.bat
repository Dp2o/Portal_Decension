@echo off
set PATH=C:\Users\dylan\Onedrive\Documents\w64devkit\bin;%PATH%

echo Compiling...
:: Check if Main.cpp is actually in a folder called 'src'
if exist src\Main.cpp (
    g++ src/Main.cpp -o Portal_Decension.exe -mwindows -O3 -lmsimg32 -lgdiplus
) else (
    g++ Main.cpp -o Portal_Decension.exe -mwindows -O3 -lmsimg32 -lgdiplus
)

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo BUILD FAILED! Check the errors above.
    pause
    exit /b
)

echo Build successful. Starting app...
start Portal_Decension.exe
exit