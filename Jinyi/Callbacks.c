#include <adc.h>
#include <usart.h>
#include <TSL1401.h>

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if(hadc->Instance == TSL1401_ADC_HANDLE.Instance){
        __Callback_ADC_TSL1401ServiceFunction();
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {

}