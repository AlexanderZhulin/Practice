#pragma once
#include "ShiftRegisterDriver.h"

#define CHANNEL_COUNT_MAX 8

// ����� ���������� ������� �����������
class LEDsManager
{
public:
	// �������� ��������� ������
	static LEDsManager* GetInstance();
	// ������������ ���������
	enum LedState
	{
		Off,
		On,
		Blink
	};
	// ������������� ������
	// [in] *pShiftRegister - ��������� �� ������� ���������� ��������
	// [in] period - ������ ��������
	void Init(ShiftRegisterDriver* pShiftRegister, uint16_t period);
	// ������������ ����� (�������-����������)
	// [in] segmentIndex - ������ ��������
	// [in] leds - ��������� ������� ���������� �������� ��� ��������
	void RegisterChannel(uint8_t segmentIndex, ShiftRegisterDriver::DataOutputState leds);
	// ���������� ��������� ������
	// [in] segmentIndex - ������ ��������
	// [in] state - ��������� ������
	void SetChannelState(uint8_t segmentIndex, LedState state);
	// ����� ���������
	void ResetState();
private:
	// �����������
	LEDsManager();
	// ���� �������� �������������
	bool isInit;
	// ��������� �� ��������� �������
	static inline ShiftRegisterDriver *ptrShiftRegister;
	// ������ ��������
	static inline uint16_t periodBlink;
	// ������� �������
	static inline SemaphoreHandle_t blinkSemaphore;
	// ����� �������
	static void TaskBlink(void* pvParameters);
	// ������� �������
	uint8_t registerCount;
	// ������� ��������� ���������
	static inline ShiftRegisterDriver::DataOutputState currentState;
	// ���������, ����������� �����
	struct Channel
	{
		uint8_t segmentIndex;
		ShiftRegisterDriver::DataOutputState leds;
		LedState state;
	};
	// ������ �������
	static inline Channel channels[CHANNEL_COUNT_MAX];
	// ���������� ��������� ��������
	static void SetRegisterState(Channel channel);
	// ���������� � On
	static void SetOnLeds(ShiftRegisterDriver::DataOutputState channelLeds);
	// ���������� � Off
	static void SetOffLeds(ShiftRegisterDriver::DataOutputState channelLeds);
	// ������������� ���������
	static inline bool isOn;
	// ���������� � Blink
	static void SetBlinkLeds();

	static inline void printCurrentState();
	static inline void printChannels();
};