/**
 * @file format.cpp
 * @brief Реализация функции formatNumber().
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.1
 */

#include "format.h"

#include <iomanip>
#include <sstream>

std::string formatNumber(double value, int digits)
{
    // «Минус ноль» (-0.0) печатается как «-0.000» - заменяем его обычным нулём
    if (value == 0.0)
    {
        value = 0.0;
    }

    std::ostringstream out;
    out << std::fixed << std::setprecision(digits) << value;
    return out.str();
}
