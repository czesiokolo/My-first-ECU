/*
 * scheduler.h
 *
 *  Created on: Oct 13, 2026
 *      Author: admin
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_
#include "stm32f4xx_hal.h"
#include <stdbool.h>
typedef enum
{
    EVENT_IGNITION_DWELL_ON,
    EVENT_IGNITION_SPARK_OFF,
    EVENT_INJECTOR_ON,
    EVENT_INJECTOR_OFF
} event_type_t;


bool events_add_or_update(event_type_t type,
                          uint8_t channel,
                          uint32_t deadline);
#endif /* INC_SCHEDULER_H_ */
