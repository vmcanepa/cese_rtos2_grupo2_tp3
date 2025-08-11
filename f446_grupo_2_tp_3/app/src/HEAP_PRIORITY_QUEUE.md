# Prompts usados para la implementacion de heap_heap_prio_queue

Se trabajo con el CLI de [Claude Code](https://docs.anthropic.com/en/docs/claude-code/overview) y al finalizar se le solicito el listado de requerimientos entregados a lo largo de la sesión para crear esta documentación.

`Aquí tienes el listado de requerimientos que me fuiste dando para esta implementación:`

##  Requerimientos Iniciales

  1. Lenguaje: Código en C (archivo heap_prio_queue.c)
  2. Estructura: Priority queue con estructuras heap
  3. Ordenamiento: Nodos ordenados por prioridad descendente (9 a 0 por ejemplo)
  4. Implementación: Binary heaps posicionados en un array usando punteros
  5. Array: Contiene índice y puntero al nodo
  6. Estructura Node: Campo uint8_t para data del mensaje y uint8_t para prioridad
  7. Indexación: Nodo índice i → hijos en 2i+1 y 2i+2 (para bubble up/bubble down)

##  Métodos Requeridos

  - create: Crea la cola de prioridad
  - insert: Inserta un nodo y lo ordena
  - extract: Remueve y devuelve el nodo de mayor prioridad

##  Modificaciones Posteriores

  8. Gestión de memoria: Asignar memoria para cada nodo cuando se necesita (no toda de una vez)
  9. Liberación: Liberar memoria cuando se extrae un nodo
  10. Capacidad fija: Usar MAX_CAPACITY como límite máximo que no se puede superar
  11. Array fijo: Array de punteros a Node (no realocación constante)
  12. Encapsulación: extract() debe liberar memoria internamente y devolver datos por parámetros de salida
  13. Estructura de librería: Separar en heap_prio_queue.h y heap_prio_queue.c 
  14. Documentación: Agregar documentación Doxygen al header
  15. Ocultar el struct Node del usuario: Mover la definición de Node del header público al archivo de implementación para mejorar el encapsulamiento
  16. Convertir PriorityQueue en tipo opaco: Usar forward declaration en el header y definir la estructura completa solo en el archivo .c
  17. Agregar calificativo static: Aplicar static a todas las funciones internas (swap, bubble_up, bubble_down) para restringir su visibilidad al archivo de implementación
  18. Documentación Doxygen completa: Agregar documentación tipo Doxygen para todas las funciones internas no declaradas en el header
  19. Organización del código: Reorganizar tanto heap_prio_queue.c como heap_prio_queue.h con secciones definidas
  20. Usar macro ENABLE_AUTO_DISCARD para controlar comportamiento de descarte: Agregar descarte automático cuando priority queue esté llena eliminando el item mas antiguo de la prioridad mas baja.

