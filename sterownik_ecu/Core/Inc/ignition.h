/*
 * ignition.h
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */

#ifndef INC_IGNITION_H_
#define INC_IGNITION_H_
#include "stm32f4xx_hal.h"
#include <ignition.h>
#include <trigger.h>
//////////////////////////////////////////////////////
///////////////////IGNITION VARIABLES ////////////////
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern volatile uint16_t ignition_target_angle_cyl1;
extern volatile uint8_t ign_cyl1;
extern uint16_t pulse_time;
extern volatile uint16_t angle_test;

extern volatile uint8_t ignition_cyl;

extern volatile uint16_t ignition_target_angle[6];

extern volatile float omega; //predkosc katowa
extern volatile float omega_prev;
extern volatile float alpha; //przyspieszenie katowe


#endif /* INC_IGNITION_H_ */


void ignition_angle_set(int16_t angle);
void ignition_update();
