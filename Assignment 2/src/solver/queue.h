#pragma once

#include "../graph/state_graph.h"

/** Representa un nodo en una cola */
struct queue_node;

/** Representa un nodo en una cola */
typedef struct queue_node QueueNode;

/** Representa un nodo en una cola */
struct queue_node
{
	/** El nodo de la tabla que guarda el estado */
	Node* node;
	/** Siguiente elemento en la cola */
	QueueNode* next;
};

/** Representa una cola de estados */
struct queue
{
	/** El primer elemento de la cola, proximo a salir */
	QueueNode* head;
	/** El último elemento de la cola, recien agregado */
	QueueNode* tail;
};

/** Representa una cola de estados */
typedef struct queue Queue;

/** Inicializa la cola */
Queue*    queue_init();
/** Obtiene el siguiente elemento de la cola */
Node*     queue_dequeue(Queue* q);
/** Agrega un elemento a la cola */
void      queue_enqueue(Queue* q, Node* node);
/** Libera todos los nodos de la cola */
void      queue_destroy(Queue* q);
