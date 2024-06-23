#pragma once
#include <Arduino.h>

// �����, ���������� �� ������� ����� �����������
class StatesManager
{
public:
	// �������� ��������� ������
	static StatesManager* GetInstance();
	// ������������� ������
	void Init(uint16_t survayPeriod_);
	// ��������� ����������
	enum States
	{
		workingState,
		waitingState
	};
	// �������� ������� ���������
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