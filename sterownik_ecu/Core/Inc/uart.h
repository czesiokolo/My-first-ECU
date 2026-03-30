/*
 * uart.h
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */

#ifndef INC_UART_H_
#define INC_UART_H_
#include "stm32f4xx_hal.h"
//////////////////////////////////////////////////////////
/////////////////UART VARIABLES //////////////////////////
extern UART_HandleTypeDef huart1;

extern char uart_buffer[64];
extern uint32_t last_uart_time;

extern uint8_t rx_char;
extern char buffer[5];   // "-123" + '\0'
extern uint8_t idx;
extern uint8_t is_negative;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_UART_H_ */


