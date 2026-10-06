/**
 * @file SmartOutlet.cpp
 * @brief Реализация класса SmartOutlet.
 * @author Vareshka86
 * @date 2026-10-06
 * @version 0.3
 */

#include "SmartOutlet.h"
#include "format.h"

#include <cmath>
#include <stdexcept>

/// Вспомогательные функции, видимые только внутри этого файла.
namespace
{

/**
 * @brief Проверяет нагрузку и при ошибке бросает исключение.
 * @param load     Нагрузка в ваттах.
 * @param maxPower Наибольшая допустимая нагрузка розетки.
 * @exception std::invalid_argument Если нагрузка вне диапазона [0; maxPower].
 */
void checkLoad(double load, double maxPower)
{
    if (!std::isfinite(load) || load < 0.0 || load > maxPower)
    {
        throw std::invalid_argument("нагрузка розетки должна быть от 0 до " + formatNumber(maxPower, 1) + " Вт");
    }
}

/**
 * @brief Проверяет напряжение и при ошибке бросает исключение.
 * @param voltage Напряжение в вольтах.
 * @exception std::invalid_argument Если напряжение вне диапазона.
 */
void checkVoltage(double voltage)
{
    if (!std::isfinite(voltage) || voltage < SmartOutlet::MIN_VOLTAGE || voltage > SmartOutlet::MAX_VOLTAGE)
    {
        throw std::invalid_argument("напряжение сети должно быть от " + std::to_string(SmartOutlet::MIN_VOLTAGE) +
                                    " до " + std::to_string(SmartOutlet::MAX_VOLTAGE) + " В");
    }
}

} // namespace

SmartOutlet::SmartOutlet(const std::string& name, double maxPower, double load, double voltage)
    : PoweredDevice(name, maxPower), // ISensor конструктора с параметрами не имеет - его вызывать не нужно
      load_(load),
      voltage_(voltage)
{
    checkLoad(load_, getPowerConsumption());
    checkVoltage(voltage_);
}

SmartOutlet::SmartOutlet(const SmartOutlet& other)
    : PoweredDevice(other), // копия первой базовой части
      ISensor(other),       // копия второй базовой части: у множественного наследования вызывается каждая
      load_(other.load_),
      voltage_(other.voltage_)
{
}

SmartOutlet& SmartOutlet::operator=(const SmartOutlet& other)
{
    if (this != &other)
    {
        PoweredDevice::operator=(other); // сначала: засчитывает прошлое по старой нагрузке
        ISensor::operator=(other);
        load_ = other.load_;
        voltage_ = other.voltage_;
    }
    return *this;
}

SmartOutlet::~SmartOutlet()
{
    if (isOn())
    {
        turnOff(); // засчитать энергию за последний интервал работы
    }
}

double SmartOutlet::getPowerUsage() const
{
    return isOn() ? load_ : 0.0;
}

std::string SmartOutlet::getStatus() const
{
    return "Розетка «" + getName() + "»: " + (isOn() ? "включена" : "выключена") + ", нагрузка " +
           formatNumber(load_, 1) + " Вт, напряжение " + formatNumber(voltage_, 1) + " В, ток " +
           formatNumber(getCurrentAmperage(), 2) + " А, мощность сейчас " + formatNumber(getPowerUsage(), 1) +
           " Вт из " + formatNumber(getPowerConsumption(), 1) + " Вт, засчитано " +
           formatNumber(getEnergyConsumed(), 3) + " кВт·ч";
}

double SmartOutlet::getCurrentVoltage() const
{
    return voltage_;
}

double SmartOutlet::getCurrentAmperage() const
{
    // P = U × I, отсюда I = P / U; напряжение всегда не меньше MIN_VOLTAGE, деления на 0 нет
    return getPowerUsage() / voltage_;
}

double SmartOutlet::getLoad() const
{
    return load_;
}

void SmartOutlet::setLoad(double load)
{
    checkLoad(load, getPowerConsumption()); // сначала проверка: при ошибке розетка не меняется
    updateMeter();                          // энергия до изменения - по старой нагрузке
    load_ = load;
}

void SmartOutlet::setVoltage(double voltage)
{
    checkVoltage(voltage);
    voltage_ = voltage; // напряжение на мощность не влияет - updateMeter() не нужен
}
