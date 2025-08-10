/*
 *
 * @author : cese_rtos2_grupo_2
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
#include "priority_queue.h"

/********************** macros and definitions *******************************/
#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(msg_event_t*))

typedef enum {

	UI_STATE_STANDBY,
	UI_STATE_RED,
	UI_STATE_GREEN,
	UI_STATE_BLUE,
	UI_STATE__N,
} ui_state_t;

/********************** internal data definition *****************************/
static bool ui_running;
static QueueHandle_t hqueue;

/********************** internal functions declaration ***********************/
static void task_ui(void *argument);
static void ao_ui_delete(void);
static void ao_ui_queue_delete(void);
/********************** internal functions definition ************************/
static void task_ui(void *argument) {

	ao_led_init();
	prio_queue_init();

	while(true) {

		msg_event_t msg;

		if(pdPASS == xQueueReceive(hqueue, &msg, 1000)) {

			data_queue_t ao_led_msg;
			ao_led_msg.action = AO_LED_MESSAGE_ON;

			switch(msg) {

				case MSG_EVENT_BUTTON_PULSE:
					ao_led_msg.color = AO_LED_COLOR_RED;
					if(prio_queue_insert(ao_led_msg, PRIO_QUEUE_PRIORITY_HIGH)){
						LOGGER_INFO("[UI] Insert High");
					}
					break;
				case MSG_EVENT_BUTTON_SHORT:
					ao_led_msg.color = AO_LED_COLOR_GREEN;
					if(prio_queue_insert(ao_led_msg, PRIO_QUEUE_PRIORITY_MEDIUM)){
						LOGGER_INFO("[UI] Insert Medium");
					}
					break;
				case MSG_EVENT_BUTTON_LONG:
					ao_led_msg.color = AO_LED_COLOR_BLUE;
					if(prio_queue_insert(ao_led_msg, PRIO_QUEUE_PRIORITY_LOW)){
						LOGGER_INFO("[UI] Insert Low");
					}
					break;
				default:
					break;
			}
		} else {
			// la tarea UI debe vivir mientras hay mensajes sin procesar en la cola ui
			// cuando se acaban los mensajes encolados, se suicida
			ao_ui_delete();
		}
	}
}

static void ao_ui_delete(void) {

	taskENTER_CRITICAL(); { // seccion critica para que nadie mande mensajes mientras elimino
		ui_running = false;
		vQueueDelete(hqueue);
		hqueue = NULL;
	} taskEXIT_CRITICAL();
	LOGGER_INFO("[UI] Elimina tarea UI y su cola");
	vTaskDelete(NULL);
}

/********************** external functions definition ************************/
bool ao_ui_init(void) {
	bool init_status = false;
	// agrego logica para que se cree la tarea solo si no hay una corriendo
	taskENTER_CRITICAL();
		if(!ui_running) {

			hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
			if(NULL != hqueue) {
				BaseType_t status;
				status = xTaskCreate(task_ui, "task_ao_ui", 128, NULL, tskIDLE_PRIORITY, NULL);
				if(pdPASS != status) {  // si falla alocacion de mem para la tarea, elimino la queue tambien
					vQueueDelete(hqueue);
					hqueue = NULL;
				} else {
					init_status = true;
					ui_running = true;
				}
		}
	} taskEXIT_CRITICAL();
	if(true == init_status){
		LOGGER_INFO("[UI] Crea tarea UI");
	} else {
		LOGGER_INFO("[UI] Error! Falla creación de tarea. Abortando init de UI.");
	}

	return init_status;
}

bool ao_ui_send_event(msg_event_t msg) {

	BaseType_t status = xQueueSend(hqueue, &msg, 0);
	while(pdPASS != status){
		LOGGER_INFO("[UI] Cola llena: descartando evento antiguo.");
		msg_event_t aux;
		xQueueReceive(hqueue, &aux, 0);
		status = xQueueSend(hqueue, &msg, 0);
	}

	LOGGER_INFO("[UI] Evento enviado: %d", msg);

	return (status == pdPASS);
}

/********************** end of file ******************************************/
