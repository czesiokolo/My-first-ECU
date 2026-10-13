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
#include <can.h>
#include <uart.h>
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <engine_info.h>



volatile uint32_t counter = 0;

/////////////////////SETUP////////////////////////////
void setup(){
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
	__HAL_TIM_DISABLE_IT(&htim5, TIM_IT_CC1);
	HAL_UART_Receive_IT(&huart1, &rx_char, 1);
	HAL_TIM_OC_Start_IT(&htim5, TIM_CHANNEL_1);

}
//////////////////////////////////////////////////////
/////////////////////LOOP/////////////////////////////
void loop(){
	counter++;
	if(HAL_GetTick() - last_uart_time >= 100)
	  {

		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
	    last_uart_time = HAL_GetTick();

	    //int len = sprintf(uart_buffer, "Synced: %u RPM: %lu\r\n",synced, rpm);
//	    int len = sprintf(uart_buffer, "counter: %lu\r\n", counter);
//	    HAL_UART_Transmit(&huart1,
//	                      (uint8_t*)uart_buffer,
//	                      len,
//	                      HAL_MAX_DELAY);
	    counter = 0;
	    can_update();
	  }
	engine_update();
	ignition_update();



}


