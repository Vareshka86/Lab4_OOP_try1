/**
 * @file SmartOutlet.h
 * @brief Класс SmartOutlet — умная розетка с датчиком напряжения (множественное наследование).
 * @details SmartDevice → PoweredDevice → **SmartOutlet** ← ISensor. Описание
 * решения — на странице @ref part3.
 * @author Vareshka86
 * @date 2026-10-06
 * @version 0.3
 */

#ifndef SMART_OUTLET_H
#define SMART_OUTLET_H

#include "ISensor.h"
#include "PoweredDevice.h"

#include <string>

/**
 * @brief Умная розетка: питает подключённый прибор и измеряет напряжение сети.
 * @details **Множественное наследование**: розетка одновременно
 * - устройство с питанием (`public PoweredDevice`) — включается, выключается,
 *   учитывает энергию;
 * - датчик (`public ISensor`) — сообщает напряжение getCurrentVoltage().
 *
 * Объект SmartOutlet можно передать туда, где ждут SmartDevice, PoweredDevice
 * или ISensor. Ромба нет: ISensor не наследует SmartDevice, поэтому часть
 * SmartDevice в розетке одна, и виртуальное наследование не нужно.
 *
 * Номинальная мощность (из PoweredDevice) — наибольшая нагрузка, на которую
 * рассчитана розетка. Нагрузка — мощность подключённого прибора; её тратит
 * включённая розетка, поэтому getPowerUsage() переопределена.
 *
 * Инварианты: нагрузка от 0 до номинальной мощности; напряжение от
 * MIN_VOLTAGE до MAX_VOLTAGE вольт.
 */
class SmartOutlet : public PoweredDevice, public ISensor
{
public:
    /// Наименьшее напряжение сети, которое может показать датчик, В.
    static const int MIN_VOLTAGE = 180;

    /// Наибольшее напряжение сети, которое может показать датчик, В.
    static const int MAX_VOLTAGE = 260;

    /**
     * @brief Создаёт выключенную розетку.
     * @param name     Имя розетки, например «Чайник».
     * @param maxPower Наибольшая допустимая нагрузка в ваттах (больше 0).
     * @param load     Мощность подключённого прибора в ваттах (от 0 до maxPower;
     *                 0 — ничего не подключено).
     * @param voltage  Напряжение сети в вольтах (от 180 до 260).
     * @exception std::invalid_argument Если какой-то параметр нарушает инвариант.
     */
    SmartOutlet(const std::string& name, double maxPower, double load, double voltage);

    /**
     * @brief Деструктор: включённую розетку перед уничтожением выключает.
     * @details Как у лампы и термостата: энергия последнего интервала
     * засчитывается. Деструктор вызывается и при удалении через указатель на
     * ISensor — у интерфейса деструктор тоже виртуальный.
     */
    ~SmartOutlet() override;

    /**
     * @brief Мощность сейчас: нагрузка, если розетка включена, иначе 0.
     * @details Переопределение PoweredDevice::getPowerUsage().
     * @return Мощность в ваттах.
     */
    double getPowerUsage() const override;

    /**
     * @brief Описание розетки одной строкой.
     * @return Например «Розетка «Чайник»: включена, нагрузка 2000.0 Вт,
     *         напряжение 230.0 В, ток 8.70 А, мощность сейчас 2000.0 Вт из
     *         3500.0 Вт, засчитано 0.000 кВт·ч».
     */
    std::string getStatus() const override;

    /**
     * @brief Напряжение сети, которое измеряет датчик розетки.
     * @details Реализация чисто виртуальной функции ISensor. Датчик стоит на
     * входе розетки, поэтому показывает напряжение и у выключенной розетки.
     * @return Напряжение в вольтах.
     */
    double getCurrentVoltage() const override;

    /**
     * @brief Ток через розетку: мощность сейчас / напряжение.
     * @return Сила тока в амперах (0, если розетка выключена).
     */
    double getCurrentAmperage() const;

    /**
     * @brief Возвращает мощность подключённого прибора.
     * @return Нагрузка в ваттах.
     */
    double getLoad() const;

    /**
     * @brief Подключает прибор другой мощности.
     * @details Перед изменением засчитывает энергию по старой нагрузке
     * (updateMeter()), как лампа при смене яркости.
     * @param load Новая нагрузка в ваттах (от 0 до номинальной мощности).
     * @exception std::invalid_argument Если нагрузка вне диапазона; розетка не меняется.
     */
    void setLoad(double load);

    /**
     * @brief Задаёт напряжение сети (имитация показаний датчика).
     * @param voltage Напряжение в вольтах (от 180 до 260).
     * @exception std::invalid_argument Если напряжение вне диапазона; розетка не меняется.
     */
    void setVoltage(double voltage);

private:
    double load_;    ///< Мощность подключённого прибора, Вт
    double voltage_; ///< Напряжение сети, В
};

#endif // SMART_OUTLET_H
