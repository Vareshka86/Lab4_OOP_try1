/**
 * @file LightBulb.cpp
 * @brief Реализация класса LightBulb.
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.1
 */

#include "LightBulb.h"
#include "format.h"

#include <stdexcept>

/// Вспомогательные функции, видимые только внутри этого файла.
namespace
{

/**
 * @brief Проверяет яркость и при ошибке бросает исключение.
 * @param brightness Яркость в процентах.
 * @exception std::invalid_argument Если яркость вне диапазона.
 */
void checkBrightness(int brightness)
{
    if (brightness < LightBulb::MIN_BRIGHTNESS || brightness > LightBulb::MAX_BRIGHTNESS)
    {
        throw std::invalid_argument("яркость лампы должна быть от " +
                                    std::to_string(LightBulb::MIN_BRIGHTNESS) + " до " +
                                    std::to_string(LightBulb::MAX_BRIGHTNESS) + " %");
    }
}

/**
 * @brief Проверяет цвет и при ошибке бросает исключение.
 * @param color Цвет свечения.
 * @exception std::invalid_argument Если цвет пустой.
 */
void checkColor(const std::string& color)
{
    if (color.empty())
    {
        throw std::invalid_argument("цвет лампы не может быть пустым");
    }
}

} // namespace

LightBulb::LightBulb(const std::string& name, double power, int brightness, const std::string& color)
    : PoweredDevice(name, power), brightness_(brightness), color_(color)
{
    checkBrightness(brightness_);
    checkColor(color_);
}

LightBulb::LightBulb(const LightBulb& other)
    : PoweredDevice(other), brightness_(other.brightness_), color_(other.color_)
{
}

LightBulb& LightBulb::operator=(const LightBulb& other)
{
    if (this != &other)
    {
        PoweredDevice::operator=(other); // сначала: засчитывает прошлое по старой яркости
        brightness_ = other.brightness_;
        color_ = other.color_;
    }
    return *this;
}

LightBulb::~LightBulb()
{
    if (isOn())
    {
        turnOff(); // засчитать энергию за последний интервал работы
    }
}

double LightBulb::getPowerUsage() const
{
    if (!isOn())
    {
        return 0.0;
    }
    return getPowerConsumption() * brightness_ / 100.0;
}

std::string LightBulb::getStatus() const
{
    return "Лампа «" + getName() + "»: " + (isOn() ? "включена" : "выключена") +
           ", яркость " + std::to_string(brightness_) + " %, цвет «" + color_ +
           "», мощность сейчас " + formatNumber(getPowerUsage(), 1) + " Вт из " +
           formatNumber(getPowerConsumption(), 1) + " Вт, засчитано " +
           formatNumber(getEnergyConsumed(), 3) + " кВт·ч";
}

int LightBulb::getBrightness() const
{
    return brightness_;
}

const std::string& LightBulb::getColor() const
{
    return color_;
}

void LightBulb::setBrightness(int brightness)
{
    checkBrightness(brightness); // сначала проверка: при ошибке лампа не меняется
    updateMeter();               // энергия до изменения - по старой яркости
    brightness_ = brightness;
}

void LightBulb::setColor(const std::string& color)
{
    checkColor(color);
    color_ = color; // цвет на мощность не влияет - updateMeter() не нужен
}
