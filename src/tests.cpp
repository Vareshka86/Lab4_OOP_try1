/**
 * @file tests.cpp
 * @brief Реализация тестов иерархии устройств умного дома.
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.1
 */

#include "tests.h"
#include "format.h"
#include "LightBulb.h"
#include "PoweredDevice.h"
#include "SmartDevice.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

/// Счётчики проверок и вспомогательные функции, видимые только внутри этого файла.
namespace
{

/// Сколько проверок выполнено.
int g_checksTotal = 0;

/// Сколько проверок прошло успешно.
int g_checksPassed = 0;

/**
 * @brief Выводит заголовок группы тестов.
 * @param title Название группы.
 */
void printSection(const std::string& title)
{
    std::cout << "\n==================== " << title << " ====================\n";
}

/**
 * @brief Засчитывает одну проверку и печатает её результат.
 * @param description Что проверялось.
 * @param passed      true — ожидание совпало с результатом.
 */
void report(const std::string& description, bool passed)
{
    ++g_checksTotal;
    if (passed)
    {
        ++g_checksPassed;
    }
    std::cout << "  " << (passed ? "[OK]     " : "[ОШИБКА] ") << description << '\n';
}

/**
 * @brief Сравнивает дробные числа с учётом погрешности вычислений.
 * @param a Первое число.
 * @param b Второе число.
 * @return true, если числа различаются меньше чем на 10⁻⁹.
 */
bool near(double a, double b)
{
    return std::fabs(a - b) < 1e-9;
}

/**
 * @brief Проверяет, что действие бросает std::invalid_argument.
 * @tparam Action Тип действия (лямбда-функция без параметров).
 * @param action Действие.
 * @return true, если было брошено std::invalid_argument.
 */
template <typename Action>
bool throwsInvalidArgument(Action action)
{
    try
    {
        action();
    }
    catch (const std::invalid_argument& error)
    {
        std::cout << "    исключение: " << error.what() << '\n';
        return true;
    }
    return false;
}

/**
 * @brief Сдвигает модельное время и печатает, что произошло.
 * @param hours На сколько часов сдвинуть время.
 */
void advance(double hours)
{
    PoweredDevice::advanceClock(hours);
    std::cout << "PoweredDevice::advanceClock(" << formatNumber(hours, 1)
              << ");   // модельное время: " << formatNumber(PoweredDevice::getClockHours(), 1)
              << " ч\n";
}

} // namespace

void testCreation()
{
    printSection("Тест 1. Создание объектов");
    const int countBefore = SmartDevice::getExistingCount();

    std::cout << "LightBulb kitchen(\"Кухня\", 60.0, 80, \"тёплый белый\");\n";
    LightBulb kitchen("Кухня", 60.0, 80, "тёплый белый");
    std::cout << "  " << kitchen.getStatus() << '\n';

    report("имя, мощность, яркость и цвет записаны",
           kitchen.getName() == "Кухня" && near(kitchen.getPowerConsumption(), 60.0) &&
               kitchen.getBrightness() == 80 && kitchen.getColor() == "тёплый белый");
    report("новая лампа выключена и ничего не тратит",
           !kitchen.isOn() && near(kitchen.getPowerUsage(), 0.0) && near(kitchen.getEnergyConsumed(), 0.0));
    report("SmartDevice::getExistingCount() вырос на 1",
           SmartDevice::getExistingCount() == countBefore + 1);
}

void testPolymorphism()
{
    printSection("Тест 2. Полиморфные вызовы");
    LightBulb hall("Прихожая", 60.0, 50, "белый");
    SmartDevice* device = &hall;   // указатель на базовый абстрактный класс
    PoweredDevice& powered = hall; // ссылка на промежуточный класс

    std::cout << "LightBulb hall(\"Прихожая\", 60.0, 50, \"белый\");\n"
              << "SmartDevice* device = &hall;\n"
              << "PoweredDevice& powered = hall;\n"
              << "device->turnOn();\n";
    report("turnOn() через указатель на SmartDevice включил лампу", device->turnOn() && hall.isOn());

    std::cout << "device->getStatus():\n  " << device->getStatus() << '\n';
    report("getStatus() через указатель на SmartDevice вызвал версию LightBulb",
           device->getStatus().find("Лампа «Прихожая»") == 0);
    report("powered.getPowerUsage() вызвал версию LightBulb: 60 Вт × 50 % = 30 Вт",
           near(powered.getPowerUsage(), 30.0));
    report("реализация по умолчанию PoweredDevice::getPowerUsage() дала бы все 60 Вт",
           near(powered.PoweredDevice::getPowerUsage(), 60.0));
    report("повторный turnOn() возвращает false — лампа уже включена", !device->turnOn());
    report("turnOff() через указатель на SmartDevice выключил лампу", device->turnOff() && !hall.isOn());
    report("повторный turnOff() возвращает false", !device->turnOff());
}

void testEnergyAccounting()
{
    printSection("Тест 3. Учёт энергии и статические члены");
    LightBulb living("Гостиная", 100.0, 100, "белый");
    LightBulb desk("Стол", 40.0, 100, "холодный белый");
    const double totalBefore = PoweredDevice::getTotalEnergyConsumed();
    std::cout << "Общая энергия до теста: PoweredDevice::getTotalEnergyConsumed() = "
              << formatNumber(totalBefore, 3) << " кВт·ч\n";

    std::cout << "\nliving.turnOn();   // 100 Вт\n";
    living.turnOn();
    advance(3.0);
    std::cout << "  до выключения:   " << living.getStatus() << '\n'
              << "living.turnOff();\n";
    living.turnOff();
    std::cout << "  после выключения: " << living.getStatus() << '\n';
    report("100 Вт × 3 ч = 0.300 кВт·ч засчитано лампе", near(living.getEnergyConsumed(), 0.3));
    report("общий счётчик вырос на 0.300 кВт·ч",
           near(PoweredDevice::getTotalEnergyConsumed(), totalBefore + 0.3));

    advance(5.0);
    report("пока лампа выключена, её энергия не растёт", near(living.getEnergyConsumed(), 0.3));

    std::cout << "\nliving.turnOn();\n";
    living.turnOn();
    advance(2.0);
    std::cout << "living.setBrightness(50);   // меняем яркость у включённой лампы\n";
    living.setBrightness(50);
    advance(2.0);
    living.turnOff();
    std::cout << "living.turnOff();\n  " << living.getStatus() << '\n';
    report("100 Вт × 2 ч + 50 Вт × 2 ч = 0.300 кВт·ч, всего у лампы 0.600",
           near(living.getEnergyConsumed(), 0.6));

    std::cout << "\nliving.turnOn();  desk.turnOn();   // 50 Вт и 40 Вт одновременно\n";
    living.turnOn();
    desk.turnOn();
    advance(1.0);
    living.turnOff();
    desk.turnOff();
    std::cout << "living.turnOff();  desk.turnOff();\n"
              << "  " << living.getStatus() << '\n'
              << "  " << desk.getStatus() << '\n';
    report("гостиная: +0.050 кВт·ч (всего 0.650), стол: 0.040 кВт·ч",
           near(living.getEnergyConsumed(), 0.65) && near(desk.getEnergyConsumed(), 0.04));

    const double totalAfter = PoweredDevice::getTotalEnergyConsumed();
    std::cout << "Общая энергия после теста: " << formatNumber(totalAfter, 3) << " кВт·ч\n";
    report("общий статический счётчик вырос ровно на сумму по лампам (0.690 кВт·ч)",
           near(totalAfter - totalBefore, living.getEnergyConsumed() + desk.getEnergyConsumed()));
}

void testInvalidArguments()
{
    printSection("Тест 4. Неверные параметры и исключения");
    const int countBefore = SmartDevice::getExistingCount();

    std::cout << "LightBulb(\"\", 60.0, 50, \"белый\")\n";
    report("пустое имя — исключение",
           throwsInvalidArgument([] { LightBulb bulb("", 60.0, 50, "белый"); }));
    std::cout << "LightBulb(\"Лампа\", 0.0, 50, \"белый\") и мощность -5.0\n";
    report("мощность 0 и -5 Вт — исключение",
           throwsInvalidArgument([] { LightBulb bulb("Лампа", 0.0, 50, "белый"); }) &&
               throwsInvalidArgument([] { LightBulb bulb("Лампа", -5.0, 50, "белый"); }));
    std::cout << "LightBulb(\"Лампа\", 60.0, 0, …) и яркость 101\n";
    report("яркость 0 % и 101 % — исключение",
           throwsInvalidArgument([] { LightBulb bulb("Лампа", 60.0, 0, "белый"); }) &&
               throwsInvalidArgument([] { LightBulb bulb("Лампа", 60.0, 101, "белый"); }));
    std::cout << "LightBulb(\"Лампа\", 60.0, 50, \"\")\n";
    report("пустой цвет — исключение",
           throwsInvalidArgument([] { LightBulb bulb("Лампа", 60.0, 50, ""); }));
    report("после неудачных попыток счётчик устройств не изменился",
           SmartDevice::getExistingCount() == countBefore);

    LightBulb lamp("Ночник", 5.0, 30, "жёлтый");
    std::cout << "\nLightBulb lamp(\"Ночник\", 5.0, 30, \"жёлтый\");\nlamp.setBrightness(150);\n";
    report("setBrightness(150) — исключение, яркость осталась 30 %",
           throwsInvalidArgument([&lamp] { lamp.setBrightness(150); }) && lamp.getBrightness() == 30);
    std::cout << "lamp.setColor(\"\");\n";
    report("setColor(\"\") — исключение, цвет прежний",
           throwsInvalidArgument([&lamp] { lamp.setColor(""); }) && lamp.getColor() == "жёлтый");

    const double clockBefore = PoweredDevice::getClockHours();
    std::cout << "PoweredDevice::advanceClock(0.0) и advanceClock(-1.0)\n";
    report("сдвиг времени на 0 и назад — исключение, время прежнее",
           throwsInvalidArgument([] { PoweredDevice::advanceClock(0.0); }) &&
               throwsInvalidArgument([] { PoweredDevice::advanceClock(-1.0); }) &&
               near(PoweredDevice::getClockHours(), clockBefore));
}

void testVirtualDestructor()
{
    printSection("Тест 5. Виртуальный деструктор");
    const int countBefore = SmartDevice::getExistingCount();
    const double totalBefore = PoweredDevice::getTotalEnergyConsumed();

    std::cout << "SmartDevice* device = new LightBulb(\"Ванная\", 60.0, 100, \"белый\");\n"
              << "device->turnOn();\n";
    SmartDevice* device = new LightBulb("Ванная", 60.0, 100, "белый");
    device->turnOn();
    advance(1.0);
    report("устройств стало на одно больше", SmartDevice::getExistingCount() == countBefore + 1);

    std::cout << "delete device;   // лампа включена, удаляем через указатель на SmartDevice\n";
    delete device;
    device = nullptr;

    report("вызвался деструктор LightBulb: он выключил лампу и засчитал 60 Вт × 1 ч = 0.060 кВт·ч",
           near(PoweredDevice::getTotalEnergyConsumed(), totalBefore + 0.06));
    report("отработали деструкторы всех уровней: счётчик устройств вернулся",
           SmartDevice::getExistingCount() == countBefore);
}

bool runAllTests()
{
    testCreation();
    testPolymorphism();
    testEnergyAccounting();
    testInvalidArguments();
    testVirtualDestructor();

    printSection("Итог");
    std::cout << "Проверок пройдено: " << g_checksPassed << " из " << g_checksTotal << '\n'
              << "Устройств сейчас: " << SmartDevice::getExistingCount() << '\n'
              << "Модельное время: " << formatNumber(PoweredDevice::getClockHours(), 1) << " ч\n"
              << "Всего засчитано энергии: "
              << formatNumber(PoweredDevice::getTotalEnergyConsumed(), 3) << " кВт·ч\n";
    return g_checksPassed == g_checksTotal;
}
