/*
 * trigger.c
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */
#include <trigger.h>
#include <ignition.h>
#include <engine_info.h>

volatile uint32_t zero_offest = 0;

volatile uint32_t last_capture = 0;
volatile uint32_t tooth_time = 0;

volatile uint32_t prev_full_tooth_time = 0;


volatile uint32_t prev_tooth_time = 0;

volatile uint8_t tooth_count = 0;
volatile uint8_t synced = 0;


volatile uint8_t cam_side = 0;
volatile uint32_t tooth_buf[TOOTH_BUF_SIZE] = {0};
volatile uint8_t tooth_idx = 0;

volatile uint16_t crank_angle = 0;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM2 &&
     htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)

  {
    uint32_t capture = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);



    tooth_time = capture - last_capture;
    last_capture = capture;

    if(prev_tooth_time != 0)
    {


      if(tooth_time > prev_tooth_time * 3 / 2)
      {

        synced = 1;
        cam_side = (GPIOD->IDR & GPIO_IDR_ID12) != 0U;
        if(cam_side){
        	crank_angle += 180;}
        else {
        	crank_angle = 0;
        	ignition_cyl = 1;}

        if(tooth_count != 58){
        	synced = 0;

        }
        tooth_count = 0;

      }
      else
      {
        tooth_count++;
        crank_angle += 60;


        if(tooth_time > 0)
        {
        	tooth_buf[tooth_idx] = tooth_time;
        	tooth_idx++;
        	if(tooth_idx >= TOOTH_BUF_SIZE)
        	    tooth_idx = 0;


        	uint32_t med = median5((uint32_t*)tooth_buf);

        	if(med > 0)
        	{
        		prev_full_tooth_time = full_tooth_time;
        		full_tooth_time = med;

        		omega_prev = omega;
        		omega = 60.0f / full_tooth_time;

        		float dt = (full_tooth_time + prev_full_tooth_time) * 0.5f;

        		alpha = (omega - omega_prev) / dt;



        	}

        }
      }
    }

    prev_tooth_time = tooth_time;






	uint16_t sector = (tooth_count == 57) ? 180 : 60;

	uint32_t tooth_time = (tooth_count == 57) ?
	                      (3 * full_tooth_time) :
	                      full_tooth_time;

	for(uint8_t cyl = 0; cyl < 5; cyl+=2)
	{
	    uint16_t target = ignition_target_angle[cyl];

	    if(target >= crank_angle &&
	       target < crank_angle + sector)
	    {
	        uint16_t delta = target - crank_angle;

	        uint32_t delay = (delta * tooth_time) / sector;

	        __HAL_TIM_SET_AUTORELOAD(&htim3, delay + 1);
	        __HAL_TIM_SET_COUNTER(&htim3, 0);

	        ignition_cyl = cyl + 1;
	        HAL_TIM_Base_Start_IT(&htim3);

	        break;
	    }
	}
	for(uint8_t cyl = 1; cyl < 6; cyl+=2)
		{
		    uint16_t target = ignition_target_angle[cyl];

		    if(target >= crank_angle &&
		       target < crank_angle + sector)
		    {
		        uint16_t delta = target - crank_angle;

		        uint32_t delay = (delta * tooth_time) / sector;

		        __HAL_TIM_SET_AUTORELOAD(&htim4, delay + 1);
		        __HAL_TIM_SET_COUNTER(&htim4, 0);

		        ignition_cyl = cyl + 1;
		        HAL_TIM_Base_Start_IT(&htim4);

		        break;
		    }
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
