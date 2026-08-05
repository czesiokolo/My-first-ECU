/*
 * ignition.c
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */
#include <ignition.h>
#include <engine_info.h>
volatile uint8_t ignition_cyl = 1;
volatile uint16_t ignition_target_angle_cyl1 = 0;
volatile uint16_t ignition_target_angle_cyl2 = 0;
volatile uint16_t ignition_target_angle_cyl3 = 0;
volatile uint16_t ignition_target_angle_cyl4 = 0;
volatile uint16_t ignition_target_angle_cyl5 = 0;
volatile uint16_t ignition_target_angle_cyl6 = 0;
volatile uint16_t ignition_angle = 0;
volatile uint8_t ign_cyl1 = 0;
volatile uint16_t angle_test = 0;


volatile uint8_t coil_state[6] = {0};
volatile uint16_t ignition_target_angle[6];

volatile float omega = 0; //predkosc katowa
volatile float omega_prev = 0;
volatile float alpha = 0; //przyspieszenie katowe




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
int16_t angle_rollover(int16_t angle){
	angle += zero_offest;
	    while(angle < 0)
	        angle += 7200;

	    while(angle >= 7200)
	        angle -= 7200;
	    return angle;

}

void ignition_angle_set(int16_t angle)
{



    ignition_target_angle[0] = angle_rollover(angle);
    ignition_target_angle[1] = angle_rollover(angle + 1200);
    ignition_target_angle[2] = angle_rollover(angle + 2400);
    ignition_target_angle[3] = angle_rollover(angle + 3600);
    ignition_target_angle[4] = angle_rollover(angle + 4800);
    ignition_target_angle[5] = angle_rollover(angle + 6000);

}

void ignition_update(void)
{
    float t = (float)dwell_time;

    float delta_angle =
        omega * t +
        0.5f * alpha * t * t;

    ignition_angle_set(
        ignition_angle - (int16_t)(delta_angle + 0.5f)
    );
}
