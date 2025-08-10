/*
 * ao_ui.h
 *
 *  Created on: Aug 2, 2025
 *      Author: grupo 2 RTOS II
 */

#ifndef INC_AO_UI_H_
#define INC_AO_UI_H_

#include <stdbool.h>
#include <stddef.h>

#include "ao_led.h"

/********************** typedef **********************************************/
typedef enum {

	MSG_EVENT_BUTTON_PULSE,
	MSG_EVENT_BUTTON_SHORT,
	MSG_EVENT_BUTTON_LONG,
	MSG_EVENT__N,
} msg_event_t;


/********************** external functions declaration ***********************/
bool ao_ui_init(void);
bool ao_ui_send_event(msg_event_t event);

#endif /* INC_AO_UI_H_ */
