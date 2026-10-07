/*
 * ignition.c
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */
#include <ignition.h>
#include <engine_info.h>
#include <main.h>
volatile uint8_t ignition_cyl = 1;
volatile uint16_t ignition_target_angle_cyl1 = 0;
volatile uint16_t ignition_target_angle_cyl2 = 0;
volatile uint16_t ignition_target_angle_cyl3 = 0;
volatile uint16_t ignition_target_angle_cyl4 = 0;
volatile uint16_t ignition_target_angle_cyl5 = 0;
volatile uint16_t ignition_target_angle_cyl6 = 0;
volatile uint16_t ignition_angle = 0;


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

    	if(ignition_cyl == 1){
    		HAL_GPIO_WritePin(Ign_1_GPIO_Port, Ign_1_Pin, GPIO_PIN_SET);
    		__HAL_TIM_SET_COUNTER(&htim5, 0);
    		__HAL_TIM_SET_AUTORELOAD(&htim5, dwell_time-1);
    		HAL_TIM_Base_Start_IT(&htim5);
    		HAL_TIM_Base_Stop(&htim3);
    	} else if(ignition_cyl == 3){
    		HAL_GPIO_WritePin(Ign_3_GPIO_Port, Ign_3_Pin, GPIO_PIN_SET);
    		__HAL_TIM_SET_COUNTER(&htim5, 0);
    		__HAL_TIM_SET_AUTORELOAD(&htim5, dwell_time-1);
    		HAL_TIM_Base_Start_IT(&htim5);
    		HAL_TIM_Base_Stop(&htim3);
    	} else if(ignition_cyl == 5){
    		HAL_GPIO_WritePin(Ign_2_GPIO_Port, Ign_2_Pin, GPIO_PIN_SET);
    		__HAL_TIM_SET_COUNTER(&htim5, 0);
    		__HAL_TIM_SET_AUTORELOAD(&htim5, dwell_time-1);
    		HAL_TIM_Base_Start_IT(&htim5);
    		HAL_TIM_Base_Stop(&htim3);
    	}
    }
    else if(htim->Instance == TIM4)
        {

        	if(ignition_cyl == 2){
        		HAL_GPIO_WritePin(Ign_5_GPIO_Port, Ign_5_Pin, GPIO_PIN_SET);
        		__HAL_TIM_SET_COUNTER(&htim6, 0);
        		__HAL_TIM_SET_AUTORELOAD(&htim6, dwell_time-1);
        		HAL_TIM_Base_Start_IT(&htim6);
        		HAL_TIM_Base_Stop(&htim4);
        	} else if(ignition_cyl == 4){
        		HAL_GPIO_WritePin(Ign_6_GPIO_Port, Ign_6_Pin, GPIO_PIN_SET);
        		__HAL_TIM_SET_COUNTER(&htim6, 0);
        		__HAL_TIM_SET_AUTORELOAD(&htim6, dwell_time-1);
        		HAL_TIM_Base_Start_IT(&htim6);
        		HAL_TIM_Base_Stop(&htim4);
        	} else if(ignition_cyl == 6){
        		HAL_GPIO_WritePin(Ign_4_GPIO_Port, Ign_4_Pin, GPIO_PIN_SET);
        		__HAL_TIM_SET_COUNTER(&htim6, 0);
        		__HAL_TIM_SET_AUTORELOAD(&htim6, dwell_time-1);
        		HAL_TIM_Base_Start_IT(&htim6);
        		HAL_TIM_Base_Stop(&htim4);
        	}
        }
    else if(htim->Instance == TIM5)
            {
    	HAL_GPIO_WritePin(Ign_1_GPIO_Port, Ign_1_Pin, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(Ign_3_GPIO_Port, Ign_3_Pin, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(Ign_2_GPIO_Port, Ign_2_Pin, GPIO_PIN_RESET);
    	HAL_TIM_Base_Stop(&htim5);
            }
    else if(htim->Instance == TIM6)
                {
    	HAL_GPIO_WritePin(Ign_5_GPIO_Port, Ign_5_Pin, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(Ign_6_GPIO_Port, Ign_6_Pin, GPIO_PIN_RESET);
    	HAL_GPIO_WritePin(Ign_4_GPIO_Port, Ign_4_Pin, GPIO_PIN_RESET);
    	HAL_TIM_Base_Stop(&htim6);
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
