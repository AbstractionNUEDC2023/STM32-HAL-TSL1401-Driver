/**
 * @file TSL1401.h
 * @author Jinyi (RobertBernard.c01@outlook.com)
 * @brief TSL1401 Driver Library Header
 * @details Function Prototypes For TSL1401 Driver Library
 * @version 0.1
 * @date 2023-05-09
 * 
 * @copyright Copyright (c) 2023
 */
#ifndef __TSL1401_DRIVER_HEADER__
#define __TSL1401_DRIVER_HEADER__

#include <adc.h>
#include <gpio.h>

#ifndef TSL1401_ADC_HANDLE
    #define TSL1401_ADC_HANDLE              hadc1
#endif
#ifndef TSL_CLK_GPIO_Port
    #define TSL_CLK_GPIO_Port               GPIOA
#endif
#ifndef TSL_CLK_Pin
    #define TSL_CLK_Pin                     GPIO_PIN_1
#endif
#ifndef TSL_SI_GPIO_Port
    #define TSL_SI_GPIO_Port                GPIOA
#endif
#ifndef TSL_SI_Pin
    #define TSL_SI_Pin                      GPIO_PIN_4
#endif

void __Callback_ADC_TSL1401ServiceFunction(void);

void TSL1401AsyncReadCpltCallback(uint16_t* FrameBuffer);

void TSL1401Initialize(void);
void TSL1401AsyncReadServiceFunction(void);

void TSL1401BurstReadTrig(void);
void TSL1401EnableAutoExposure(void);
void TSL1401DisableAutoExposure(void);
void TSL1401EnableContinuousExposure(void);
void TSL1401DisableContinuousExposure(void);
void TSL1401UpdateExposureTime(uint32_t ExposureTime_uS);
#endif