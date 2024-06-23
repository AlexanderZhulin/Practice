#pragma once
#include <Arduino.h>

// ƒрайвер сдвигового регистра
class ShiftRegisterDriver
{
private:
	uint8_t ClockPin;
	uint8_t DataPin;

public:
#pragma pack(1)
	// —труктура, описывающа€ состо€ние выводов сдвигового регистра
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

	//  онструктор
	// [in] clockPin - пин тактировани€ шины
	// [in] dataPin - пин дл€ передачи данных на шину
	ShiftRegisterDriver(uint8_t clockPin, uint8_t dataPin);
	// ”станавливает состо€ние выводов сдвигового регистра
	// [in] dataOutputState - состо€ние выводов
	void SetDataOutputState(DataOutputState dataOutputState);
private:
	// ѕеревод структуры DataOutputState в uint8_t
	// [in] dataOutputState - состо€ние выводов
	uint8_t to_uint8_t(DataOutputState dataOutputState);
};