/**
 * @file prio_queue.h
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

#ifndef PRIO_QUEUE_H
#define PRIO_QUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include "ao_led.h"

/**
 * @brief Capacidad máxima de la cola de prioridad
 * 
 * Esta macro define el número máximo de elementos que pueden almacenarse
 * en la cola de prioridad. Puede modificarse según las necesidades de implementación.
 */
#define MAX_CAPACITY 10

/**
 * @brief Habilita descarte automático cuando la cola está llena
 * 
 * Si está definida, insert_in_prio_queue() descartará automáticamente
 * el último elemento cuando la cola esté llena para hacer espacio al nuevo.
 * Si no está definida, insert_in_prio_queue() fallará cuando la cola esté llena.
 */
// #define ENABLE_AUTO_DISCARD

/*===========================================================================*/
/*                            TYPE DEFINITIONS                              */
/*===========================================================================*/

/**
 * @brief Enumeración de niveles de prioridad para la cola
 * 
 * Define los diferentes niveles de prioridad disponibles para los elementos
 * de la cola. Los valores más altos representan mayor prioridad.
 */
typedef enum {
  PRIO_QUEUE_PRIORITY_LOW,      
  PRIO_QUEUE_PRIORITY_MEDIUM,   
  PRIO_QUEUE_PRIORITY_HIGH      
} prio_queue_priority_t;

/**
 * @brief Estructura opaca de Cola de Prioridad
 * 
 * Los detalles de implementación están ocultos del usuario para mejor encapsulación.
 * Usa implementación de heap binario con indexación de arreglo donde
 * para nodo en índice i: hijo izquierdo = 2i+1, hijo derecho = 2i+2
 */
typedef struct PriorityQueue PriorityQueue;

/*===========================================================================*/
/*                         PUBLIC FUNCTION DECLARATIONS                     */
/*=========================================================================== */

/*---------------------------------------------------------------------------*/
/*                       Creation and Destruction                           */
/*---------------------------------------------------------------------------*/

/**
 * @brief Crea e inicializa una nueva cola de prioridad
 * 
 * Asigna memoria para la estructura de cola de prioridad e inicializa
 * un arreglo de MAX_CAPACITY punteros a Nodo, todos asignados a NULL.
 * 
 * @return Puntero a ColaDePrioridad recién creada, o NULL si falla la asignación
 * 
 * @note Complejidad temporal: O(1)
 * @note El llamador es responsable de llamar destroy_priority_queue() para liberar memoria
 */
PriorityQueue* create_priority_queue(void);

/**
 * @brief Destruye la cola de prioridad y libera toda la memoria asignada
 * 
 * Libera todas las estructuras Nodo individuales, el arreglo heap, y la
 * estructura ColaDePrioridad misma.
 * 
 * @param pq Puntero a la ColaDePrioridad a destruir
 * 
 * @note Complejidad temporal: O(n) - debe liberar todos los nodos
 * @note Después de llamar esta función, el puntero pq se vuelve inválido
 * @note Operación thread-safe protegida con sección crítica
 */
void destroy_priority_queue(PriorityQueue* pq);

/*---------------------------------------------------------------------------*/
/*                           Queue Operations                               */
/*---------------------------------------------------------------------------*/

/**
 * @brief Inserta un nuevo elemento en la cola de prioridad
 * 
 * Crea un nuevo Nodo con los datos y prioridad dados, asigna un número de secuencia,
 * lo inserta en el heap, y mantiene el orden del heap usando operación bubble-up.
 * 
 * @param pq Puntero a la ColaDePrioridad
 * @param data Datos a almacenar (data_queue_t)
 * @param priority Valor de prioridad (prio_queue_priority_t)
 * 
 * @return true si la inserción fue exitosa, false si la cola está llena o falló la asignación
 * 
 * @note Complejidad temporal: O(log n)
 * @note Se asigna memoria para cada nuevo nodo individualmente
 * @note Se asigna contador de secuencia para mantener orden FIFO en empates de prioridad
 * @note Operación thread-safe protegida con sección crítica
 */
bool insert_in_prio_queue(PriorityQueue* pq, data_queue_t data, prio_queue_priority_t priority);

/**
 * @brief Extrae el elemento de mayor prioridad de la cola
 * 
 * Remueve el elemento raíz (mayor prioridad), copia sus datos y prioridad
 * a los parámetros de salida, libera la memoria del nodo, y restaura el orden del heap
 * usando operación bubble-down.
 * 
 * @param pq Puntero a la ColaDePrioridad
 * @param data Puntero para almacenar los datos extraídos 
 * @param priority Puntero para almacenar la prioridad extraída 
 * 
 * @return true si la extracción fue exitosa, false si la cola está vacía o los parámetros son NULL
 * 
 * @note Complejidad temporal: O(log n)
 * @note La memoria del nodo extraído se libera automáticamente
 * @note Esta función proporciona encapsulación - el llamador no accede memoria interna directamente
 * @note Operación thread-safe protegida con sección crítica
 */
bool extract_from_prio_queue(PriorityQueue* pq, data_queue_t* data, prio_queue_priority_t* priority);

/*---------------------------------------------------------------------------*/
/*                           Query Functions                                */
/*---------------------------------------------------------------------------*/

/**
 * @brief Verifica si la cola de prioridad está vacía
 * 
 * @param pq Puntero a la ColaDePrioridad
 * 
 * @return true si la cola está vacía o pq es NULL, false en caso contrario
 * 
 * @note Complejidad temporal: O(1)
 */
bool is_empty(PriorityQueue* pq);

/**
 * @brief Obtiene el número actual de elementos en la cola
 * 
 * @param pq Puntero a la ColaDePrioridad
 * 
 * @return Número de elementos actualmente en la cola, 0 si pq es NULL
 * 
 * @note Complejidad temporal: O(1)
 */
int get_size(PriorityQueue* pq);

#endif