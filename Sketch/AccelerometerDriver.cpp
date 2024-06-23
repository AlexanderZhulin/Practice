#include "AccelerometerDriver.h"
#include <Wire.h>

AccelerometerDriver::AccelerometerDriver(uint16_t address) : Address(address), position({})
{
    Wire.begin();
    Wire.beginTransmission(Address);
    Wire.write(PWR_MGMT_1);  // PWR_MGMT_1 register
    Wire.write(0);           // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
}

int16_t* AccelerometerDriver::GetDataRegisters(uint8_t baseRegister, uint8_t registerCount)
{
    static int16_t data[REGISTER_COUNT] = {};

    Wire.beginTransmission(Address);
    Wire.write(baseRegister);
    Wire.endTransmission(false);
    Wire.requestFrom(Address, registerCount * 2); // request a total of 3 registers

    for (byte i = 0; i < registerCount; i++)
        data[i] = Wire.read() << 8 | Wire.read();

    return data;
}

AccelerometerDriver::Position AccelerometerDriver::GetAcclelerometerPosition()
{
    float angles[REGISTER_COUNT] = {};
    int16_t* ptrData = GetDataRegisters(ACCEL_XOUT_H, REGISTER_COUNT);

    for (byte i = 0; i < REGISTER_COUNT; i++)
        angles[i] = *(ptrData + i) / SCALE_FACTOR * GRAVITATIONAL_ACCELERATION;

    position = {
        static_cast<float>(atan2(angles[1], angles[2]) * 180 / M_PI),
        static_cast<float>(atan2(angles[0], angles[2]) * 180 / M_PI),
        static_cast<float>(atan2(angles[0], angles[1]) * 180 / M_PI)
    };

    return position;
}

float AccelerometerDriver::GetResultingAcceleration()
{
    int16_t* ptrData = GetDataRegisters(ACCEL_XOUT_H, REGISTER_COUNT);
    float resultingAcceleration = 0.0;
    
    for (byte i = 0; i < REGISTER_COUNT; i++)
        resultingAcceleration += *(ptrData + i) / SCALE_FACTOR * GRAVITATIONAL_ACCELERATION;

    return abs(resultingAcceleration) - GRAVITATIONAL_ACCELERATION;
}

uint16_t AccelerometerDriver::GetAddress()
{
    return Address;
}

AccelerometerDriver::Position AccelerometerDriver::GetCurriosPosition()
{
    return position;
}