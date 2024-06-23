#pragma once
#include <Arduino.h>

#define SCALE_FACTOR				16384.0
#define GRAVITATIONAL_ACCELERATION	9.806
#define REGISTER_COUNT				3
#define PWR_MGMT_1					0x6B
#define ACCEL_XOUT_H				0x3B // ��������� ������� 0x3B (ACCEL_XOUT_H)

// ������� �������������
class AccelerometerDriver
{
public:
	// ���������, ����������� ��������� ������������� � ������������
	struct Position
	{
		float xAngle; // ������ �� ��� X, �������
		float yAngle; // ������ �� ��� Y, �������
		float zAngle; // ������ �� ��� Z, �������
	};
	// �����������
	// [in] accelerometerAddress - ����� ������������� �� ���� I2C
	AccelerometerDriver(uint16_t address);

	// ���������� ��������� ��������� ������������� � ���������� ���������
	Position GetAcclelerometerPosition();
	// �������� ����� �������
	uint16_t GetAddress();
	// �������� ������� �������
	Position GetCurriosPosition();
	// �������� ������� �������������� ���������
	float GetResultingAcceleration();

private:
	// ����� �������
	uint16_t Address;
	// ������� ������� �������
	Position position;
	// �������� ������ � ��������� �������������
	// [in] baseRegister - ����� �������� �������� �������������
	// [in] registerCount - ����� ���������, ������� ����� ���������
	int16_t* GetDataRegisters(uint8_t baseRegister, uint8_t registerCount);
};