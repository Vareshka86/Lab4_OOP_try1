/**
 * @file Thermostat.cpp
 * @brief Реализация класса Thermostat.
 * @author Vareshka86
 * @date 2026-10-06
 * @version 0.2
 */

#include "Thermostat.h"
#include "format.h"

#include <stdexcept>

/// Вспомогательные функции, видимые только внутри этого файла.
namespace
{

/**
 * @brief Проверяет температуру и при ошибке бросает исключение.
 * @param temperature Температура в градусах.
 * @exception std::invalid_argument Если температура вне диапазона.
 */
void checkTemperature(int temperature)
{
    if (temperature < Thermostat::MIN_TEMPERATURE || temperature > Thermostat::MAX_TEMPERATURE)
    {
        throw std::invalid_argument("температура термостата должна быть от " +
                                    std::to_string(Thermostat::MIN_TEMPERATURE) + " до " +
                                    std::to_string(Thermostat::MAX_TEMPERATURE) + " °C");
    }
}

} // namespace

Thermostat::Thermostat(const std::string& name, double power, int temperature, Mode mode)
    : PoweredDevice(name, power), temperature_(temperature), mode_(mode)
{
    checkTemperature(temperature_);
}

Thermostat::~Thermostat()
{
    if (isOn())
    {
        turnOff(); // засчитать энергию за последний интервал работы
    }
}

std::string Thermostat::getStatus() const
{
    // getPowerUsage() здесь - реализация по умолчанию из PoweredDevice
    return "Термостат «" + getName() + "»: " + (isOn() ? "включён" : "выключен") +
           ", режим «" + modeName(mode_) + "», температура " + std::to_string(temperature_) +
           " °C, мощность сейчас " + formatNumber(getPowerUsage(), 1) + " Вт из " +
           formatNumber(getPowerConsumption(), 1) + " Вт, засчитано " +
           formatNumber(getEnergyConsumed(), 3) + " кВт·ч";
}

int Thermostat::getTemperature() const
{
    return temperature_;
}

Thermostat::Mode Thermostat::getMode() const
{
    return mode_;
}

void Thermostat::setTemperature(int temperature)
{
    checkTemperature(temperature); // сначала проверка: при ошибке термостат не меняется
    temperature_ = temperature;    // мощность от температуры не зависит - updateMeter() не нужен
}

void Thermostat::setMode(Mode mode)
{
    mode_ = mode; // мощность от режима не зависит - updateMeter() не нужен
}

std::string Thermostat::modeName(Mode mode)
{
    switch (mode)
    {
    case Mode::Heating:
        return "обогрев";
    case Mode::Cooling:
        return "охлаждение";
    case Mode::Eco:
        return "эко";
    }
    return "неизвестный режим"; // сюда попасть нельзя: все режимы перечислены выше
}
