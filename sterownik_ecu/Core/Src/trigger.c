/*
 * trigger.c
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */
#include <trigger.h>
#include <ignition.h>

volatile uint32_t zero_offest = 0;

volatile uint32_t last_capture = 0;
volatile uint32_t tooth_time = 0;
volatile uint32_t full_tooth_time = 0;

volatile uint32_t prev_tooth_time = 0;

volatile uint8_t tooth_count = 0;
volatile uint8_t synced = 0;



volatile uint32_t tooth_buf[TOOTH_BUF_SIZE] = {0};
volatile uint8_t tooth_idx = 0;
volatile uint32_t rpm = 0;

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
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, synced);
        tooth_count = 0;
      }
      else
      {
        tooth_count++;



        if(tooth_time > 0)
        {
        	tooth_buf[tooth_idx] = tooth_time;
        	tooth_idx++;
        	if(tooth_idx >= TOOTH_BUF_SIZE)
        	    tooth_idx = 0;


        	uint32_t med = median5((uint32_t*)tooth_buf);

        	if(med > 0)
        	{
        	    full_tooth_time = med;
        	    rpm = 1000000 / full_tooth_time;
        	}

        }
      }
    }

    prev_tooth_time = tooth_time;



	uint16_t base_angle = tooth_count * 60;
	uint16_t delta = ignition_target_angle_cyl1 - base_angle;
	uint32_t delay;

    uint8_t in_range = (ignition_target_angle_cyl1 >= base_angle && ignition_target_angle_cyl1 < (tooth_count + 1) * 60);
    uint8_t in_gap = (tooth_count == 57 && ignition_target_angle_cyl1 >= 3480);
    if(in_range || in_gap){

    	if(tooth_count == 57 && ignition_target_angle_cyl1 >= 3480)
    	{
    	    uint16_t before_gap = 3480 - base_angle;
    	    uint16_t in_gap = delta - before_gap;

    	    delay = (before_gap * full_tooth_time) / 60
    	          + (in_gap * full_tooth_time) / 60;
    	}
    	else
    	{
    	    delay = (delta * full_tooth_time) / 60;
    	}


        			__HAL_TIM_SET_AUTORELOAD(&htim3, delay+1);
        	        __HAL_TIM_SET_COUNTER(&htim3, 0);
        	        HAL_TIM_Base_Start_IT(&htim3);
        }
  }
}
uint32_t median5(uint32_t *buf)
{
    uint32_t a[5];

    for(int i = 0; i < 5; i++)
        a[i] = buf[i];

    // prosty bubble sort (5 elementów = OK)
    for(int i = 0; i < 4; i++)
    {
        for(int j = i + 1; j < 5; j++)
        {
            if(a[j] < a[i])
            {
                uint32_t tmp = a[i];
                a[i] = a[j];
                a[j] = tmp;
            }
        }
    }

    return a[2]; // mediana
}
