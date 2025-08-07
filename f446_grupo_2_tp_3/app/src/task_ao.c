/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
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

#include "task_ao.h"
#include "ao_ui.h"
#include "ao_led.h"

/********************** macros and definitions *******************************/
#define TASK_PERIOD_MS_           (50)
#define NLEDS 3

/********************** external data definition *****************************/
bool ao_running = false;
ao_led_handle_t led_red, led_green, led_blue;

/********************** internal data definition *****************************/
static ao_led_handle_t * haos[NLEDS];

/********************** internal functions declaration ***********************/
static void task_ao(void* argument);
static void task_ao_delete(void);

/********************** internal functions definition ************************/
static void task_ao(void* argument) {

	ao_led_init(&led_red, AO_LED_COLOR_RED);
	ao_led_init(&led_green, AO_LED_COLOR_GREEN);
	ao_led_init(&led_blue, AO_LED_COLOR_BLUE);
    haos[0] = &led_red;
    haos[1] = &led_green;
    haos[2] = &led_blue;

	while(ao_running) {

		ao_ui_process();

		for(uint8_t i = 0; i < NLEDS; i++) {

			ao_led_process(haos[i]);
		}
		ui_running_update();
		vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
	}
	task_ao_delete();
}

static void task_ao_delete(void) {

	LOGGER_INFO("[AO] Elimino tarea AO y cola UI"); // se elimina en cualquier estado
	taskENTER_CRITICAL(); {		// seccion critica para que nadie mande mensajes mientras elimino

		ao_ui_queue_delete();

		for(uint8_t i = 0; i < NLEDS; i++) {

			ao_led_delete_cola(haos[i]);
		}
	} taskEXIT_CRITICAL();
	vTaskDelete(NULL);
}

/********************** external functions definition ************************/
bool task_ao_init(void) {

	// agrego logica para que se cree la tarea solo si no hay una corriendo
	if(!ao_running) {

		BaseType_t status;
		status = xTaskCreate(task_ao, "task_ao", 128, NULL, tskIDLE_PRIORITY + 1, NULL);

		if(pdPASS != status){
			LOGGER_INFO("[UI] Error! Falla creación de tarea. Abortando init de AO.");
			return false;				// salgo de ao_ui_init
		}
		ao_running = true;
		LOGGER_INFO("[AO] Crea tarea AO");
	}

	return ao_ui_init();
}

/********************** end of file ******************************************/
