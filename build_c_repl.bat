@echo off
echo Compiling C Language Interactive Terminal...

gcc -o c_repl_advanced.exe c_repl_advanced.c

if %errorlevel% equ 0 (
    echo Compilation successful!
    echo Run: c_repl_advanced.exe
    pause
    c_repl_advanced.exe
) else (
    echo Compilation failed! Please check if GCC compiler is installed
    pause
)
