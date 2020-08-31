#include "queue.h"
#include <stdlib.h>

/** Inicializa la cola */
Queue* queue_init()
{
	Queue* q = malloc(sizeof(Queue));

	q -> head = NULL;
	q -> tail = NULL;

	return q;
}

/** Agrega un elemento a la cola */
void queue_enqueue(Queue* q, Node* node)
{
	/* Si no hay ultimo, tampoco hay primero */
	if(q -> tail == NULL)
	{
		q -> head = malloc(sizeof(QueueNode));
		q -> tail = q -> head;

		q -> head -> node = node;
		q -> head -> next = NULL;
	}
	else
	{
		q -> tail -> next = malloc(sizeof(QueueNode));
		q -> tail -> next -> next = NULL;
		q -> tail -> next -> node = node;

		q -> tail = q -> tail -> next;
	}
}

/** Obtiene el siguiente elemento de la cola */
Node* queue_dequeue(Queue* q)
{
	Node* ret = q -> head -> node;
	QueueNode* node = q -> head;
	q -> head = q -> head -> next;
	free(node);

	if(!q -> head)
	{
		q -> tail = NULL;
	}
	return ret;
}

/** Libera todos los nodos de la cola */
void queue_destroy(Queue* q)
{
	while(q -> head)
	{
		QueueNode* node = q -> head;
		q -> head = q -> head -> next;
		free(node);
	}
	free(q);
}
