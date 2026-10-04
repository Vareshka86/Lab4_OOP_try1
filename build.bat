@echo off
chcp 65001 > nul
rem Сборка лабораторной работы №4 компилятором g++ (MinGW-w64).
rem Результат: build\lab4.exe
rem Компилируются все файлы src\*.cpp - g++ сам раскрывает шаблон *.cpp,
rem поэтому при добавлении нового задания этот файл менять не нужно.
rem Стандарт языка - C++14 (требование преподавателя): флаг -std=c++14.
rem Флаг -static встраивает библиотеки MinGW (libstdc++ и др.) прямо в exe,
rem поэтому программа запускается и на компьютере, где MinGW не установлен.

if not exist build mkdir build

g++ -std=c++14 -Wall -Wextra -pedantic -static -Iinclude src\*.cpp -o build\lab4.exe

if errorlevel 1 (
    echo Сборка завершилась с ошибкой.
    exit /b 1
)
echo Сборка успешна: build\lab4.exe
