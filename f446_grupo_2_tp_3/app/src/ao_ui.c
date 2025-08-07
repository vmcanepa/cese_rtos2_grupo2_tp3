/*
 * ao_ui.c
 *
 *  Created on: Aug 2, 2025
 *      Author: grupo 2 RTOS II
 */

/********************** inclusions *******************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao_ui.h"

/********************** macros and definitions *******************************/
#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(msg_t*))

typedef enum {

	UI_STATE_STANDBY,
	UI_STATE_RED,
	UI_STATE_GREEN,
	UI_STATE_BLUE,
} ui_state_t;

/********************** external data declaration *****************************/
extern ao_led_handle_t led_red, led_green, led_blue;
extern bool ao_running;

/********************** internal data definition *****************************/
static QueueHandle_t hqueue;
static ui_state_t estado_ui = UI_STATE_STANDBY;

/********************** external functions definition ************************/
void ao_ui_process(void) {

	msg_t* pmsg;

	if(pdPASS == xQueueReceive(hqueue, (void*) &pmsg, 0)) {

		bool msgSent = pdFAIL;

		switch(pmsg->data) {

			case MSG_EVENT_BUTTON_PULSE:
				msgSent = pdFAIL;
				if(UI_STATE_STANDBY == estado_ui)
					msgSent = pdPASS;			// no requiere manda ningun otro mensaje
				if(UI_STATE_GREEN == estado_ui)
					msgSent = ao_led_send(&led_green, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(UI_STATE_BLUE == estado_ui)
					msgSent = ao_led_send(&led_blue, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(msgSent && ao_led_send(&led_red,  AO_LED_MESSAGE_ON, ao_ui_callback)) {
					estado_ui = UI_STATE_RED;
					LOGGER_INFO("[UI] Estado RED");
				}
				break;
			case MSG_EVENT_BUTTON_SHORT:
				if(UI_STATE_STANDBY == estado_ui)
					msgSent = pdPASS;			// no requiere manda ningun otro mensaje
				if(UI_STATE_RED == estado_ui)
					msgSent = ao_led_send(&led_red, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(UI_STATE_BLUE == estado_ui)
					msgSent = ao_led_send(&led_blue, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(msgSent && ao_led_send(&led_green, AO_LED_MESSAGE_ON, ao_ui_callback)){
					estado_ui = UI_STATE_GREEN;
					LOGGER_INFO("[UI] Estado GREEN");
				}
				break;
			case MSG_EVENT_BUTTON_LONG:
				if(UI_STATE_STANDBY == estado_ui)
					msgSent = pdPASS;			// no requiere manda ningun otro mensaje
				if(UI_STATE_RED == estado_ui)
					msgSent = ao_led_send(&led_red, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(UI_STATE_GREEN == estado_ui)
					msgSent = ao_led_send(&led_green, AO_LED_MESSAGE_OFF, ao_ui_callback);
				if(msgSent && ao_led_send(&led_blue, AO_LED_MESSAGE_ON, ao_ui_callback)){
					estado_ui = UI_STATE_BLUE;
					LOGGER_INFO("[UI] Estado BLUE");
				}
				break;
			default:
				break;
		}
		// Aviso al callback del remitente (button)
		if (pmsg->process_cb)
			pmsg->process_cb(pmsg);
	}
}

bool ao_ui_init(void) {
	// agrego logica para que se cree la cola solo si no hay una creada
	if(NULL == hqueue) {
		hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);

		if(NULL == hqueue) {

			LOGGER_INFO("[UI] Error! Falla creación de cola. Abortando init de UI.");
			return false;
		}
		vQueueAddToRegistry(hqueue, "Cola UI");
		LOGGER_INFO("[UI] Crea cola UI");
	}
	return true;
}

bool ao_ui_send_event(msg_event_t event, ui_callback_t cbFunction) {

	if(NULL == hqueue) {

		LOGGER_INFO("[UI] Error! Cola UI no exite.");
		return false;
	}
	msg_t* pmsg = pvPortMalloc(sizeof(msg_t));

	if(!pmsg)
		return false;

	pmsg->size = sizeof(msg_t);
	pmsg->data = event;
	pmsg->process_cb = cbFunction;
	BaseType_t result = xQueueSend(hqueue, &pmsg, 0);

	if(pdPASS != result) {

		LOGGER_INFO("[UI] Error! Cola UI llena, liberando mem del msg.");
		vPortFree(pmsg);
		return false;
	}
	return true;
}

void ao_ui_callback(ao_led_message_t* pmsg) {

    vPortFree((void*)pmsg);
    LOGGER_INFO("[UI] Callback: memoria de mensaje LED liberada");
}

void ao_ui_queue_delete(void) {

	if(NULL != hqueue) {

		msg_t* pmsg;

		while(pdPASS == xQueueReceive(hqueue, (void*)&pmsg, 0)) {

			vPortFree((void*)pmsg);		// libero la memoria de posibles mensajes encolados
		}
		vQueueDelete(hqueue);
		hqueue = NULL;
	}
}

void ui_running_update(void) {

	// chequear si hay mensajes para procesar en alguna cola
	// uxQueueMessagesWaiting: Devuelve la cantidad de mensajes actualmente en la cola.
	UBaseType_t msgInQueues = 0;
	msgInQueues += uxQueueMessagesWaiting(hqueue);						// cola UI

	if(NULL != led_red.hqueue)
		msgInQueues += uxQueueMessagesWaiting(led_red.hqueue);			// cola RED

	if(NULL != led_green.hqueue)
		msgInQueues += uxQueueMessagesWaiting(led_green.hqueue);	// cola GREEN

	if(NULL != led_blue.hqueue)
		msgInQueues += uxQueueMessagesWaiting(led_blue.hqueue);		// cola BLUE

	if(!msgInQueues) ao_running = false;
}

/********************** end of file ******************************************/
