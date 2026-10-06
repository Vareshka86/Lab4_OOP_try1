/**
 * @file SmartDevice.cpp
 * @brief Реализация класса SmartDevice.
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.1
 */

#include "SmartDevice.h"

#include <stdexcept>

// Определение статического поля: одно на всю программу, общее для всех устройств
int SmartDevice::existingCount_ = 0;

SmartDevice::SmartDevice(const std::string& name)
    : name_(name), isOn_(false)
{
    if (name_.empty())
    {
        throw std::invalid_argument("имя устройства не может быть пустым");
    }
    ++existingCount_; // только после проверки: при исключении объекта нет
}

SmartDevice::SmartDevice(const SmartDevice& other)
    : name_(other.name_ + " (копия)"), isOn_(other.isOn_)
{
    ++existingCount_; // копия - ещё одно существующее устройство
}

SmartDevice& SmartDevice::operator=(const SmartDevice& other)
{
    if (this != &other) // проверка самоприсваивания
    {
        isOn_ = other.isOn_; // имя не меняем: оно принадлежит этому устройству
    }
    return *this;
}

SmartDevice::~SmartDevice()
{
    --existingCount_;
}

const std::string& SmartDevice::getName() const
{
    return name_;
}

bool SmartDevice::isOn() const
{
    return isOn_;
}

int SmartDevice::getExistingCount()
{
    return existingCount_;
}

void SmartDevice::setOn(bool on)
{
    isOn_ = on;
}
