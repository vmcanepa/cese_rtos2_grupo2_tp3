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
static node_t * queue_head; 	// elemento de max prioridad de la cola
static node_t * queue_tail; // elemento de min prioridad de la cola

/********************** internal functions declaration ***********************/
static node_t * find_pos_in_queue_(uint8_t priority);
static void insert_ordered_node_(node_t * new_node);

/********************** external functions definition ************************/
bool prio_queue_init() {

	if(queue_initialized)
		return false;

	if(1 >= MAX_QUEUE_LENGTH_)	// el algoritmo funciona con colas de tamaño mayor a 1
		return false;
	queue_head = NULL;
	queue_tail = NULL;
	queue_count = 0;
	queue_initialized = true;
	return queue_initialized;
}

bool prio_queue_insert(data_queue_t data, prio_queue_priority_t priority) {

	if(!queue_initialized)
		return false;

	taskENTER_CRITICAL(); { // protejo la escritura y ordenamiento para no romper la queue
		// elimino el último elemento de la cola para dejar luagar porque ya no queda espacio
		if(MAX_QUEUE_LENGTH_ <= queue_count) {
			queue_tail = queue_tail->prev;
			free(queue_tail->next);					// libero la memoria del último (borrado)
			queue_tail->next = NULL;
			queue_count--;
		}

		// alocar la memoria para el nodo nuevo y completar con los datos
		node_t* nuevo_nodo = (node_t*)malloc(sizeof(node_t));
		if(NULL == nuevo_nodo)
			return false;
		memcpy(&nuevo_nodo->data, &data, sizeof(data_queue_t));
		nuevo_nodo->priority = priority;
		nuevo_nodo->prev = NULL;
		nuevo_nodo->next = NULL;

		// inserta ordenado por prioridad
		insert_ordered_node_(nuevo_nodo);

		queue_count++;
	} taskEXIT_CRITICAL();
	return true;
}

bool prio_queue_extract(data_queue_t * data, prio_queue_priority_t * priority) {

	node_t nodo = {0};

	if(!queue_initialized || NULL == queue_head)	// no hay nada en la cola o no está inicializado
		return false;

	taskENTER_CRITICAL(); { // protejo la lectura y ordenamiento para no romper la queue

		// obtengo la información de debo devolver
		*data = queue_head->data;
		*priority = queue_head->priority;

		if(queue_head == queue_tail) {	// es el último de la cola

			free(queue_head);					// elimino el elemento
			queue_count = 0;				// reseteo todas las varibles
			queue_head = NULL;
			queue_tail = NULL;

		} else {

			queue_head = queue_head->next;	// apunto el elemento de salida al proximo
			free(queue_head->prev);			// elimino el elemento
			queue_head->prev = NULL;			// como es el primero no hay anterior
			queue_count--;
		}
	} taskEXIT_CRITICAL();

	return true;
}

/********************** internal functions definition ************************/
static node_t * find_pos_in_queue_(prio_queue_priority_t priority) {

	node_t* nodo_actual = queue_head; // empieza a buscar desde la max prioridad hacia la menor

	while(NULL != nodo_actual) {

		if(nodo_actual->priority < priority)
			return nodo_actual;					// lugar donde voy a insertar el nodo
		nodo_actual = nodo_actual->next;
	}
	return NULL;							// no hay nadie con menor prioridad
}

static void insert_ordered_node_(node_t * nuevo_nodo) {

    if(0 == queue_count) {			// caso inicial, no hay nada guardado en la cola

		queue_head = nuevo_nodo;			// queda como head de la cola
		queue_tail = nuevo_nodo;			// y tambien como tail
		return;
    }

    // busco un lugar para guardar según la prioridad
	node_t* nodo_siguiente = find_pos_in_queue_(nuevo_nodo->priority);

	if(NULL == nodo_siguiente) {	// si no hay siguiente, el nuevo es el de menor prioridad de la cola, va al fondo
		nuevo_nodo->prev = queue_tail;
		queue_tail->next = nuevo_nodo;
		queue_tail = nuevo_nodo;
		return;
	}

	if(NULL == nodo_siguiente->prev) {			// este es el caso de que lo debo almacenar al comienzo de la cola

		nuevo_nodo->next = nodo_siguiente;			// apunto al que antes era el primero
		nodo_siguiente->prev = nuevo_nodo;			// el que antes era primero ahora es segundo asi que apunto al nuevo primero como predecesor
		queue_head = nuevo_nodo;			// es la nueva cabecera de la cola
		return;
	}
	// por último cuando lo almaceno entre otros elementos
	node_t* nodo_anterior = nodo_siguiente->prev;	// obtengo la direccion del elemento anterior en la cola
	nuevo_nodo->next = nodo_siguiente;
	nuevo_nodo->prev = nodo_anterior;
	nodo_siguiente->prev = nuevo_nodo;
	nodo_anterior->next = nuevo_nodo;
	return;
}
