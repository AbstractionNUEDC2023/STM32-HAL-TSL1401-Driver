# TSL1401 Driver For STM32

Here is a simple implementation of TSL140's driver with HAL.

## Warning

**This driver uses SysTick as time source.And configure SysTick into 1MHz in TSL1401Initialize**

## Features

1. Auto exposure
2. Asynchronous read out TSL1401 (Non-blocking)
3. Burst read and continuous read configurable

## Usage

1. Create a cubemx project, make sure "Generate peripheral initialization as a pair of .c/.h files per peripheral" is enabled
2. Configure peripheral properly and generate code (ADC interrupt must be enabled).
3. Copy TSL1401.h and TSL1401.c into your project
4. Call __Callback_ADC_TSL1401ServiceFunction in HAL_ADC_ConvCpltCallback (Important !!!)

## Debug Tool

LinearCCD_Debug.py provides TSL1401's frame buffer visualization and median、average analyze. To use this script, please transmit a json string like this via UART:

```json
{"Pixel": [FrameBuffer]}
```

And modify here:

```python
com = serial.Serial('/dev/ttyACM0', 115200, timeout=50)
```

## Function Reference Manual

### __Callback_ADC_TSL1401_ServiceFunction

#### Description

Service function for TSL1401 driver, which should be called from HAL_ADC_ConvCpltCallback.Make sure this
function is called after the adc connected to TSL1401's AO convert complete.

### TSL1401AsyncReadCpltCallback

#### Description

Callback function which will be called after TSL1401 read out has been done.Should be written by user.

#### Parameters

| Parameter   | Type      | Description                                                               |
|-------------|-----------|---------------------------------------------------------------------------|
| FrameBuffer | uint16_t* | Frame buffer of TSL1401 drivr, uint16_t[128], DO NOT MODIFY IN YOUR CODE. |

### TSL1401Initialize

### Description

Initializer of driver.  
**After this be called, SysTick frequency will be 1MHz instead of 1kHz**

### TSL1401AsyncReadServiceFunction

#### Description

Service function for TSL1401 driver.Should be CONSTANTLY called from infinite loop on main function.

### TSL1401BurstReadTrig

#### Description

When driver is configured to discontinuous read mode, call this to trig an exposure and read out operation.

### TSL1401EnableAutoExposure

#### Description

Enable auto exposure feature of the TSL1401 driver, which will limit average brightness into 1000~3500 
by control exposure time if possiable.

### TSL1401DisableAutoExposure

#### Description

Disable auto exposure feature of the TSL1401 driver.

### TSL1401EnableContinuousExposure

#### Description

Enable continuous exposure.

### TSL1401DisableContinuousExposure

#### Description

Disable continuous exposure.

### TSL1401UpdateExposureTime

#### Description

Update exposure time in manual exposure mode.

### Parameters

| Parameter       | Type     | Description                   |
|-----------------|----------|-------------------------------|
| ExposureTime_uS | uint32_t | Exposure time in microseconds |