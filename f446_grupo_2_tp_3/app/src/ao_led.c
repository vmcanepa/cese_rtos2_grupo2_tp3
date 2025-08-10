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
static void turnOnLed(ao_led_color_t color);
static void turnOffLed(ao_led_color_t color);

/********************** internal functions definition ************************/
static void turnOnLed(ao_led_color_t color) {

	HAL_GPIO_WritePin(led_port_[color], led_pin_[color], LED_ON);
}

static void turnOffLed(ao_led_color_t color) {

	HAL_GPIO_WritePin(led_port_[color], led_pin_[color], LED_OFF);
}

/********************** external functions declaration ***********************/

static void task_led(void *argument) {

	data_queue_t data;

	// sacar de la cola de prioridad

	// encender el led que corresponda

	// delay de 5 seg

	// apagar el led
}

bool ao_led_init() {

	// esta deberia crear la tarea!!!!


	return true;
}


/********************** end of file ******************************************/
