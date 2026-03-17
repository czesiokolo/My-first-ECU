/*
 * main2.c
 *
 *  Created on: Mar 10, 2026
 *      Author: admin
 */
#include <main.h>
#include <main2.h>
#include "stdio.h"
#include <string.h>
#include <stdlib.h>
//////////////////////////////////////////////////////////
/////////////////UART VARIABLES //////////////////////////
extern UART_HandleTypeDef huart1;

char uart_buffer[64];
uint32_t last_uart_time = 0;

uint8_t rx_char;
char buffer[5];   // "-123" + '\0'
uint8_t idx = 0;
uint8_t is_negative = 0;

//////////////////////////////////////////////////////////
/////////////////CRANKSHAFT SENSOR VARIABLES /////////////
extern TIM_HandleTypeDef htim2;

volatile uint32_t last_capture = 0;
volatile uint32_t tooth_time = 0;
volatile uint32_t full_tooth_time = 0;
volatile uint32_t prev_tooth_time = 0;

volatile uint8_t tooth_count = 0;
volatile uint8_t synced = 0;

volatile uint32_t full_tooth_time1 = 0;
volatile uint32_t full_tooth_time2 = 0;
volatile uint32_t full_tooth_time3 = 0;
volatile uint32_t full_tooth_time4 = 0;
volatile uint32_t rpm = 0;
//////////////////////////////////////////////////////
///////////////////IGNITION VARIABLES ////////////////
extern TIM_HandleTypeDef htim3;

volatile uint16_t ignition_target_angle_cyl1 = 0;
volatile uint8_t ign_cyl1 = 0;
uint8_t pulse_time = 100;
volatile uint16_t angle_test = 0;
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
	    int len = sprintf(uart_buffer, "tooth: %u\r\n", angle_test);
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
void ignition_angle_set(int16_t angle)
{
    while(angle < 0)
        angle += 3600;

    while(angle >= 3600)
        angle -= 3600;

    ignition_target_angle_cyl1 = angle;
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2 &&
     htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)

  {
    uint32_t capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);

//    if(tooth_count == angle_test){
//    	__HAL_TIM_SET_AUTORELOAD(&htim3, 10-1);
//    	__HAL_TIM_SET_COUNTER(&htim3, 0);
//    	HAL_TIM_Base_Start_IT(&htim3);
//    }

    if(ignition_target_angle_cyl1 >= tooth_count * 60 && ignition_target_angle_cyl1 < (tooth_count + 1) * 60){
    			__HAL_TIM_SET_AUTORELOAD(&htim3, 100-1);
    	        __HAL_TIM_SET_COUNTER(&htim3, 0);
    	        HAL_TIM_Base_Start_IT(&htim3);
    }
    tooth_time = capture - last_capture;
    last_capture = capture;

    if(prev_tooth_time != 0)
    {
      if(tooth_time > prev_tooth_time * 3 / 2)
      {

        synced = 1;
        if(tooth_count != 58){
        	synced = 0;
        }
        tooth_count = 0;
      }
      else
      {
        tooth_count++;



        if(tooth_time > 0)
        {
        full_tooth_time1 = tooth_time;
        full_tooth_time = (full_tooth_time1+full_tooth_time2+full_tooth_time3+full_tooth_time4)/4;
    	rpm = 60000000 / (full_tooth_time * 60);
        }
      }
    }

    prev_tooth_time = tooth_time;
    full_tooth_time4 = full_tooth_time3;
    full_tooth_time3 = full_tooth_time2;
    full_tooth_time2 = full_tooth_time1;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
    	if(ign_cyl1 == 0){
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    		__HAL_TIM_SET_COUNTER(&htim3, 0);
    		__HAL_TIM_SET_AUTORELOAD(&htim3, pulse_time-1);

    		ign_cyl1 = 1;

    	}else{
    		HAL_TIM_Base_Stop_IT(&htim3);
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    		ign_cyl1 = 0;
    		HAL_TIM_Base_Stop_IT(&htim3);
    	}



    }
}
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
