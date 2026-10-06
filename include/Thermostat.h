/**
 * @file Thermostat.h
 * @brief Класс Thermostat — умный термостат (конкретное устройство, третий уровень иерархии).
 * @details SmartDevice → PoweredDevice → **Thermostat**. Описание решения — на
 * странице @ref part2.
 * @author Vareshka86
 * @date 2026-10-06
 * @version 0.2
 */

#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "PoweredDevice.h"

#include <string>

/**
 * @brief Умный термостат: заданная температура и режим работы.
 * @details Конкретный класс: реализует getStatus(), единственную оставшуюся
 * чисто виртуальную функцию, поэтому объект Thermostat можно создать.
 *
 * getPowerUsage() термостат **не переопределяет**: работает реализация по
 * умолчанию из PoweredDevice — включённый термостат тратит всю номинальную
 * мощность. Так в иерархии видны оба случая: лампа переопределяет виртуальную
 * функцию, термостат пользуется версией базового класса.
 *
 * Инвариант: температура от MIN_TEMPERATURE до MAX_TEMPERATURE градусов.
 */
class Thermostat : public PoweredDevice
{
public:
    /**
     * @brief Режим работы термостата.
     */
    enum class Mode
    {
        Heating, ///< Обогрев
        Cooling, ///< Охлаждение
        Eco      ///< Экономичный режим
    };

    /// Наименьшая температура, которую можно задать, °C.
    static const int MIN_TEMPERATURE = 5;

    /// Наибольшая температура, которую можно задать, °C.
    static const int MAX_TEMPERATURE = 35;

    /**
     * @brief Создаёт выключенный термостат.
     * @param name        Имя термостата, например «Спальня».
     * @param power       Номинальная мощность в ваттах (больше 0).
     * @param temperature Заданная температура в градусах (от 5 до 35).
     * @param mode        Режим работы.
     * @exception std::invalid_argument Если имя пустое, мощность не больше 0
     * или температура вне диапазона.
     */
    Thermostat(const std::string& name, double power, int temperature, Mode mode);

    /**
     * @brief Конструктор копирования: такой же термостат в том же состоянии.
     * @details Копируются температура, режим, мощность и состояние «включён/выключен»;
     * имя получает пометку « (копия)», накопленная энергия не копируется.
     * @param other Копируемый термостат.
     */
    Thermostat(const Thermostat& other);

    /**
     * @brief Присваивание: термостат становится таким же, как другой.
     * @details Копируются температура, режим, мощность и состояние; имя и накопленная
     * энергия остаются прежними. Самоприсваивание безопасно.
     * @param other Термостат, из которого берутся параметры.
     * @return Ссылка на этот термостат.
     */
    Thermostat& operator=(const Thermostat& other);

    /**
     * @brief Деструктор: включённый термостат перед уничтожением выключает.
     * @details Как и у лампы: выключение засчитывает энергию за последний
     * интервал работы, иначе она пропала бы из статистики.
     */
    ~Thermostat() override;

    /**
     * @brief Описание термостата одной строкой.
     * @return Например «Термостат «Спальня»: включён, режим «обогрев»,
     *         температура 22 °C, мощность сейчас 1500.0 Вт из 1500.0 Вт,
     *         засчитано 0.000 кВт·ч».
     */
    std::string getStatus() const override;

    /**
     * @brief Возвращает заданную температуру.
     * @return Температура в градусах Цельсия.
     */
    int getTemperature() const;

    /**
     * @brief Возвращает режим работы.
     * @return Режим.
     */
    Mode getMode() const;

    /**
     * @brief Задаёт температуру.
     * @param temperature Новая температура в градусах (от 5 до 35).
     * @exception std::invalid_argument Если температура вне диапазона;
     * термостат не меняется.
     */
    void setTemperature(int temperature);

    /**
     * @brief Меняет режим работы.
     * @param mode Новый режим.
     */
    void setMode(Mode mode);

    /**
     * @brief Возвращает название режима по-русски.
     * @details Статическая функция: объект для вызова не нужен —
     * `Thermostat::modeName(Thermostat::Mode::Eco)`.
     * @param mode Режим.
     * @return «обогрев», «охлаждение» или «эко».
     */
    static std::string modeName(Mode mode);

private:
    int temperature_; ///< Заданная температура, °C
    Mode mode_;       ///< Режим работы
};

#endif // THERMOSTAT_H
