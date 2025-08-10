/*
 * ao_led.h
 *
 *  Created on: Aug 2, 2025
 *      Author: grupo 2 RTOS II
 */

#ifndef INC_AO_LED_H_
#define INC_AO_LED_H_

//#include "ao_ui.h"

/********************** inclusions *******************************************/

/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef enum {

  AO_LED_MESSAGE_ON,
  AO_LED_MESSAGE_OFF,
  AO_LED_MESSAGE__N,
} ao_led_action_t;

typedef enum {

  AO_LED_COLOR_RED,
  AO_LED_COLOR_GREEN,
  AO_LED_COLOR_BLUE,
} ao_led_color_t;

typedef struct {

	ao_led_action_t action;
	ao_led_color_t color;
} data_queue_t;


/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
bool ao_led_init();


#endif /* INC_AO_LED_H_ */
