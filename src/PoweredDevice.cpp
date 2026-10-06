/**
 * @file PoweredDevice.cpp
 * @brief Реализация класса PoweredDevice.
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.1
 */

#include "PoweredDevice.h"

#include <cmath>
#include <stdexcept>

// Определения статических полей: общие для всех устройств
double PoweredDevice::totalEnergyKWh_ = 0.0;
double PoweredDevice::clockHours_ = 0.0;

PoweredDevice::PoweredDevice(const std::string& name, double powerConsumption)
    : SmartDevice(name), // сначала строится часть базового класса
      powerConsumption_(powerConsumption),
      energyKWh_(0.0),
      meterStartHours_(0.0)
{
    if (!std::isfinite(powerConsumption_) || powerConsumption_ <= 0.0)
    {
        throw std::invalid_argument("номинальная мощность должна быть числом больше 0");
    }
}

PoweredDevice::PoweredDevice(const PoweredDevice& other)
    : SmartDevice(other), // копируются имя (с пометкой) и состояние
      powerConsumption_(other.powerConsumption_),
      energyKWh_(0.0),                 // энергия оригинала копии не передаётся
      meterStartHours_(clockHours_)    // если копия включена, учёт идёт с этого момента
{
}

PoweredDevice& PoweredDevice::operator=(const PoweredDevice& other)
{
    if (this != &other) // проверка самоприсваивания
    {
        updateMeter(); // засчитать прошлое по старым параметрам - ДО их замены
        SmartDevice::operator=(other);
        powerConsumption_ = other.powerConsumption_;
        meterStartHours_ = clockHours_; // новый интервал учёта начинается сейчас
    }
    return *this;
}

bool PoweredDevice::turnOn()
{
    if (isOn())
    {
        return false;
    }
    meterStartHours_ = clockHours_; // интервал учёта начинается сейчас
    setOn(true);
    return true;
}

bool PoweredDevice::turnOff()
{
    if (!isOn())
    {
        return false;
    }
    updateMeter(); // засчитать энергию, ПОКА устройство ещё включено
    setOn(false);
    return true;
}

double PoweredDevice::getPowerUsage() const
{
    // Реализация по умолчанию: включено - номинальная мощность, выключено - 0
    return isOn() ? powerConsumption_ : 0.0;
}

double PoweredDevice::getPowerConsumption() const
{
    return powerConsumption_;
}

double PoweredDevice::getEnergyConsumed() const
{
    return energyKWh_;
}

double PoweredDevice::getTotalEnergyConsumed()
{
    return totalEnergyKWh_;
}

double PoweredDevice::getClockHours()
{
    return clockHours_;
}

void PoweredDevice::advanceClock(double hours)
{
    if (!std::isfinite(hours) || hours <= 0.0)
    {
        throw std::invalid_argument("время можно сдвинуть только вперёд, на число часов больше 0");
    }
    clockHours_ += hours;
}

void PoweredDevice::updateMeter()
{
    if (!isOn())
    {
        return;
    }

    const double hours = clockHours_ - meterStartHours_;
    // Вт × ч = Вт·ч; делим на 1000 - получаем кВт·ч.
    // getPowerUsage() - виртуальный вызов: мощность конкретного устройства
    const double energy = getPowerUsage() * hours / 1000.0;

    energyKWh_ += energy;
    totalEnergyKWh_ += energy;
    meterStartHours_ = clockHours_; // следующий интервал начинается сейчас
}
