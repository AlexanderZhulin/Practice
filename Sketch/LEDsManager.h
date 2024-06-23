#pragma once
#include "ShiftRegisterDriver.h"

#define CHANNEL_COUNT_MAX 8

// Класс управления работой светодиодов
class LEDsManager
{
public:
	// Получить экземпляр класса
	static LEDsManager* GetInstance();
	// Перечисление состояний
	enum LedState
	{
		Off,
		On,
		Blink
	};
	// Инициализация потока
	// [in] *pShiftRegister - указатель на драйвер сдвигового регистра
	// [in] period - период моргания
	void Init(ShiftRegisterDriver* pShiftRegister, uint16_t period);
	// Регистрирует канал (сегмент-светодиоды)
	// [in] segmentIndex - индекс сегмента
	// [in] leds - состояние выводов сдвигового регистра для сегмента
	void RegisterChannel(uint8_t segmentIndex, ShiftRegisterDriver::DataOutputState leds);
	// Установить состояние канала
	// [in] segmentIndex - индекс сегмента
	// [in] state - состояние канала
	void SetChannelState(uint8_t segmentIndex, LedState state);
	// Сброс состояния
	void ResetState();
private:
	// Конструктор
	LEDsManager();
	// Флаг проверки инициализации
	bool isInit;
	// Указатель на сдвиговый регистр
	static inline ShiftRegisterDriver *ptrShiftRegister;
	// Период моргания
	static inline uint16_t periodBlink;
	// Семафор мигания
	static inline SemaphoreHandle_t blinkSemaphore;
	// Поток мигания
	static void TaskBlink(void* pvParameters);
	// Счетчик каналов
	uint8_t registerCount;
	// Текущее суммарное состояние
	static inline ShiftRegisterDriver::DataOutputState currentState;
	// Структура, описывающая канал
	struct Channel
	{
		uint8_t segmentIndex;
		ShiftRegisterDriver::DataOutputState leds;
		LedState state;
	};
	// Массив каналов
	static inline Channel channels[CHANNEL_COUNT_MAX];
	// Устанавить состояние регистра
	static void SetRegisterState(Channel channel);
	// Установить в On
	static void SetOnLeds(ShiftRegisterDriver::DataOutputState channelLeds);
	// Установить в Off
	static void SetOffLeds(ShiftRegisterDriver::DataOutputState channelLeds);
	// Переключатель состояний
	static inline bool isOn;
	// Установить в Blink
	static void SetBlinkLeds();

	static inline void printCurrentState();
	static inline void printChannels();
};