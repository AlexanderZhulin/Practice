#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "LEDsManager.h"
#include "ShiftRegisterDriver.h"
#include "AccelerometerDriver.h"
#include "SegmentIdentifier.h"
#include "StatesManager.h"

#define MPU_ADDRESS 0x68       // Адрес датчика
#define CLOCK_PIN   18         // Пин синхронизации
#define DATA_PIN    23         // Пин передачи данных

#define STATE_TRANSITION_WAITING_TIME   3000                      // Время ожидания перехода состояний, мс
#define PERIOD_BLINK                    500                       // Частота моргания, мс
#define ACCELEROMETER_SURVAY_PERIOD     10 / portTICK_PERIOD_MS   // Частота опроса акселерометра, мс
#define LEDS_UPDADTE_PERIOD             100 / portTICK_PERIOD_MS  // Частота обновления светодиодов, мс

const ShiftRegisterDriver::DataOutputState LEFT = { 0, 0, 0, 0, 1, 0, 0, 0 };   // Левый светодиод
const ShiftRegisterDriver::DataOutputState RIGHT = { 1, 0, 0, 0, 0, 0, 0, 0 };  // Правый светодиод
const ShiftRegisterDriver::DataOutputState TOP = { 0, 1, 0, 0, 0, 0, 0, 0 };    // Верхний светодиод
const ShiftRegisterDriver::DataOutputState BUTTOM = { 0, 0, 1, 0, 0, 0, 0, 0 }; // Нижний светодиод
const ShiftRegisterDriver::DataOutputState CENTER = { 0, 0, 0, 1, 0, 0, 0, 0 }; // Центральый светодиод

ShiftRegisterDriver* ptrShiftRegister = nullptr;
AccelerometerDriver* ptrAccelerometer = nullptr;

enum Segments
{
    Top,
    Right,
    Buttom,
    Left,
    Center
};

const uint8_t SEGMENT_COUNT = 5;  // Число сегментов
SegmentIdentifier<SEGMENT_COUNT>* ptrSegmentIdentifier = nullptr;
SegmentIdentifier<SEGMENT_COUNT>::Segment segments[SEGMENT_COUNT] = {
    // Сегменты
    {// TOP: сегмент 0
        { -5.0, -30.0, -180.0 },
        { -90.0, 30.0, 180.0 },
        1.0
    },
    {// RIGHT: сегмент 1
        { 30.0, -5.0, -180.0 },
        { -30.0, -90.0, 180.0 },
        1.0
    },
    {// BUTTOM: сегмент 2
        { 5.0, 30.0, -180.0 },
        { 90.0, -30.0, 180.0 },
        1.0
    },
    {// LEFT: сегмент 3
        { -30.0, 5.0, -180.0 },
        { 30.0, 90.0, 180.0 },
        2.0
    },
    {// CENTER: сегмент 4
        { 5.0, -5.0, -180.0 },
        { -5.0, 5.0, 180.0 },
        1.0
    }
};

float resultingAcceleration{};
AccelerometerDriver::Position accelerometerPosition{};

StatesManager::States stateDevice;

SemaphoreHandle_t printMutex = nullptr;

// Поток опроса акселерометра
void TaskSurveyAccelerometer(void* pvParameters);
// Поток управления светодиодами
void TaskManageLeds(void* pvParameters);

void TaskTestStates(void* pvParameters);

// Отладочные функции вывода показаний в порт
void printPositionAccelerometer();
void printSegment();
void printResultingAcceleration();

void setup()
{
    printMutex = xSemaphoreCreateMutex();

    static AccelerometerDriver accelerometer(MPU_ADDRESS);
    ptrAccelerometer = &accelerometer;

    static ShiftRegisterDriver shiftRegister(CLOCK_PIN, DATA_PIN);
    ptrShiftRegister = &shiftRegister;

    static SegmentIdentifier<SEGMENT_COUNT> segmentIdentifier(segments);
    ptrSegmentIdentifier = &segmentIdentifier;

    Serial.begin(115200);
    while (!Serial);

    LEDsManager::GetInstance()->Init(ptrShiftRegister, PERIOD_BLINK);
    //StatesManager::GetInstance()->Init(STATE_TRANSITION_WAITING_TIME);

    stateDevice = StatesManager::States::workingState;

    xTaskCreate(
        TaskSurveyAccelerometer,
        "SurveyAccelerometer",
        2048,
        NULL,
        5,
        NULL
    );

    xTaskCreate(
        TaskManageLeds,
        "ManageLeds",
        2048,
        NULL,
        1,
        NULL
    );
        
    xTaskCreate(
        TaskTestStates,
        "TestStates",
        2048,
        NULL,
        1,
        NULL
    );
}

void loop() {}

void printPositionAccelerometer()
{
    if (xSemaphoreTake(printMutex, portMAX_DELAY))
    {
        Serial.print("Positions:\n\t");
        Serial.print(ptrAccelerometer->GetCurriosPosition().xAngle);
        Serial.print("\t");
        Serial.print(ptrAccelerometer->GetCurriosPosition().yAngle);
        Serial.print("\t");
        Serial.print(ptrAccelerometer->GetCurriosPosition().zAngle);
        Serial.print("\n");
        Serial.println("End");

        xSemaphoreGive(printMutex);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void printSegment()
{
    if (xSemaphoreTake(printMutex, portMAX_DELAY))
    {
        Serial.println("-------------------");
        Serial.print("Current Segment: ");
        if (ptrSegmentIdentifier->CurrentSegment() != UINT8_MAX)
            Serial.print(ptrSegmentIdentifier->CurrentSegment());
        else
            Serial.print("-");
        Serial.println(" |");
        Serial.println("-------------------");

        xSemaphoreGive(printMutex);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void printResultingAcceleration()
{
    if (xSemaphoreTake(printMutex, portMAX_DELAY))
    {
        Serial.print("Resulting Accelerations:\n\t");
        Serial.println(ptrAccelerometer->GetResultingAcceleration());
        Serial.println("End");

        xSemaphoreGive(printMutex);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);
}

void TaskTestStates(void* pvParameters)
{
    (void)pvParameters;

    uint16_t count = 0;
    float resultAccelTransition;
    bool isInRange;
    const uint8_t surveyPeriod = 100;
    const uint8_t hyst = 0.3;

    for (;;)
    {
        if (count == 0)
            resultAccelTransition = resultingAcceleration;

        vTaskDelay(surveyPeriod / portTICK_PERIOD_MS);
        count += surveyPeriod;
        isInRange = (resultingAcceleration <= resultAccelTransition + hyst)
            && (resultAccelTransition - hyst <= resultingAcceleration);

        if (count == STATE_TRANSITION_WAITING_TIME)
        {
            if (isInRange)
                stateDevice = StatesManager::States::waitingState;
            else
                stateDevice = StatesManager::States::workingState;
            count = 0;
        }
        else
        {
            if (!isInRange)
                stateDevice = StatesManager::States::workingState;
        }
    }
}

/* 
* Поток обновляет показания акселерометра:
* Положение акселерометра по трем осям (в градусах)
* Результирующее ускроение (м/с^2)
*/
void TaskSurveyAccelerometer(void* pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        accelerometerPosition = ptrAccelerometer->GetAcclelerometerPosition();
        resultingAcceleration = ptrAccelerometer->GetResultingAcceleration();
        vTaskDelay(ACCELEROMETER_SURVAY_PERIOD);
    }
}

/* 
* Поток определяет сегмент по положению,
* обновляет состояния светодиодов
*/
void TaskManageLeds(void* pvParameters)
{
    (void)pvParameters;

    // Регистрация каналов сегмент-светодиоды
    LEDsManager::GetInstance()->RegisterChannel(Segments::Top, TOP);
    LEDsManager::GetInstance()->RegisterChannel(Segments::Right, RIGHT);
    LEDsManager::GetInstance()->RegisterChannel(Segments::Buttom, BUTTOM);
    LEDsManager::GetInstance()->RegisterChannel(Segments::Left, LEFT);
    LEDsManager::GetInstance()->RegisterChannel(Segments::Center, CENTER);

    uint8_t currentSegment{};
    
    for (;;)
    {
        LEDsManager::GetInstance()->ResetState();
        currentSegment = ptrSegmentIdentifier->GetCurrentSegment(accelerometerPosition);

        //stateDevice = StatesManager::GetInstance()->GetCurrentState(resultingAcceleration, stateDevice);
        switch (stateDevice)
        {
        case StatesManager::States::workingState:
            LEDsManager::GetInstance()->SetChannelState(currentSegment, LEDsManager::LedState::On);
            break;
        case StatesManager::States::waitingState:
            if (currentSegment == Segments::Center)
                LEDsManager::GetInstance()->SetChannelState(currentSegment, LEDsManager::LedState::Off);
            else
                LEDsManager::GetInstance()->SetChannelState(currentSegment, LEDsManager::LedState::Blink);
            break;
        default:
            break;
        }
        vTaskDelay(LEDS_UPDADTE_PERIOD);
    }
}