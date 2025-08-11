/*
 * priority_queue.c
 *
 *  Created on: Aug 9, 2025
 *      Author: cese_rtos2_grupo_2
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os.h"

#include "priority_queue.h"

/********************** macros and definitions *******************************/
#define MAX_QUEUE_LENGTH_            (10)

typedef struct node_t {

	data_queue_t data;
	struct node_t * next;
	struct node_t * prev;
	uint8_t priority;
}node_t;

/********************** internal data definition *****************************/
static bool queue_initialized = false;
static uint16_t queue_count;
static node_t * queue_head;				// elemento de max prioridad de la cola
static node_t * queue_tail;				// elemento de min prioridad de la cola
static node_t * queue_high_prio;
static node_t * queue_medium_prio;

/********************** internal functions declaration ***********************/
static node_t * find_pos_in_queue_(node_t * new_node);
static void insert_ordered_node_(node_t * new_node);
static void delete_rear_node(void);
static void delete_head_node(void);

/********************** external functions definition ************************/
bool prio_queue_init() {

	if(queue_initialized)
		return false;

	if(1 >= MAX_QUEUE_LENGTH_)
		return false;
	queue_head = NULL;
	queue_tail = NULL;
	queue_high_prio = NULL;
	queue_medium_prio = NULL;
	queue_count = 0;
	queue_initialized = true;
	return queue_initialized;
}

bool prio_queue_insert(data_queue_t data, prio_queue_priority_t priority) {

	if(!queue_initialized)
		return false;

	taskENTER_CRITICAL(); { 					// protejo la escritura y ordenamiento para no romper la queue

		node_t* nuevo_nodo = (node_t*)malloc(sizeof(node_t));

		if(NULL == nuevo_nodo)
			return false;

		if(MAX_QUEUE_LENGTH_ <= queue_count)
			delete_rear_node();
		memcpy(&nuevo_nodo->data, &data, sizeof(data_queue_t));
		nuevo_nodo->priority = priority;
		nuevo_nodo->prev = NULL;
		nuevo_nodo->next = NULL;
		insert_ordered_node_(nuevo_nodo);
		queue_count++;
	} taskEXIT_CRITICAL();
	return true;
}

bool prio_queue_extract(data_queue_t * data, prio_queue_priority_t * priority) {

	if(!queue_initialized || NULL == queue_head)
		return false;

	if(NULL == data || NULL == priority)
		return false;

	taskENTER_CRITICAL(); {				// protejo la lectura y ordenamiento para no romper la queue

		*data = queue_head->data;
		*priority = queue_head->priority;
		delete_head_node();
	} taskEXIT_CRITICAL();
	return true;
}

/********************** internal functions definition ************************/
static node_t * find_pos_in_queue_(node_t * new_node) {

	node_t * temp;

	if(PRIO_QUEUE_PRIORITY_HIGH == new_node->priority) {

		temp = queue_high_prio;
		queue_high_prio = new_node;
		return temp;
	}

	if(PRIO_QUEUE_PRIORITY_LOW == new_node->priority) {

		temp = queue_medium_prio;
		queue_medium_prio = new_node;
		return temp;
	}
	return NULL;
}

static void insert_ordered_node_(node_t * nuevo_nodo) {

    if(0 == queue_count) {

		queue_head = nuevo_nodo;
		queue_tail = nuevo_nodo;
		return;
    }
	node_t* nodo_siguiente = find_pos_in_queue_(nuevo_nodo);

	if(NULL == nodo_siguiente) {

		nuevo_nodo->prev = queue_tail;
		queue_tail->next = nuevo_nodo;
		queue_tail = nuevo_nodo;
		return;
	}

	if(NULL == nodo_siguiente->prev) {

		nuevo_nodo->next = nodo_siguiente;
		nodo_siguiente->prev = nuevo_nodo;
		queue_head = nuevo_nodo;
		return;
	}
	node_t* nodo_anterior = nodo_siguiente->prev;
	nuevo_nodo->next = nodo_siguiente;
	nuevo_nodo->prev = nodo_anterior;
	nodo_siguiente->prev = nuevo_nodo;
	nodo_anterior->next = nuevo_nodo;
	return;
}

static void delete_rear_node(void) {

	if(queue_tail == queue_high_prio) {

		queue_high_prio = queue_high_prio->prev;

		if(queue_high_prio && queue_high_prio->priority != queue_tail->priority)
			queue_high_prio = NULL;
	} else if(queue_tail == queue_medium_prio) {

		queue_medium_prio = queue_medium_prio->prev;

		if(queue_medium_prio && queue_medium_prio->priority != queue_tail->priority)
			queue_medium_prio = NULL;
	}
	queue_tail = queue_tail->prev;
	free(queue_tail->next);
	queue_tail->next = NULL;
	queue_count--;
}

static void delete_head_node(void) {

	if(queue_head == queue_high_prio) {

		queue_high_prio = queue_high_prio->next;

		if(queue_high_prio && queue_high_prio->priority != queue_head->priority)
			queue_high_prio = NULL;
	} else if(queue_head == queue_medium_prio) {

		queue_medium_prio = queue_medium_prio->next;

		if(queue_medium_prio && queue_medium_prio->priority != queue_head->priority)
			queue_medium_prio = NULL;
	}

	if(queue_head == queue_tail) {

		free(queue_head);
		queue_head = NULL;
		queue_tail = NULL;
		queue_high_prio = NULL;
		queue_medium_prio = NULL;
		queue_count = 0;
	} else {

		queue_head = queue_head->next;
		free(queue_head->prev);
		queue_head->prev = NULL;
		queue_count--;
	}
}
