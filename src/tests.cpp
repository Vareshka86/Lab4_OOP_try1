/**
 * @file tests.cpp
 * @brief Реализация тестов иерархии устройств умного дома.
 * @author Vareshka86
 * @date 2026-10-06
 * @version 1.0
 */

#include "tests.h"
#include "format.h"
#include "ISensor.h"
#include "LightBulb.h"
#include "PoweredDevice.h"
#include "SmartDevice.h"
#include "SmartOutlet.h"
#include "Thermostat.h"

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

    std::cout << "\nThermostat bedroom(\"Спальня\", 1500.0, 22, Thermostat::Mode::Heating);\n";
    Thermostat bedroom("Спальня", 1500.0, 22, Thermostat::Mode::Heating);
    std::cout << "  " << bedroom.getStatus() << '\n';
    report("термостат: имя, мощность, температура и режим записаны",
           bedroom.getName() == "Спальня" && near(bedroom.getPowerConsumption(), 1500.0) &&
               bedroom.getTemperature() == 22 && bedroom.getMode() == Thermostat::Mode::Heating);
    report("новый термостат выключен и ничего не тратит",
           !bedroom.isOn() && near(bedroom.getPowerUsage(), 0.0));

    std::cout << "bedroom.setTemperature(18);  bedroom.setMode(Thermostat::Mode::Eco);\n";
    bedroom.setTemperature(18);
    bedroom.setMode(Thermostat::Mode::Eco);
    std::cout << "  " << bedroom.getStatus() << '\n';
    report("температура и режим изменены; Thermostat::modeName(Mode::Eco) = «эко»",
           bedroom.getTemperature() == 18 && bedroom.getMode() == Thermostat::Mode::Eco &&
               Thermostat::modeName(Thermostat::Mode::Eco) == "эко");
    report("устройств стало на 2 больше (лампа и термостат)",
           SmartDevice::getExistingCount() == countBefore + 2);
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

    Thermostat bedroom("Спальня", 1500.0, 22, Thermostat::Mode::Heating);
    device = &bedroom;
    PoweredDevice& heater = bedroom;
    std::cout << "\nThermostat bedroom(\"Спальня\", 1500.0, 22, Thermostat::Mode::Heating);\n"
              << "device = &bedroom;\n"
              << "PoweredDevice& heater = bedroom;\n"
              << "device->turnOn();\n";
    device->turnOn();
    std::cout << "device->getStatus():\n  " << device->getStatus() << '\n';
    report("тот же указатель device теперь вызвал getStatus() термостата",
           device->getStatus().find("Термостат «Спальня»") == 0);
    report("heater.getPowerUsage(): термостат не переопределяет её — реализация по умолчанию, все 1500 Вт",
           near(heater.getPowerUsage(), 1500.0));
    device->turnOff();
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

    std::cout << "\nThermostat(\"Термостат\", 1000.0, 4, …) и температура 36\n";
    report("температура 4 °C и 36 °C — исключение",
           throwsInvalidArgument([] { Thermostat t("Термостат", 1000.0, 4, Thermostat::Mode::Heating); }) &&
               throwsInvalidArgument([] { Thermostat t("Термостат", 1000.0, 36, Thermostat::Mode::Cooling); }));
    report("после неудачных попыток счётчик устройств не изменился (одна лампа «Ночник»)",
           SmartDevice::getExistingCount() == countBefore + 1);

    Thermostat office("Кабинет", 1000.0, 21, Thermostat::Mode::Eco);
    std::cout << "Thermostat office(\"Кабинет\", 1000.0, 21, Thermostat::Mode::Eco);\noffice.setTemperature(40);\n";
    report("setTemperature(40) — исключение, температура осталась 21 °C",
           throwsInvalidArgument([&office] { office.setTemperature(40); }) && office.getTemperature() == 21);

    std::cout << "\nSmartOutlet(\"Розетка\", 3500.0, 4000.0, 230.0), нагрузка -1 и напряжение 170 и 270 В\n";
    report("нагрузка больше номинальной и меньше 0, напряжение 170 и 270 В — исключение",
           throwsInvalidArgument([] { SmartOutlet o("Розетка", 3500.0, 4000.0, 230.0); }) &&
               throwsInvalidArgument([] { SmartOutlet o("Розетка", 3500.0, -1.0, 230.0); }) &&
               throwsInvalidArgument([] { SmartOutlet o("Розетка", 3500.0, 100.0, 170.0); }) &&
               throwsInvalidArgument([] { SmartOutlet o("Розетка", 3500.0, 100.0, 270.0); }));

    SmartOutlet outlet("Тостер", 2000.0, 800.0, 230.0);
    std::cout << "SmartOutlet outlet(\"Тостер\", 2000.0, 800.0, 230.0);\n"
              << "outlet.setLoad(2500.0);  outlet.setVoltage(300.0);\n";
    report("setLoad(2500) и setVoltage(300) — исключение, нагрузка и напряжение прежние",
           throwsInvalidArgument([&outlet] { outlet.setLoad(2500.0); }) &&
               throwsInvalidArgument([&outlet] { outlet.setVoltage(300.0); }) &&
               near(outlet.getLoad(), 800.0) && near(outlet.getCurrentVoltage(), 230.0));

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

void testDeviceArray()
{
    printSection("Тест 6. Массив устройств через указатели на базовый класс");
    const int countBefore = SmartDevice::getExistingCount();
    const double totalBefore = PoweredDevice::getTotalEnergyConsumed();

    // Массив указателей на абстрактный SmartDevice: в нём лежат объекты разных
    // классов. Параметры заведомо верные, поэтому конструкторы исключений не бросят.
    const int DEVICE_COUNT = 4;
    SmartDevice* devices[DEVICE_COUNT] = {
        new LightBulb("Кухня", 60.0, 100, "тёплый белый"),
        new Thermostat("Спальня", 1500.0, 22, Thermostat::Mode::Heating),
        new LightBulb("Коридор", 40.0, 50, "белый"),
        new Thermostat("Кабинет", 1000.0, 24, Thermostat::Mode::Cooling)};

    std::cout << "SmartDevice* devices[4] = {\n"
              << "    new LightBulb(\"Кухня\", 60.0, 100, \"тёплый белый\"),\n"
              << "    new Thermostat(\"Спальня\", 1500.0, 22, Thermostat::Mode::Heating),\n"
              << "    new LightBulb(\"Коридор\", 40.0, 50, \"белый\"),\n"
              << "    new Thermostat(\"Кабинет\", 1000.0, 24, Thermostat::Mode::Cooling)};\n";
    report("создано 4 устройства", SmartDevice::getExistingCount() == countBefore + 4);

    std::cout << "\nfor (SmartDevice* device : devices) device->turnOn();\n";
    bool allTurnedOn = true;
    for (SmartDevice* device : devices)
    {
        allTurnedOn = device->turnOn() && allTurnedOn;
    }
    int onCount = 0;
    for (const SmartDevice* device : devices)
    {
        if (device->isOn())
        {
            ++onCount;
        }
    }
    report("включены все 4 устройства", allTurnedOn && onCount == DEVICE_COUNT);

    advance(2.0);

    std::cout << "for (SmartDevice* device : devices) device->turnOff();\n";
    for (SmartDevice* device : devices)
    {
        device->turnOff();
    }
    std::cout << "Статистика:\n";
    for (const SmartDevice* device : devices)
    {
        std::cout << "  " << device->getStatus() << '\n'; // версия getStatus() - по типу объекта
    }

    onCount = 0;
    for (const SmartDevice* device : devices)
    {
        if (device->isOn())
        {
            ++onCount;
        }
    }
    report("после выключения ни одно устройство не включено", onCount == 0);

    // Кухня 60 Вт, Спальня 1500 Вт, Коридор 40 Вт × 50 % = 20 Вт, Кабинет 1000 Вт;
    // (60 + 1500 + 20 + 1000) Вт × 2 ч / 1000 = 5.160 кВт·ч
    const double added = PoweredDevice::getTotalEnergyConsumed() - totalBefore;
    std::cout << "Общая энергия выросла на " << formatNumber(added, 3) << " кВт·ч\n";
    report("общий счётчик: (60 + 1500 + 20 + 1000) Вт × 2 ч = 5.160 кВт·ч", near(added, 5.16));

    std::cout << "\nfor (SmartDevice*& device : devices) { delete device; device = nullptr; }\n";
    for (SmartDevice*& device : devices)
    {
        delete device; // виртуальный деструктор: удаляется весь объект
        device = nullptr;
    }
    report("все устройства удалены: счётчик устройств вернулся",
           SmartDevice::getExistingCount() == countBefore);
}

void testMultipleInheritance()
{
    printSection("Тест 7. Множественное наследование: розетка с датчиком");
    const int countBefore = SmartDevice::getExistingCount();
    const double totalBefore = PoweredDevice::getTotalEnergyConsumed();

    SmartOutlet kettle("Чайник", 3500.0, 2000.0, 230.0);
    SmartDevice* device = &kettle;   // розетка как умное устройство
    PoweredDevice& powered = kettle; // как устройство с питанием
    ISensor& sensor = kettle;        // как датчик - второй базовый класс
    std::cout << "SmartOutlet kettle(\"Чайник\", 3500.0, 2000.0, 230.0);\n"
              << "SmartDevice* device = &kettle;\n"
              << "PoweredDevice& powered = kettle;\n"
              << "ISensor& sensor = kettle;\n"
              << "device->turnOn();\n";
    device->turnOn();
    std::cout << "device->getStatus():\n  " << device->getStatus() << '\n';
    report("через SmartDevice* вызвана getStatus() розетки", device->getStatus().find("Розетка «Чайник»") == 0);
    report("powered.getPowerUsage(): розетка переопределила её — мощность нагрузки 2000 Вт",
           near(powered.getPowerUsage(), 2000.0));
    report("sensor.getCurrentVoltage() через ISensor& — 230 В", near(sensor.getCurrentVoltage(), 230.0));
    report("ток: 2000 Вт / 230 В = 8.70 А", formatNumber(kettle.getCurrentAmperage(), 2) == "8.70");

    advance(1.0);
    std::cout << "kettle.setLoad(500.0);   // подключили прибор меньшей мощности\n";
    kettle.setLoad(500.0);
    advance(2.0);
    std::cout << "device->turnOff();\n";
    device->turnOff();
    std::cout << "  " << kettle.getStatus() << '\n';
    report("2000 Вт × 1 ч + 500 Вт × 2 ч = 3.000 кВт·ч засчитано розетке и общему счётчику",
           near(kettle.getEnergyConsumed(), 3.0) &&
               near(PoweredDevice::getTotalEnergyConsumed() - totalBefore, 3.0));

    std::cout << "kettle.setVoltage(215.0);   // розетка выключена, датчик работает\n";
    kettle.setVoltage(215.0);
    report("выключенная розетка: мощность 0, датчик показывает 215 В",
           near(kettle.getPowerUsage(), 0.0) && near(sensor.getCurrentVoltage(), 215.0));

    // Массив устройств: датчики среди них находим через dynamic_cast
    const int DEVICE_COUNT = 3;
    SmartDevice* devices[DEVICE_COUNT] = {
        new LightBulb("Гостиная", 100.0, 100, "белый"),
        new SmartOutlet("Обогреватель", 3500.0, 1500.0, 228.0),
        new Thermostat("Детская", 1200.0, 23, Thermostat::Mode::Eco)};
    std::cout << "\nSmartDevice* devices[3] = {лампа «Гостиная», розетка «Обогреватель», термостат «Детская»};\n"
              << "Датчики среди устройств - dynamic_cast<const ISensor*>(device):\n";
    int sensorCount = 0;
    for (const SmartDevice* item : devices)
    {
        const ISensor* itemSensor = dynamic_cast<const ISensor*>(item); // nullptr, если объект не датчик
        if (itemSensor != nullptr)
        {
            ++sensorCount;
            std::cout << "  " << item->getName() << ": датчик, напряжение "
                      << formatNumber(itemSensor->getCurrentVoltage(), 1) << " В\n";
        }
        else
        {
            std::cout << "  " << item->getName() << ": не датчик\n";
        }
    }
    report("датчик среди трёх устройств один — розетка, 228 В", sensorCount == 1);

    for (SmartDevice*& item : devices)
    {
        delete item;
        item = nullptr;
    }

    // Удаление через указатель на второй базовый класс
    SmartOutlet* washer = new SmartOutlet("Стиральная машина", 2500.0, 2000.0, 230.0);
    washer->turnOn();
    std::cout << "\nSmartOutlet* washer = new SmartOutlet(\"Стиральная машина\", 2500.0, 2000.0, 230.0);\n"
              << "washer->turnOn();\n";
    advance(0.5);
    const double totalBeforeDelete = PoweredDevice::getTotalEnergyConsumed();
    ISensor* washerSensor = washer;
    std::cout << "ISensor* washerSensor = washer;\ndelete washerSensor;   // розетка включена\n";
    delete washerSensor;
    washerSensor = nullptr;
    washer = nullptr;
    report("delete через ISensor*: вызвался деструктор розетки, засчитано 2000 Вт × 0.5 ч = 1.000 кВт·ч",
           near(PoweredDevice::getTotalEnergyConsumed() - totalBeforeDelete, 1.0));
    report("все временные устройства удалены (осталась только розетка «Чайник»)",
           SmartDevice::getExistingCount() == countBefore + 1);
}

void testCopyAndAssignment()
{
    printSection("Тест 8. Копирование и присваивание");
    const int countBefore = SmartDevice::getExistingCount();
    const double totalBefore = PoweredDevice::getTotalEnergyConsumed();

    // --- Конструктор копирования: выключенная лампа
    LightBulb original("Кухня", 60.0, 80, "тёплый белый");
    std::cout << "LightBulb original(\"Кухня\", 60.0, 80, \"тёплый белый\");\n"
              << "LightBulb copy(original);\n";
    LightBulb copy(original);
    std::cout << "  " << copy.getStatus() << '\n';
    report("копия: те же мощность, яркость и цвет, состояние «выключена»",
           near(copy.getPowerConsumption(), 60.0) && copy.getBrightness() == 80 &&
               copy.getColor() == "тёплый белый" && !copy.isOn());
    report("имя копии — «Кухня (копия)»: копия отличается от оригинала", copy.getName() == "Кухня (копия)");
    report("копия учтена в счётчике устройств: стало на 2 больше",
           SmartDevice::getExistingCount() == countBefore + 2);

    std::cout << "copy.setBrightness(20);  copy.setColor(\"синий\");\n";
    copy.setBrightness(20);
    copy.setColor("синий");
    report("независимость: оригинал не изменился (яркость 80 %, цвет «тёплый белый»)",
           original.getBrightness() == 80 && original.getColor() == "тёплый белый");
    original.turnOn();
    report("независимость: включение оригинала не включило копию", original.isOn() && !copy.isOn());
    original.turnOff();

    // --- Конструктор копирования: включённый термостат
    Thermostat heater("Спальня", 1500.0, 22, Thermostat::Mode::Heating);
    heater.turnOn();
    advance(2.0);
    std::cout << "\nThermostat heater(\"Спальня\", 1500.0, 22, Thermostat::Mode::Heating);\n"
              << "heater.turnOn();   // работает 2 часа\n"
              << "Thermostat twin(heater);   // копия включённого термостата\n";
    const double totalBeforeCopy = PoweredDevice::getTotalEnergyConsumed();
    Thermostat twin(heater);
    std::cout << "  " << twin.getStatus() << '\n';
    report("копия включённого термостата тоже включена, температура и режим те же",
           twin.isOn() && twin.getTemperature() == 22 && twin.getMode() == Thermostat::Mode::Heating);
    report("энергия оригинала копии не передана: у копии 0 кВт·ч, общий счётчик при копировании не изменился",
           near(twin.getEnergyConsumed(), 0.0) &&
               near(PoweredDevice::getTotalEnergyConsumed(), totalBeforeCopy));
    advance(1.0);
    heater.turnOff();
    twin.turnOff();
    report("через час: оригинал 1500 Вт × 3 ч = 4.500 кВт·ч, копия — только свой час: 1.500 кВт·ч",
           near(heater.getEnergyConsumed(), 4.5) && near(twin.getEnergyConsumed(), 1.5));
    report("сумма по двум устройствам равна приросту общего счётчика (6.000 кВт·ч)",
           near(PoweredDevice::getTotalEnergyConsumed() - totalBefore,
                heater.getEnergyConsumed() + twin.getEnergyConsumed()));

    // --- Присваивание: параметры и состояние берутся из другой лампы, имя и энергия остаются
    LightBulb lamp("Стол", 100.0, 100, "белый");
    LightBulb dim("Ночник", 40.0, 25, "жёлтый");
    lamp.turnOn();
    advance(2.0);
    std::cout << "\nLightBulb lamp(\"Стол\", 100.0, 100, \"белый\");   lamp.turnOn();   // 2 часа на 100 Вт\n"
              << "LightBulb dim(\"Ночник\", 40.0, 25, \"жёлтый\");   // выключен\n"
              << "lamp = dim;\n";
    lamp = dim;
    std::cout << "  " << lamp.getStatus() << '\n';
    report("после присваивания параметры как у «Ночника»: 40 Вт, 25 %, «жёлтый», лампа выключена",
           near(lamp.getPowerConsumption(), 40.0) && lamp.getBrightness() == 25 &&
               lamp.getColor() == "жёлтый" && !lamp.isOn());
    report("имя осталось своим — «Стол», а не «Ночник»", lamp.getName() == "Стол");
    report("энергия до присваивания засчитана по СТАРЫМ параметрам: 100 Вт × 2 ч = 0.200 кВт·ч",
           near(lamp.getEnergyConsumed(), 0.2));

    std::cout << "lamp.turnOn();  advance(1.0);  lamp.turnOff();\n";
    lamp.turnOn();
    advance(1.0);
    lamp.turnOff();
    report("дальше учёт идёт по новым параметрам: 40 Вт × 25 % × 1 ч = 0.010, всего 0.210 кВт·ч",
           near(lamp.getEnergyConsumed(), 0.21));

    // --- Присваивание включённого источника выключенному приёмнику
    dim.turnOn();
    std::cout << "\ndim.turnOn();   // источник включён\nlamp = dim;\n";
    lamp = dim;
    report("выключенная лампа после присваивания из включённой стала включённой", lamp.isOn());
    advance(2.0);
    lamp.turnOff();
    dim.turnOff();
    report("учёт у неё начался с момента присваивания: 0.210 + 10 Вт × 2 ч = 0.230 кВт·ч",
           near(lamp.getEnergyConsumed(), 0.23));

    // --- Самоприсваивание
    std::cout << "\nlamp = lamp;   // самоприсваивание\n";
    const double energyBefore = lamp.getEnergyConsumed();
    LightBulb& same = lamp;
    lamp = same; // через ссылку: компилятор не предупреждает о самоприсваивании
    report("самоприсваивание ничего не меняет и не ломает счётчики",
           lamp.getName() == "Стол" && lamp.getBrightness() == 25 && near(lamp.getEnergyConsumed(), energyBefore));

    // --- Розетка: две базовые части
    SmartOutlet outlet("Чайник", 3500.0, 2000.0, 230.0);
    outlet.turnOn();
    std::cout << "\nSmartOutlet outlet(\"Чайник\", 3500.0, 2000.0, 230.0);   outlet.turnOn();\n"
              << "SmartOutlet socketCopy(outlet);\n";
    SmartOutlet socketCopy(outlet);
    std::cout << "  " << socketCopy.getStatus() << '\n';
    ISensor& copySensor = socketCopy;
    report("копия розетки: те же нагрузка 2000 Вт, напряжение 230 В (через ISensor&), состояние «включена»",
           near(socketCopy.getLoad(), 2000.0) && near(copySensor.getCurrentVoltage(), 230.0) && socketCopy.isOn());
    socketCopy.setLoad(500.0);
    socketCopy.setVoltage(215.0);
    report("независимость: оригинал по-прежнему 2000 Вт и 230 В",
           near(outlet.getLoad(), 2000.0) && near(outlet.getCurrentVoltage(), 230.0));
    SmartOutlet other("Тостер", 2000.0, 800.0, 220.0);
    std::cout << "SmartOutlet other(\"Тостер\", 2000.0, 800.0, 220.0);   other = outlet;\n";
    other = outlet;
    report("присваивание розетки: мощность, нагрузка, напряжение и состояние как у «Чайника», имя «Тостер»",
           near(other.getPowerConsumption(), 3500.0) && near(other.getLoad(), 2000.0) &&
               near(other.getCurrentVoltage(), 230.0) && other.isOn() && other.getName() == "Тостер");
    outlet.turnOff();
    socketCopy.turnOff();
    other.turnOff();

    // --- Копии в массиве указателей: каждая — отдельный объект, удаляется самостоятельно
    const int sizeBefore = SmartDevice::getExistingCount();
    SmartDevice* devices[2] = {new LightBulb(original), new Thermostat(heater)};
    std::cout << "\nSmartDevice* devices[2] = {new LightBulb(original), new Thermostat(heater)};\n";
    report("в массиве копии лампы и термостата: имена «Кухня (копия)» и «Спальня (копия)»",
           devices[0]->getName() == "Кухня (копия)" && devices[1]->getName() == "Спальня (копия)" &&
               SmartDevice::getExistingCount() == sizeBefore + 2);
    for (SmartDevice*& device : devices)
    {
        delete device;
        device = nullptr;
    }
    report("после удаления копий счётчик устройств вернулся", SmartDevice::getExistingCount() == sizeBefore);

    // Устройства этого теста (original, copy, heater, twin, lamp, dim, outlet, socketCopy, other) - 9 штук
    report("в тесте создано 9 устройств, пока они живы (до выхода из функции)",
           SmartDevice::getExistingCount() == countBefore + 9);
}

bool runAllTests()
{
    testCreation();
    testPolymorphism();
    testEnergyAccounting();
    testInvalidArguments();
    testVirtualDestructor();
    testDeviceArray();
    testMultipleInheritance();
    testCopyAndAssignment();

    printSection("Итог");
    std::cout << "Проверок пройдено: " << g_checksPassed << " из " << g_checksTotal << '\n'
              << "Устройств сейчас: " << SmartDevice::getExistingCount() << '\n'
              << "Модельное время: " << formatNumber(PoweredDevice::getClockHours(), 1) << " ч\n"
              << "Всего засчитано энергии: "
              << formatNumber(PoweredDevice::getTotalEnergyConsumed(), 3) << " кВт·ч\n";
    return g_checksPassed == g_checksTotal;
}
