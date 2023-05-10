#include <stdio.h>
#include <usart.h>
#include <string.h>
#include <TSL1401.h>

struct __TSL1401VariablePack_t {
    uint8_t     NextBurstReadFlag:1;
    uint8_t     AutoExposureEnable:1;
    uint8_t     AdcDataAvailableFlag:1;
    uint8_t     ContinuousExposureEnable:1;
    uint16_t    HalfClockPeroid_uS;
    uint8_t     GeneratedClockCycleCounter;
    uint32_t    ExposureTime_uS;
    uint32_t    OperationStartTick;
    uint8_t     ExposureAndReadOutState;
    uint16_t    FrameBuffer[128];
    uint32_t    FrameAverageBrightness;
} __TSL1401VariablePack = { 0 };

void __Callback_ADC_TSL1401ServiceFunction(void) {
    __TSL1401VariablePack.AdcDataAvailableFlag = 1;
}

__weak void TSL1401AsyncReadCpltCallback(uint16_t* FrameBuffer) {
    char outputStringBuffer[64];
    sprintf(outputStringBuffer, "{\"Pixel\":[");
    HAL_UART_Transmit(&huart2, (uint8_t*)outputStringBuffer, strlen(outputStringBuffer), HAL_MAX_DELAY);
    for(uint8_t i = 0; i < 127; i++) {
        sprintf(outputStringBuffer, "%u,", FrameBuffer[i]);
        HAL_UART_Transmit(&huart2, (uint8_t*)outputStringBuffer, strlen(outputStringBuffer), HAL_MAX_DELAY);
    }
    sprintf(outputStringBuffer, "%u]}\n", FrameBuffer[127]);
    HAL_UART_Transmit(&huart2, (uint8_t*)outputStringBuffer, strlen(outputStringBuffer), HAL_MAX_DELAY);
}

void TSL1401Initialize(void) {
    __TSL1401VariablePack.NextBurstReadFlag = 0;
    __TSL1401VariablePack.AutoExposureEnable = 1;
    __TSL1401VariablePack.AdcDataAvailableFlag = 0;
    __TSL1401VariablePack.ContinuousExposureEnable = 1;
    __TSL1401VariablePack.ExposureTime_uS = 20000;
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000000);
}
void TSL1401BurstReadTrig(void) {
    if (__TSL1401VariablePack.ContinuousExposureEnable)
        return;
    __TSL1401VariablePack.NextBurstReadFlag = 1;
}
void TSL1401EnableAutoExposure(void) { __TSL1401VariablePack.AutoExposureEnable = 1; }
void TSL1401DisableAutoExposure(void) { __TSL1401VariablePack.AutoExposureEnable = 0; }
void TSL1401EnableContinuousExposure(void) { __TSL1401VariablePack.ContinuousExposureEnable = 1;}
void TSL1401DisableContinuousExposure(void) { __TSL1401VariablePack.ContinuousExposureEnable = 0; }
void TSL1401UpdateExposureTime(uint32_t ExposureTime_uS) {
    __TSL1401VariablePack.ExposureTime_uS = ExposureTime_uS;
    __TSL1401VariablePack.HalfClockPeroid_uS = ExposureTime_uS / 220;
}

void TSL1401AsyncReadServiceFunction(void) {
    switch(__TSL1401VariablePack.ExposureAndReadOutState) {
        case 0:
            HAL_GPIO_WritePin(TSL_CLK_GPIO_Port, TSL_CLK_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(TSL_SI_GPIO_Port, TSL_SI_Pin, GPIO_PIN_RESET);
            __TSL1401VariablePack.OperationStartTick = HAL_GetTick();
            __TSL1401VariablePack.ExposureAndReadOutState = 1;
            break;
        case 1:
            if (__TSL1401VariablePack.OperationStartTick < HAL_GetTick()){
                HAL_GPIO_WritePin(TSL_CLK_GPIO_Port, TSL_CLK_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(TSL_SI_GPIO_Port, TSL_SI_Pin, GPIO_PIN_SET);
                __TSL1401VariablePack.OperationStartTick = HAL_GetTick();
                __TSL1401VariablePack.ExposureAndReadOutState = 2;
            }
            break;
        case 2:
            if (HAL_GetTick() > __TSL1401VariablePack.OperationStartTick) {
                HAL_GPIO_WritePin(TSL_CLK_GPIO_Port, TSL_CLK_Pin, GPIO_PIN_SET);
                HAL_GPIO_WritePin(TSL_SI_GPIO_Port, TSL_SI_Pin, GPIO_PIN_RESET);
                __TSL1401VariablePack.ExposureAndReadOutState = 3;
                __TSL1401VariablePack.GeneratedClockCycleCounter = 0;
            }
            break;
        // Generating SI Signal
        case 3:
            if (__TSL1401VariablePack.GeneratedClockCycleCounter < 128) {
                if (HAL_GetTick() > __TSL1401VariablePack.OperationStartTick + __TSL1401VariablePack.HalfClockPeroid_uS) {
                    HAL_GPIO_WritePin(TSL_CLK_GPIO_Port, TSL_CLK_Pin, GPIO_PIN_RESET);
                    __TSL1401VariablePack.OperationStartTick = HAL_GetTick();
                    __TSL1401VariablePack.AdcDataAvailableFlag = 0;
                    HAL_ADC_Start_IT(&TSL1401_ADC_HANDLE);
                    __TSL1401VariablePack.ExposureAndReadOutState = 4;
                }
            } else {
                __TSL1401VariablePack.ExposureAndReadOutState = 6;
            }
            break;
        case 4:
            if (__TSL1401VariablePack.AdcDataAvailableFlag) {
                __TSL1401VariablePack.FrameBuffer[__TSL1401VariablePack.GeneratedClockCycleCounter] = HAL_ADC_GetValue(&TSL1401_ADC_HANDLE);
                __TSL1401VariablePack.FrameAverageBrightness = __TSL1401VariablePack.FrameAverageBrightness + __TSL1401VariablePack.FrameBuffer[__TSL1401VariablePack.GeneratedClockCycleCounter];
                __TSL1401VariablePack.ExposureAndReadOutState = 5;
            }
            break;
        case 5:
            if (HAL_GetTick() > __TSL1401VariablePack.OperationStartTick + __TSL1401VariablePack.HalfClockPeroid_uS) {
                HAL_GPIO_WritePin(TSL_CLK_GPIO_Port, TSL_CLK_Pin, GPIO_PIN_SET);
                __TSL1401VariablePack.GeneratedClockCycleCounter = __TSL1401VariablePack.GeneratedClockCycleCounter + 1;
                __TSL1401VariablePack.OperationStartTick = HAL_GetTick();
                __TSL1401VariablePack.ExposureAndReadOutState = 3;
            }
            break;
        case 6:
            if (__TSL1401VariablePack.AutoExposureEnable) {
                __TSL1401VariablePack.FrameAverageBrightness = __TSL1401VariablePack.FrameAverageBrightness / 128;
                if (__TSL1401VariablePack.FrameAverageBrightness < 1000 && __TSL1401VariablePack.ExposureTime_uS < 100000) {
                    TSL1401UpdateExposureTime(__TSL1401VariablePack.ExposureTime_uS + 500);
                    __TSL1401VariablePack.ExposureAndReadOutState = 0;
                } else if (__TSL1401VariablePack.FrameAverageBrightness > 3500 && __TSL1401VariablePack.ExposureTime_uS > 5000) {
                    TSL1401UpdateExposureTime(__TSL1401VariablePack.ExposureTime_uS - 500);
                    __TSL1401VariablePack.ExposureAndReadOutState = 0;
                } else {
                        TSL1401AsyncReadCpltCallback(__TSL1401VariablePack.FrameBuffer);
                        if (__TSL1401VariablePack.ContinuousExposureEnable)
                            __TSL1401VariablePack.ExposureAndReadOutState = 0;
                        else
                            __TSL1401VariablePack.ExposureAndReadOutState = 0xFF;
                }
            } else {
                TSL1401AsyncReadCpltCallback(__TSL1401VariablePack.FrameBuffer);
                if (__TSL1401VariablePack.ContinuousExposureEnable)
                    __TSL1401VariablePack.ExposureAndReadOutState = 0;
                else
                    __TSL1401VariablePack.ExposureAndReadOutState = 0xFF;
            }
            break;
        default:
            __TSL1401VariablePack.ExposureAndReadOutState = __TSL1401VariablePack.NextBurstReadFlag ? 0 : 0xFF;
            __TSL1401VariablePack.NextBurstReadFlag = __TSL1401VariablePack.NextBurstReadFlag ? 0 : __TSL1401VariablePack.NextBurstReadFlag;
            break;
    }
}
