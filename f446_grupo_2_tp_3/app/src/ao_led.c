/*
 * ao_led.c
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

#include "ao_led.h"

/********************** macros and definitions *******************************/
#define QUEUE_LED_LENGTH_		(10)
#define QUEUE_LED_ITEM_SIZE_	(sizeof(ao_led_message_t*))

/********************** internal data definition *****************************/
static GPIO_TypeDef* led_port_[] = {LED_RED_PORT, LED_GREEN_PORT,  LED_BLUE_PORT};
static uint16_t led_pin_[] = {LED_RED_PIN,  LED_GREEN_PIN, LED_BLUE_PIN };
static const char *colorNames[] = {"RED", "GREEN", "BLUE"};

/********************** internal functions declaration ***********************/
static void turnOnLed(ao_led_handle_t* hao);
static void turnOffLed(ao_led_handle_t* hao);

/********************** internal functions definition ************************/
static void turnOnLed(ao_led_handle_t* hao) {

	HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_ON);
}

static void turnOffLed(ao_led_handle_t* hao) {

	HAL_GPIO_WritePin(led_port_[hao->color], led_pin_[hao->color], LED_OFF);
}

/********************** external functions declaration ***********************/
void ao_led_process(ao_led_handle_t * hao) {

	ao_led_message_t* pmsg;

	if (pdPASS == xQueueReceive(hao->hqueue, (void*)&pmsg, 0)) {

		if (AO_LED_MESSAGE_ON == pmsg->action)
			turnOnLed(hao);
		else
			turnOffLed(hao);
		pmsg->process_cb(pmsg); // corre collback de UI que libera la mem
	}
}

bool ao_led_init(ao_led_handle_t* hao, ao_led_color_t color) {

	hao->color = color;
	if(NULL == hao->hqueue){ // solo se crea si no hay una creada
		hao->hqueue = xQueueCreate(QUEUE_LED_LENGTH_, QUEUE_LED_ITEM_SIZE_);

		if(NULL == hao->hqueue) {

			LOGGER_INFO("[LED] Error! Falla creación de cola. Abortando init de LED %d", hao->color);
			return false;		// salgo de ao_led_init
		}
		vQueueAddToRegistry(hao->hqueue, colorNames[hao->color]);
		LOGGER_INFO("[LED] Crea cola led %d", hao->color);
	}
	return true;
}

bool ao_led_send(ao_led_handle_t* hao, ao_led_action_t msg, led_callback_t cbFunction) {

	BaseType_t status =  pdFAIL;

	if(NULL == hao->hqueue){ // si el init de led falló la cola no existe
		LOGGER_INFO("[LED] Error. Cola de led %d no existe", hao->color);
		return status;
	}

	ao_led_message_t* pmsg = (ao_led_message_t*)pvPortMalloc(sizeof(ao_led_message_t));

	if(NULL != pmsg) {

		pmsg->action = msg;
		pmsg->process_cb = cbFunction;
		status = xQueueSend(hao->hqueue, (void*)&pmsg, 0);

		if(pdPASS == status) {

			LOGGER_INFO("[LED] mensaje enviado");
		} else {

			LOGGER_INFO("[LED] mensaje no enviado");
			vPortFree((void*)pmsg);
			LOGGER_INFO("[LED] memoria liberada");
		}
	} else {

        LOGGER_INFO("[LED] Memoria insuficiente");
    }
	return (status == pdPASS);
}

void ao_led_delete_cola(ao_led_handle_t* hao) {

	// el logging no puede quedar adentro de las secciones criticas
	if(NULL != hao->hqueue) {

		LOGGER_INFO("[LED] Cola eliminada para LED %d", hao->color);
		taskENTER_CRITICAL(); {				// seccion critica para que nadie inserte mensajes mientras vacio la cola

			ao_led_message_t* pmsg;

			while(pdPASS == xQueueReceive(hao->hqueue, (void*)&pmsg, 0)) {

				vPortFree((void*)pmsg);		// libero la memoria de posibles mensajes encolados
			}
			vQueueDelete(hao->hqueue);
			hao->hqueue = NULL;
		}taskEXIT_CRITICAL();
	}
}

/********************** end of file ******************************************/
