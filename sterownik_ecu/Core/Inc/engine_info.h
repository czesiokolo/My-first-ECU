/*
 * engine_info.h
 *
 *  Created on: Aug 5, 2026
 *      Author: admin
 */

#ifndef INC_ENGINE_INFO_H_
#define INC_ENGINE_INFO_H_
#include "stm32f4xx_hal.h"

//actual

extern volatile uint16_t rpm;
extern volatile uint32_t full_tooth_time;
extern volatile uint16_t ignition_time;
extern volatile uint32_t dwell_time;


extern volatile uint16_t map;
extern volatile uint16_t maf;
extern volatile uint16_t voltage;
extern volatile uint16_t lambda;
extern volatile uint16_t egt;
extern volatile uint16_t iat;
extern volatile uint16_t coolant_temp_engine;
extern volatile uint16_t coolant_temp_cooler;
extern volatile uint16_t accelerator;
extern volatile uint16_t rpm_regulator;


//target
extern volatile uint16_t target_lambda;
extern volatile uint16_t target_rpm;
extern volatile uint16_t target_rpm_regulator;

void engine_update();

void dwell_update();
void map_update();
void maf_update();

#endif /* INC_ENGINE_INFO_H_ */
