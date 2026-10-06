/**
 * @file LightBulb.h
 * @brief Класс LightBulb — умная лампа (конкретное устройство, третий уровень иерархии).
 * @details SmartDevice → PoweredDevice → **LightBulb**. Описание решения — на
 * странице @ref part1.
 * @author Vareshka86
 * @date 2026-10-05
 * @version 0.1
 */

#ifndef LIGHT_BULB_H
#define LIGHT_BULB_H

#include "PoweredDevice.h"

#include <string>

/**
 * @brief Умная лампа: яркость и цвет свечения.
 * @details Конкретный (не абстрактный) класс: в нём есть реализации всех чисто
 * виртуальных функций, поэтому объект LightBulb можно создать.
 *
 * Переопределяет getPowerUsage(): включённая лампа тратит долю номинальной
 * мощности, равную яркости (при яркости 50 % — половину).
 *
 * Инварианты: яркость от MIN_BRIGHTNESS до MAX_BRIGHTNESS процентов; цвет не пустой.
 */
class LightBulb : public PoweredDevice
{
public:
    /// Наименьшая яркость, %.
    static const int MIN_BRIGHTNESS = 1;

    /// Наибольшая яркость, %.
    static const int MAX_BRIGHTNESS = 100;

    /**
     * @brief Создаёт выключенную лампу.
     * @param name       Имя лампы, например «Кухня».
     * @param power      Номинальная мощность в ваттах (больше 0).
     * @param brightness Яркость в процентах (от 1 до 100).
     * @param color      Цвет свечения, например «тёплый белый».
     * @exception std::invalid_argument Если какой-то параметр нарушает инвариант.
     */
    LightBulb(const std::string& name, double power, int brightness, const std::string& color);

    /**
     * @brief Конструктор копирования: такая же лампа в том же состоянии.
     * @details Копируются яркость, цвет, мощность и состояние «включена/выключена»;
     * имя получает пометку « (копия)», накопленная энергия не копируется (см.
     * PoweredDevice::PoweredDevice(const PoweredDevice&)). Копия независима:
     * изменение яркости копии оригинал не затрагивает.
     * @param other Копируемая лампа.
     */
    LightBulb(const LightBulb& other);

    /**
     * @brief Присваивание: лампа становится такой же, как другая.
     * @details Копируются яркость, цвет, мощность и состояние; имя и накопленная
     * энергия остаются прежними. Энергия, потраченная до присваивания, засчитывается
     * по старой яркости. Самоприсваивание безопасно.
     * @param other Лампа, из которой берутся параметры.
     * @return Ссылка на эту лампу.
     */
    LightBulb& operator=(const LightBulb& other);

    /**
     * @brief Деструктор: включённую лампу перед уничтожением выключает.
     * @details Выключение засчитывает энергию за последний интервал работы, иначе
     * она пропала бы из статистики. Вызов turnOff() здесь — это вызов
     * виртуальной функции в деструкторе: пока выполняется тело ~LightBulb(),
     * объект ещё является лампой, и внутри turnOff() вызывается
     * LightBulb::getPowerUsage(). В деструкторе PoweredDevice так сделать уже
     * нельзя: там часть «лампа» уже уничтожена, и вызвалась бы реализация по умолчанию.
     */
    ~LightBulb() override;

    /**
     * @brief Мощность лампы сейчас: номинальная × яркость / 100.
     * @details Переопределение виртуальной функции PoweredDevice::getPowerUsage().
     * @return Мощность в ваттах (0, если лампа выключена).
     */
    double getPowerUsage() const override;

    /**
     * @brief Описание лампы одной строкой.
     * @return Например «Лампа «Кухня»: включена, яркость 80 %, цвет «тёплый белый»,
     *         мощность сейчас 48.0 Вт из 60.0 Вт, засчитано 0.000 кВт·ч».
     */
    std::string getStatus() const override;

    /**
     * @brief Возвращает яркость.
     * @return Яркость в процентах.
     */
    int getBrightness() const;

    /**
     * @brief Возвращает цвет свечения.
     * @return Константная ссылка на цвет.
     */
    const std::string& getColor() const;

    /**
     * @brief Меняет яркость.
     * @details Перед изменением засчитывает энергию по старой яркости
     * (updateMeter()), поэтому учёт остаётся точным, даже если яркость меняют
     * у включённой лампы.
     * @param brightness Новая яркость в процентах (от 1 до 100).
     * @exception std::invalid_argument Если яркость вне диапазона; лампа не меняется.
     */
    void setBrightness(int brightness);

    /**
     * @brief Меняет цвет свечения.
     * @param color Новый цвет (не пустой).
     * @exception std::invalid_argument Если цвет пустой; лампа не меняется.
     */
    void setColor(const std::string& color);

private:
    int brightness_;    ///< Яркость, %
    std::string color_; ///< Цвет свечения (не пустой)
};

#endif // LIGHT_BULB_H
