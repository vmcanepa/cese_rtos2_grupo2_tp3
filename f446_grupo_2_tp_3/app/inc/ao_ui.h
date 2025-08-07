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

typedef struct msg_s msg_t;

typedef void (*ui_callback_t)(msg_t* pmsg); // cuando UI termina, le avisa a button

struct msg_s
{
    size_t size;
    msg_event_t data;
    ui_callback_t process_cb;
};


/********************** external functions declaration ***********************/
bool ao_ui_init(void);
bool ao_ui_send_event(msg_event_t event, ui_callback_t cbFunction);
void ao_ui_process(void);
void ao_ui_callback(ao_led_message_t * pmsg);
void ao_ui_queue_delete(void);
void ui_running_update(void);

#endif /* INC_AO_UI_H_ */
