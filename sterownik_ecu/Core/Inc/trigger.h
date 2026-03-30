/*
 * trigger.h
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */

#ifndef INC_TRIGGER_H_
#define INC_TRIGGER_H_
#include "stm32f4xx_hal.h"
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

//////////////////////////////////////////////////////////
/////////////////CRANKSHAFT SENSOR VARIABLES /////////////
extern TIM_HandleTypeDef htim2;

extern volatile uint32_t last_capture;
extern volatile uint32_t tooth_time;
extern volatile uint32_t full_tooth_time;
extern volatile uint32_t prev_tooth_time;

extern volatile uint8_t tooth_count;
extern volatile uint8_t synced;

extern volatile uint32_t full_tooth_time1;
extern volatile uint32_t full_tooth_time2;
extern volatile uint32_t full_tooth_time3;
extern volatile uint32_t full_tooth_time4;
extern volatile uint32_t rpm;

#endif /* INC_TRIGGER_H_ */
