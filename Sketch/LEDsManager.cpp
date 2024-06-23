#include "LEDsManager.h"

LEDsManager* LEDsManager::GetInstance()
{
	static LEDsManager lm;
	return &lm;
}

LEDsManager::LEDsManager() : isInit(false), registerCount(0)
{
	isOn = false;
	currentState = {};
}

void LEDsManager::Init(ShiftRegisterDriver* pShiftRegister, uint16_t period)
{
	if (isInit)
		return;

	isInit = true;
	periodBlink = period;
	blinkSemaphore = xSemaphoreCreateBinary();

	ptrShiftRegister = pShiftRegister;

	xTaskCreate(
		TaskBlink,
		"Blink",
		2048,
		NULL,
		5,
		NULL
	);
}

void LEDsManager::TaskBlink(void* pvParameters)
{
	(void)pvParameters;

	for (;;)
	{
		xSemaphoreTake(blinkSemaphore, (periodBlink / portTICK_PERIOD_MS));
		isOn = !isOn;
		SetBlinkLeds();
		ptrShiftRegister->SetDataOutputState(currentState);
	}
}

void LEDsManager::printChannels()
{
	Serial.println("Segment\tLeds\t\tState");
	for (auto channel : channels)
	{
		Serial.print(channel.segmentIndex);
		Serial.print("\t");
		Serial.print(channel.leds.Q0);
		Serial.print(channel.leds.Q1);
		Serial.print(channel.leds.Q2);
		Serial.print(channel.leds.Q3);
		Serial.print(channel.leds.Q4);
		Serial.print(channel.leds.Q5);
		Serial.print(channel.leds.Q6);
		Serial.print(channel.leds.Q7);
		Serial.print("\t");
		Serial.println(channel.state);
	}
}

void LEDsManager::printCurrentState()
{
	Serial.print("isOn: ");
	Serial.println(isOn);
	Serial.print("state: ");
	Serial.print("0b");
	Serial.print(currentState.Q0);
	Serial.print(currentState.Q1);
	Serial.print(currentState.Q2);
	Serial.print(currentState.Q3);
	Serial.print(currentState.Q4);
	Serial.print(currentState.Q5);
	Serial.print(currentState.Q6);
	Serial.println(currentState.Q7);
}

void LEDsManager::RegisterChannel(uint8_t segmentIndex, ShiftRegisterDriver::DataOutputState leds)
{
	if (!isInit)
		return;

	if (registerCount <= CHANNEL_COUNT_MAX)
	{
		channels[registerCount].leds = leds;
		channels[registerCount].state = LedState::Off;
		channels[registerCount].segmentIndex = segmentIndex;
		registerCount++;
	}
}

void LEDsManager::SetChannelState(uint8_t segmentIndex, LedState state)
{
	if (!isInit)
		return;

	for (auto &channel : channels)
	{
		if (channel.segmentIndex == segmentIndex)
		{
			channel.state = state;
			SetRegisterState(channel);
			xSemaphoreGive(blinkSemaphore);
			break;
		}
	}
}

void LEDsManager::ResetState()
{
	for (auto& channel : channels)
		channel.state = LedState::Off;
	currentState = {};
	ptrShiftRegister->SetDataOutputState(currentState);
}

void LEDsManager::SetRegisterState(Channel channel)
{
	switch (channel.state)
	{
	case LedState::Off:
		SetOffLeds(channel.leds);
		break;
	case LedState::On:
		SetOnLeds(channel.leds);
		break;
	default:
		break;
	}
}

void LEDsManager::SetBlinkLeds()
{
	for (auto channel : channels)
	{
		if (channel.state == LedState::Blink)
		{
			if (isOn)
				SetOnLeds(channel.leds);
			else
				SetOffLeds(channel.leds);
		}
	}
}

void LEDsManager::SetOnLeds(ShiftRegisterDriver::DataOutputState channelLeds)
{
	currentState.Q0 |= (channelLeds.Q0 == 1) ? 1 : 0;
	currentState.Q1 |= (channelLeds.Q1 == 1) ? 1 : 0;
	currentState.Q2 |= (channelLeds.Q2 == 1) ? 1 : 0;
	currentState.Q3 |= (channelLeds.Q3 == 1) ? 1 : 0;
	currentState.Q4 |= (channelLeds.Q4 == 1) ? 1 : 0;
	currentState.Q5 |= (channelLeds.Q5 == 1) ? 1 : 0;
	currentState.Q6 |= (channelLeds.Q6 == 1) ? 1 : 0;
	currentState.Q7 |= (channelLeds.Q7 == 1) ? 1 : 0;
}

void LEDsManager::SetOffLeds(ShiftRegisterDriver::DataOutputState channelLeds)
{
	currentState.Q0 &= (channelLeds.Q0 == 1) ? 0 : 1;
	currentState.Q1 &= (channelLeds.Q1 == 1) ? 0 : 1;
	currentState.Q2 &= (channelLeds.Q2 == 1) ? 0 : 1;
	currentState.Q3 &= (channelLeds.Q3 == 1) ? 0 : 1;
	currentState.Q4 &= (channelLeds.Q4 == 1) ? 0 : 1;
	currentState.Q5 &= (channelLeds.Q5 == 1) ? 0 : 1;
	currentState.Q6 &= (channelLeds.Q6 == 1) ? 0 : 1;
	currentState.Q7 &= (channelLeds.Q7 == 1) ? 0 : 1;
}