#include "StatesManager.h"

StatesManager* StatesManager::GetInstance()
{
	static StatesManager statesManager;
	return &statesManager;
}

StatesManager::StatesManager() : isInit(false) {}

void StatesManager::Init(uint16_t survayPeriod_)
{
	if (isInit) return;

	survayPeriod = survayPeriod_;
	isInit = true;
	checkStateSemaphore = xSemaphoreCreateBinary();

	xTaskCreate(
		TaskDeviceStateTransition,
		"DeviceStateTransition",
		2048,
		NULL,
		3,
		NULL
	);
}


void StatesManager::TaskDeviceStateTransition(void* pvParameters)
{
	(void)pvParameters;

	for (;;)
	{
		xSemaphoreTake(checkStateSemaphore, (survayPeriod / portTICK_PERIOD_MS));
	}
}

StatesManager::States StatesManager::GetCurrentState(float resultingAcceration)
{
	if (!isInit) return States::workingState;
	
	// Надо ли запомнить полученный resultingAcceleration ?
	// Если - да, ждем survayPeriod мс
	// Если - нет, возвращаем текущее состояние

	
	xSemaphoreGive(checkStateSemaphore);
	return States::workingState;
}