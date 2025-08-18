/*
 * priority_queue.h
 *
 *  Created on: Aug 9, 2025
 *      Author: cese_rtos2_grupo_2
 */

#ifndef INC_PRIORITY_QUEUE_H_
#define INC_PRIORITY_QUEUE_H_

#include <stdbool.h>
#include "ao_led.h"

typedef enum {

  PRIO_QUEUE_PRIORITY_LOW,
  PRIO_QUEUE_PRIORITY_MEDIUM,
  PRIO_QUEUE_PRIORITY_HIGH
} prio_queue_priority_t;


bool prio_queue_init();
bool prio_queue_insert(data_queue_t data, prio_queue_priority_t priority);
bool prio_queue_extract(data_queue_t * data, prio_queue_priority_t * priority, TickType_t timeout);

#endif /* INC_PRIORITY_QUEUE_H_ */
