/**
 * @file main.cpp
 * @brief Точка входа программы лабораторной работы №4: запуск тестов иерархии.
 * @details Лабораторная работа выполняется поэтапно:
 * - v0.1 — часть 1: SmartDevice → PoweredDevice → LightBulb, учёт энергии;
 * - v0.2 — часть 2: Thermostat, массив устройств через указатели на базовый класс;
 * - v0.3 — часть 3: множественное наследование — ISensor и SmartOutlet;
 * - v1.0 — часть 4: конструкторы копирования и присваивание с учётом состояния.
 *
 * Сами тесты — в отдельном файле tests.cpp, как требует задание.
 * @author Vareshka86
 * @date 2026-10-06
 * @version 1.0
 */

#include "tests.h"

#include <iostream>
#include <string>

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

/**
 * @brief Ждёт нажатия Enter перед закрытием окна.
 * @details Если программу запустили двойным щелчком, без этой паузы окно
 * консоли закроется сразу после вывода.
 */
void waitForEnter()
{
    std::cout << "\nНажмите Enter, чтобы закрыть программу...";
    std::string line;
    std::getline(std::cin, line);
}

} // namespace

/**
 * @brief Главная функция: запускает все тесты.
 * @return 0 — все проверки пройдены, 1 — есть ошибки.
 */
int main()
{
    setupConsole();
    std::cout << "Лабораторная работа №4 по ООП: устройства умного дома (вариант 5), v1.0\n";

    const bool allPassed = runAllTests();

    waitForEnter();
    return allPassed ? 0 : 1;
}
