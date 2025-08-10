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

#include "task_button.h"
#include "ao_ui.h"

/********************** macros and definitions *******************************/
#define TASK_PERIOD_MS_           (50)
#define BUTTON_PULSE_TIMEOUT_     (200)
#define BUTTON_SHORT_TIMEOUT_     (1000)
#define BUTTON_LONG_TIMEOUT_      (2000)


/********************** internal data declaration ****************************/
enum button_type_t {

	BUTTON_TYPE_NONE,
	BUTTON_TYPE_PULSE,
	BUTTON_TYPE_SHORT,
	BUTTON_TYPE_LONG,
	BUTTON_TYPE__N,
};

/********************** internal data definition *****************************/
static struct {

	button_type_t estado;
    uint32_t counter;
} button;



/* ================================================================================================== */
/* Agregar secuencia de entradas de testing que se quieran probar: */
#ifdef TESTING_SIMULATION_BUTTON_INPUTS

	#ifdef TESTING_ARRAY_INPUTS_1
		static const button_type_t s_btn_seq_default[] = {
															BUTTON_TYPE_NONE,
															BUTTON_TYPE_PULSE,
															BUTTON_TYPE_NONE };
	#elif defined(TESTING_ARRAY_INPUTS_2)
		static const button_type_t s_btn_seq_default[] = {
															BUTTON_TYPE_NONE,
															BUTTON_TYPE_PULSE,
															BUTTON_TYPE_NONE,
															BUTTON_TYPE_SHORT,
															BUTTON_TYPE_NONE,
															BUTTON_TYPE_LONG,
															BUTTON_TYPE_NONE };
	#elif defined(TESTING_ARRAY_INPUTS_3)
		static const button_type_t s_btn_seq_default[] = {
															BUTTON_TYPE_NONE,
															BUTTON_TYPE_LONG,
															BUTTON_TYPE_NONE };
	#elif defined(TESTING_ARRAY_INPUTS_4)
		static const button_type_t s_btn_seq_default[] = {
															/* agregar secuencias.. */
															BUTTON_TYPE_NONE };
	#elif defined(TESTING_ARRAY_INPUTS_5)
		static const button_type_t s_btn_seq_default[] = {
															/* agregar secuencias.. */
															BUTTON_TYPE_NONE };
	#elif defined(TESTING_ARRAY_INPUTS_6)
		static const button_type_t s_btn_seq_default[] = {
															/* agregar secuencias.. */
															BUTTON_TYPE_NONE };
	#endif


	static size_t s_btn_len  = sizeof(s_btn_seq_default) / sizeof(s_btn_seq_default[0]);	/* cantidad de elementos. */
	static size_t s_btn_idx  = 0;		                                                    /* indice. */

#endif
/* ================================================================================================== */


/********************** internal functions declaration ***********************/
static void button_init_(void);
/* static button_type_t button_process_state_(bool value); */

/********************** internal functions definition ************************/
static void button_init_(void) {

	button.counter = 0;
}

#ifndef TESTING_SIMULATION_BUTTON_INPUTS

static inline button_type_t button_process_state_(bool value) {

	button_type_t ret = BUTTON_TYPE_NONE;

	if(value) {

		button.counter += TASK_PERIOD_MS_;
	} else {

		if(BUTTON_LONG_TIMEOUT_ <= button.counter) {

			ret = BUTTON_TYPE_LONG;
		} else if(BUTTON_SHORT_TIMEOUT_ <= button.counter) {

			ret = BUTTON_TYPE_SHORT;
		} else if(BUTTON_PULSE_TIMEOUT_ <= button.counter) {

			ret = BUTTON_TYPE_PULSE;
		}
		button.counter = 0;
	}
	return ret;
}

#endif

/********************** external functions definition ************************/
void task_button(void* argument) {

	button_init_();

	while(true) {

		button_type_t button_type = get_button_type();

		switch(button_type) {

			case BUTTON_TYPE_NONE:
				break;
			case BUTTON_TYPE_PULSE:
				if(ao_ui_send_event(MSG_EVENT_BUTTON_PULSE))
					LOGGER_INFO("[BUTTON] pulso enviado");
				break;
			case BUTTON_TYPE_SHORT:
				if(ao_ui_send_event(MSG_EVENT_BUTTON_SHORT))
					LOGGER_INFO("[BUTTON] corto enviado");
				break;
			case BUTTON_TYPE_LONG:
				if(ao_ui_send_event(MSG_EVENT_BUTTON_LONG))
					LOGGER_INFO("[BUTTON] largo enviado");
				break;
			default:
				LOGGER_INFO("[BTN] error");
				break;
		}
		vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
	}
}






/* Obtener tipo de entrada boton */
button_type_t get_button_type(void)
{

#ifndef TESTING_SIMULATION_BUTTON_INPUTS
/* MODO NORMAL: --------------------------------------------------------------
 */

	GPIO_PinState button_state;

#ifdef GRUPO2_446
	/* Hardware F446: */
	button_state = !HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN);
#else
	/* Hardware F407: */
	button_state = HAL_GPIO_ReadPin(BTN_PORT, BTN_PIN);
#endif

	return button_process_state_(button_state);


#else
/* MODO TESTING (CON ENTRADAS PREESTABLECIDAS DE TIPOS DE BOTONES): ----------
 */

	button_type_t r;

	if(0 == s_btn_len)
		return BUTTON_TYPE_NONE;

	r = s_btn_seq_default[s_btn_idx];

	if((s_btn_idx + 1) < s_btn_len)
	{
		s_btn_idx++;
	}

	return r;


#endif  /*¨not TESTING_SIMULATION_BUTTON_INPUTS */
}












/********************** end of file ******************************************/
