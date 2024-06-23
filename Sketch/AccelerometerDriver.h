#pragma once
#include <Arduino.h>

#define SCALE_FACTOR				16384.0
#define GRAVITATIONAL_ACCELERATION	9.806
#define REGISTER_COUNT				3
#define PWR_MGMT_1					0x6B
#define ACCEL_XOUT_H				0x3B // стартовый регистр 0x3B (ACCEL_XOUT_H)

// Драйвер акселерометра
class AccelerometerDriver
{
public:
	// Структура, описывающая положение акселерометра в пространстве
	struct Position
	{
		float xAngle; // Наклон по оси X, градусы
		float yAngle; // Наклон по оси Y, градусы
		float zAngle; // Наклон по оси Z, градусы
	};
	// Конструктор
	// [in] accelerometerAddress - адрес акселерометра на шине I2C
	AccelerometerDriver(uint16_t address);

	// Производит измерение положения акселерометра и возвращает результат
	Position GetAcclelerometerPosition();
	// Получить адрес датчика
	uint16_t GetAddress();
	// Получить текущую позицию
	Position GetCurriosPosition();
	// Получить текущее результирующее ускорение
	float GetResultingAcceleration();

private:
	// Адрес датчика
	uint16_t Address;
	// Текущая позиция позиция
	Position position;
	// Получить данные с регистров акселерометра
	// [in] baseRegister - адрес базового регистра акселерометра
	// [in] registerCount - число регистров, которые нужно прочитать
	int16_t* GetDataRegisters(uint8_t baseRegister, uint8_t registerCount);
};