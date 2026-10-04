/**
 * @file main.cpp
 * @brief Точка входа программы лабораторной работы №4.
 * @details Лабораторная работа выполняется поэтапно, и в каждой версии
 * программы добавляется новая часть иерархии устройств умного дома.
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.0
 */

#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // подключать только основную часть WinAPI
#include <windows.h>        // SetConsoleOutputCP, SetConsoleCP
#endif

/// Вспомогательные функции, видимые только внутри этого файла.
namespace
{

/**
 * @brief Настраивает консоль Windows на кодировку UTF-8.
 * @details Исходные файлы сохранены в UTF-8. Без этой настройки
 * русский текст в консоли Windows выводится «кракозябрами».
 * В Linux/macOS консоль уже работает в UTF-8, поэтому там функция
 * ничего не делает (код внутри `#ifdef _WIN32` не компилируется).
 */
void setupConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

} // namespace

/**
 * @brief Главная функция программы.
 * @return 0 — при нормальном завершении.
 */
int main()
{
    setupConsole();
    std::cout << "Лабораторная работа №4 по ООП: устройства умного дома.\n"
              << "Иерархия классов будет добавлена в версии v0.1.\n";
    return 0;
}
