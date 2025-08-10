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
#include "priority_queue.h"

/********************** macros and definitions *******************************/
#define TASK_PERIOD_MS_         (50)
#define QUEUE_LED_LENGTH_		(10)
#define QUEUE_LED_ITEM_SIZE_	(sizeof(ao_led_message_t*))

/********************** internal data definition *****************************/
static GPIO_TypeDef* led_port_[] = {LED_RED_PORT, LED_GREEN_PORT,  LED_BLUE_PORT};
static uint16_t led_pin_[] = {LED_RED_PIN,  LED_GREEN_PIN, LED_BLUE_PIN };
static const char *colorNames[] = {"RED", "GREEN", "BLUE"};
static const char *prioNames[] = {"LOW", "MED", "HIGH"};
static bool led_task_running = false;

/********************** internal functions declaration ***********************/
static void task_led(void *argument);
static void turnOnLed(ao_led_color_t color, TickType_t * t0_led_on);
static void turnOffLed(ao_led_color_t color);

/********************** internal functions definition ************************/
static inline void todos_los_led_apagados(void) {

	HAL_GPIO_WritePin(LED_RED_PORT,   LED_RED_PIN,   LED_OFF);
	HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_OFF);
	HAL_GPIO_WritePin(LED_BLUE_PORT,  LED_BLUE_PIN,  LED_OFF);
}

static void turnOnLed(ao_led_color_t color, TickType_t * t0_led_on) {

	HAL_GPIO_WritePin(led_port_[color], led_pin_[color], LED_ON);
	*t0_led_on = xTaskGetTickCount(); /* comenzar a contar tiempo desde led encendido. */
}

static void turnOffLed(ao_led_color_t color) {

	HAL_GPIO_WritePin(led_port_[color], led_pin_[color], LED_OFF);
}

/********************** external functions declaration ***********************/

static void task_led(void *argument) {

	LOGGER_INFO("[LED] tarea iniciada");

	while(true)	{

		prio_queue_priority_t prio;
		data_queue_t data;

		/* Sacar de la cola de prioridad:
		 */
		if(prio_queue_extract(&data, &prio)) {

			if(AO_LED_MESSAGE_ON == data.action) {

				TickType_t xLastLedOnTime;
				/* Encender por 5 segundos el LED de prioridad p: */
				LOGGER_INFO("[LED] ON %s (p=%s)", colorNames[data.color], prioNames[prio]);
				turnOnLed(data.color, &xLastLedOnTime);
				vTaskDelayUntil(&xLastLedOnTime, pdMS_TO_TICKS(5000));
				turnOffLed(data.color);
				LOGGER_INFO("[LED] OFF %s", colorNames[data.color]);
			}
		} else {

			vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
		}
	}
}

bool ao_led_init() {

	if(led_task_running) /* si la tarea ya ha sido creada... */
		return true;

	if(pdPASS == xTaskCreate(task_led, "task_led", 128, NULL, tskIDLE_PRIORITY, NULL)) {

		todos_los_led_apagados();
		led_task_running = true;
		LOGGER_INFO("[LED] tarea creada");
		return true;
	}
	LOGGER_INFO("[LED] error en ao_led_init().");
	return false;
}

/********************** end of file ******************************************/
