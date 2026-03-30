/*
 * main2.c
 *
 *  Created on: Mar 10, 2026
 *      Author: admin
 */
#include <main.h>
#include <main2.h>
#include <trigger.h>
#include <ignition.h>
#include <uart.h>
#include "stdio.h"
#include <string.h>
#include <stdlib.h>




/////////////////////SETUP////////////////////////////
void setup(){
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
	HAL_UART_Receive_IT(&huart1, &rx_char, 1);

}
//////////////////////////////////////////////////////
/////////////////////LOOP/////////////////////////////
void loop(){
	if(HAL_GetTick() - last_uart_time >= 500)
	  {

		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
	    last_uart_time = HAL_GetTick();

	    //int len = sprintf(uart_buffer, "Synced: %u RPM: %lu\r\n",synced, rpm);
	    int len = sprintf(uart_buffer, "angle: %u\r\n", ignition_target_angle_cyl1);
	    HAL_UART_Transmit(&huart1,
	                      (uint8_t*)uart_buffer,
	                      len,
	                      HAL_MAX_DELAY);
	  }
//	ignition_angle_set(300);
//		  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
//		     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
//		     HAL_Delay(500);
//
//		     // LED na PA6 OFF, PA7 ON
//		     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
//		     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
//		     HAL_Delay(500);

}


