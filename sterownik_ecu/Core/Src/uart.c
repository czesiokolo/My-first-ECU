/*
 * uart.c
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */
#include <uart.h>
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <ignition.h>
char uart_buffer[64];
uint32_t last_uart_time = 0;

uint8_t rx_char;
char buffer[5];   // "-123" + '\0'
uint8_t idx = 0;
uint8_t is_negative = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        if (rx_char == '-' && idx == 0)
        {
            is_negative = 1;
        }
        else if (rx_char >= '0' && rx_char <= '9')
        {
            if (idx < 4)  // miejsce na max 3 cyfry + opcjonalny znak
            {
                buffer[idx++] = rx_char;
            }
        }
        else if (rx_char == '\n')
        {
            buffer[idx] = '\0';

            int value = atoi(buffer);

            if (is_negative)
            {
                value = -value;
            }

            ignition_angle_set(value);

            // reset stanu
            idx = 0;
            is_negative = 0;
        }

        HAL_UART_Receive_IT(&huart1, &rx_char, 1);
    }
}
