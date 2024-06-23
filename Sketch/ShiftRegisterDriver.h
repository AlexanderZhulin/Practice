#pragma once
#include <Arduino.h>

// ������� ���������� ��������
class ShiftRegisterDriver
{
private:
	uint8_t ClockPin;
	uint8_t DataPin;

public:
#pragma pack(1)
	// ���������, ����������� ��������� ������� ���������� ��������
	struct DataOutputState
	{
		uint8_t Q0 : 1;
		uint8_t Q1 : 1;
		uint8_t Q2 : 1;
		uint8_t Q3 : 1;
		uint8_t Q4 : 1;
		uint8_t Q5 : 1;
		uint8_t Q6 : 1;
		uint8_t Q7 : 1;
	};
#pragma pack()

	// �����������
	// [in] clockPin - ��� ������������ ����
	// [in] dataPin - ��� ��� �������� ������ �� ����
	ShiftRegisterDriver(uint8_t clockPin, uint8_t dataPin);
	// ������������� ��������� ������� ���������� ��������
	// [in] dataOutputState - ��������� �������
	void SetDataOutputState(DataOutputState dataOutputState);
private:
	// ������� ��������� DataOutputState � uint8_t
	// [in] dataOutputState - ��������� �������
	uint8_t to_uint8_t(DataOutputState dataOutputState);
};