/**
 * @file format.h
 * @brief Вывод дробных чисел с заданным числом знаков после точки.
 * @details Нужен в описаниях устройств: мощность печатается с одним знаком
 * («48.0 Вт»), энергия — с тремя («0.240 кВт·ч»).
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.1
 */

#ifndef FORMAT_H
#define FORMAT_H

#include <string>

/**
 * @brief Переводит число в строку с фиксированным числом знаков после точки.
 * @param value  Число.
 * @param digits Сколько знаков после точки оставить.
 * @return Строка, например formatNumber(0.24, 3) → «0.240».
 */
std::string formatNumber(double value, int digits);

#endif // FORMAT_H
