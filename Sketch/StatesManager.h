#pragma once
#include <Arduino.h>

// Класс, отвечающий за переход между состояниями
class StatesManager
{
public:
	// Получить экземпляр класса
	static StatesManager* GetInstance();
	// Инициализация потока
	void Init(uint16_t survayPeriod_);
	// Состояния устройства
	enum States
	{
		workingState,
		waitingState
	};
	// Получить текущее состояние
	States GetCurrentState(float resultingAcceleration);
private:
	StatesManager();
	bool isInit;
	States currentState;
	float resultingAccerationTransition;
	static void TaskDeviceStateTransition(void* pvParameters);
	static inline SemaphoreHandle_t checkStateSemaphore;
	static inline uint16_t survayPeriod;
};