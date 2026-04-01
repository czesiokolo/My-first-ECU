/*
 * trigger.h
 *
 *  Created on: Mar 27, 2026
 *      Author: admin
 */

#ifndef INC_TRIGGER_H_
#define INC_TRIGGER_H_
#include "stm32f4xx_hal.h"

#define TOOTH_BUF_SIZE 5

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

//////////////////////////////////////////////////////////
/////////////////CRANKSHAFT SENSOR VARIABLES /////////////
extern TIM_HandleTypeDef htim2;

extern volatile uint32_t zero_offest;

extern volatile uint32_t last_capture;
extern volatile uint32_t tooth_time;
extern volatile uint32_t full_tooth_time;
extern volatile uint32_t prev_tooth_time;

extern volatile uint8_t tooth_count;
extern volatile uint8_t synced;

extern volatile uint32_t tooth_buf[TOOTH_BUF_SIZE];
extern volatile uint8_t tooth_idx;

extern volatile uint32_t rpm;

uint32_t median5(uint32_t *buf);
#endif /* INC_TRIGGER_H_ */
