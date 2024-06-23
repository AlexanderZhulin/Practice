#include "ShiftRegisterDriver.h"

ShiftRegisterDriver::ShiftRegisterDriver(uint8_t clockPin, uint8_t dataPin): ClockPin(clockPin), DataPin(dataPin)
{
	pinMode(ClockPin, OUTPUT);
	pinMode(DataPin, OUTPUT);
}

uint8_t ShiftRegisterDriver::to_uint8_t(ShiftRegisterDriver::DataOutputState dataOutputState)
{
	uint8_t result = 0;
	result |= dataOutputState.Q0;
	result |= dataOutputState.Q1 << 1;
	result |= dataOutputState.Q2 << 2;
	result |= dataOutputState.Q3 << 3;
	result |= dataOutputState.Q4 << 4;
	result |= dataOutputState.Q5 << 5;
	result |= dataOutputState.Q6 << 6;
	result |= dataOutputState.Q7 << 7;

	return result;
}

void ShiftRegisterDriver::SetDataOutputState(ShiftRegisterDriver::DataOutputState dataOutputState)
{
	uint8_t state = to_uint8_t(dataOutputState);
	bool bit = 0;
	for (int8_t i = 7; i >= 0; i--)
	{
		digitalWrite(ClockPin, LOW);
		bit = ( state & (1 << i) ) > 0 ? HIGH : LOW;
		digitalWrite(DataPin, bit);
		digitalWrite(ClockPin, HIGH);
	}
}