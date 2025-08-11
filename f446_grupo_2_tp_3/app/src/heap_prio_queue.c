/**
 * @file prio_queue.c
 * @brief Implementación de Cola de Prioridad usando heap binario con asignación dinámica de memoria
 * 
 * Esta librería proporciona una implementación de cola de prioridad max-heap donde los nodos
 * están ordenados por prioridad en orden descendente (prioridad más alta primero).
 * En caso de igual prioridad, se mantiene orden FIFO usando contador de secuencia.
 * 
 * @author cese_rtos2_grupo_2 (feat. Claude Code)
 * @version 1.0
 * @date 10 Aug 2025
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_os.h"
#include "heap_prio_queue.h"

/*===========================================================================*/
/*                            TYPE DEFINITIONS                              */
/*===========================================================================*/

/**
 * @brief Estructura del Nodo que contiene datos y prioridad
 * 
 * Estructura interna no expuesta al usuario. Representa un solo nodo
 * en la cola de prioridad con datos y valor de prioridad para ordenamiento del heap.
 */
typedef struct {
    data_queue_t data;                  /**< Datos del nodo */
    prio_queue_priority_t priority;     /**< Valor de prioridad (enum prio_queue_priority_t, valores más altos = mayor prioridad) */
    uint64_t seq;                       /**< Valor de secuencia (valores menores = mensajes más antiguos) */
} Node;

/**
 * @brief Definición de estructura interna de Cola de Prioridad
 * 
 * Contiene un arreglo heap de punteros a Nodo y el tamaño actual.
 * Esta implementación está oculta del usuario para mejor encapsulación.
 */
struct PriorityQueue {
    Node** heap;        /**< Arreglo de punteros a estructuras Nodo */
    int size;           /**< Número actual de elementos en la cola */
    uint64_t next_seq;  /**< Contador de inserción */
};

/*===========================================================================*/
/*                      INTERNAL FUNCTION DECLARATIONS                      */
/*===========================================================================*/

/**
 * @brief Intercambia dos punteros a Nodo en el heap
 * 
 * Función auxiliar interna para intercambiar posiciones de dos nodos
 * en el arreglo heap. Se usa durante operaciones del heap para mantener
 * la propiedad del heap.
 * 
 * @param a Puntero al primer puntero a Nodo
 * @param b Puntero al segundo puntero a Nodo
 * 
 * @note Esta es una función interna no expuesta en la API pública
 * @note Complejidad temporal: O(1)
 */
static void swap(Node** a, Node** b);

/**
 * @brief Restaura la propiedad max-heap moviendo elemento hacia arriba
 * 
 * Función recursiva interna que compara un nodo con su padre
 * y los intercambia si el hijo tiene mayor prioridad. Continúa hasta
 * que se satisface la propiedad del heap o se alcanza la raíz.
 * 
 * @param pq Puntero a la ColaDePrioridad
 * @param index Índice del elemento para hacer bubble up
 * 
 * @note Se usa después de inserción para mantener el orden del heap
 * @note Complejidad temporal: O(log n) - altura del árbol
 * @note Implementación recursiva
 */
static void bubble_up(PriorityQueue* pq, int index);

/**
 * @brief Restaura la propiedad max-heap moviendo elemento hacia abajo
 * 
 * Función recursiva interna que compara un nodo con sus hijos
 * e intercambia con el hijo que tiene la mayor prioridad. Continúa
 * recursivamente hasta que se satisface la propiedad del heap o se alcanza una hoja.
 * 
 * @param pq Puntero a la ColaDePrioridad
 * @param index Índice del elemento para hacer bubble down
 * 
 * @note Se usa después de extracción para mantener el orden del heap
 * @note Encuentra el más grande entre el nodo y sus hijos
 * @note Complejidad temporal: O(log n) - altura del árbol
 * @note Implementación recursiva
 */
static void bubble_down(PriorityQueue* pq, int index);

/**
 * @brief Determina si un nodo tiene mayor prioridad que otro
 * 
 * Función de comparación que implementa la lógica de prioridad para el heap.
 * Primero compara por prioridad, y en caso de empate, el nodo más antiguo (menor seq) gana.
 * Esto asegura comportamiento FIFO para elementos de igual prioridad.
 * 
 * @param a Puntero al primer nodo a comparar
 * @param b Puntero al segundo nodo a comparar
 * 
 * @return true si el nodo 'a' tiene mayor prioridad que 'b', false en caso contrario
 * 
 * @note Implementa lógica: mayor prioridad gana, en empate el más antiguo gana
 * @note Complejidad temporal: O(1)
 */
static bool node_is_higher(const Node* a, const Node* b);

/*===========================================================================*/
/*                       PUBLIC FUNCTION DEFINITIONS                        */
/*===========================================================================*/

PriorityQueue* create_priority_queue() {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;
    
    // aloca el array fijo de MAX_CAPACITY punteros a NULL
    pq->heap = (Node**)malloc(MAX_CAPACITY * sizeof(Node*));
    if (!pq->heap) {
        free(pq);
        return NULL;
    }
    
    for (int i = 0; i < MAX_CAPACITY; i++) {
        pq->heap[i] = NULL;
    }
    
    pq->size = 0;
    pq->next_seq = 0;
    return pq;
}

void destroy_priority_queue(PriorityQueue* pq) {
    if (!pq) return;
    
    taskENTER_CRITICAL(); {
    
        for (int i = 0; i < pq->size; i++) {
            if (pq->heap[i]) {
                free(pq->heap[i]);
            }
        }
        free(pq->heap);
    
    } taskEXIT_CRITICAL();
    
    free(pq);
}

bool insert_in_prio_queue(PriorityQueue* pq, data_queue_t data, prio_queue_priority_t priority) {
    if (!pq) {
        return false;
    }
    
    taskENTER_CRITICAL(); {
    
#ifdef ENABLE_AUTO_DISCARD
        // Si la cola está llena, descartar el último elemento (nodo mas viejo de la prio mas baja)
        if (pq->size >= MAX_CAPACITY) {
            free(pq->heap[pq->size - 1]);
            pq->heap[pq->size - 1] = NULL;
            pq->size--;
        }
#else
        // Comportamiento default: fallar si la cola está llena
        if (pq->size >= MAX_CAPACITY) {
            taskEXIT_CRITICAL();
            return false;
        }
#endif
        
        // asigna memoria para el nuevo Nodo
        Node* new_node = (Node*)malloc(sizeof(Node));
        if (!new_node) {
            taskEXIT_CRITICAL(); // salir de zona critica antes de salir de la función
            return false;
        }
        
        memcpy(&new_node->data, &data, sizeof(data_queue_t));
        new_node->priority = priority;
        new_node->seq = pq->next_seq++;
        
        pq->heap[pq->size] = new_node;
        
        bubble_up(pq, pq->size);
        pq->size++;
    
    } taskEXIT_CRITICAL();
    
    return true;
}

bool extract_from_prio_queue(PriorityQueue* pq, data_queue_t* data, prio_queue_priority_t* priority) {
    // libera la memoria internamente y devuelve solo los datos
    if (!pq || pq->size == 0 || !data || !priority) {
        return false;
    }
    
    taskENTER_CRITICAL(); {
    
        Node* max_node = pq->heap[0];
        
        *data = max_node->data;
        *priority = max_node->priority;
        
        //  libera la memoria del nodo
        free(max_node);
        
        pq->size--;
        if (pq->size > 0) {
            pq->heap[0] = pq->heap[pq->size];
            // marca la última posición del arreglo como NULL después de extraer
            pq->heap[pq->size] = NULL;
            bubble_down(pq, 0);
        } else {
            pq->heap[0] = NULL;
        }
        
    } taskEXIT_CRITICAL();
    
    return true;
}

bool is_empty(PriorityQueue* pq) {
    return pq ? pq->size == 0 : true;
}

int get_size(PriorityQueue* pq) {
    return pq ? pq->size : 0;
}

/*===========================================================================*/
/*                      INTERNAL FUNCTION DEFINITIONS                       */
/*===========================================================================*/

static void swap(Node** a, Node** b) {
    Node* temp = *a;
    *a = *b;
    *b = temp;
}

static bool node_is_higher(const Node* a, const Node* b) {
    if (a->priority != b->priority) {
        return (a->priority > b->priority);      // bigger priority wins
    }
    return (a->seq < b->seq);                    // tie: older wins (FIFO)
}

static void bubble_up(PriorityQueue* pq, int index) {
    if (index == 0) return;
    
    int parent_index = (index - 1) / 2;
    
    if (node_is_higher(pq->heap[index], pq->heap[parent_index])) {
        swap(&pq->heap[index], &pq->heap[parent_index]);
        bubble_up(pq, parent_index);
    }
}

static void bubble_down(PriorityQueue* pq, int index) {
    int largest = index;
    int left_child = 2 * index + 1;
    int right_child = 2 * index + 2;
    
    if (left_child < pq->size && 
        node_is_higher(pq->heap[left_child], pq->heap[largest])) {
        largest = left_child;
    }
    
    if (right_child < pq->size && 
        node_is_higher(pq->heap[right_child], pq->heap[largest])) {
        largest = right_child;
    }
    
    if (largest != index) {
        swap(&pq->heap[index], &pq->heap[largest]);
        bubble_down(pq, largest);
    }
}
