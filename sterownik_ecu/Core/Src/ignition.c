/*
 * ignition.c
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */
#include <ignition.h>
volatile uint16_t ignition_target_angle_cyl1 = 0;
volatile uint16_t ignition_angle = -60;
volatile uint8_t ign_cyl1 = 0;
volatile uint16_t angle_test = 0;
volatile uint16_t dwell_time = 1300;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
    	if(ign_cyl1 == 0){
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    		__HAL_TIM_SET_COUNTER(&htim3, 0);
    		__HAL_TIM_SET_AUTORELOAD(&htim3, dwell_time-1);

    		ign_cyl1 = 1;

    	}else{
    		HAL_TIM_Base_Stop_IT(&htim3);
    		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    		ign_cyl1 = 0;
    		HAL_TIM_Base_Stop_IT(&htim3);
    	}



    }
}
void ignition_angle_set(int16_t angle)
{
    while(angle < 0)
        angle += 3600;

    while(angle >= 3600)
        angle -= 3600;

    ignition_target_angle_cyl1 = angle;
}
void ignition_update(){

		int32_t delta_angle = (int32_t)dwell_time * 60 / full_tooth_time;
	    int16_t ignition_charge_start = ignition_angle - delta_angle;

	    ignition_angle_set(ignition_charge_start);
}
