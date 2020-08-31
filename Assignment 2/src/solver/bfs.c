#include "bfs.h"
#include "queue.h"

/** Retorna un estado vacío */
State state_init_empty()
{
	State state = calloc(height, sizeof(uint8_t*));

	for(uint8_t row = 0; row < height; row++)
	{
		state[row] = calloc(width, sizeof(uint8_t));
	}

	return state;
}

/** Computa el hash de la solución, para poder verificar eficientemente */
uint64_t get_solution_hash(Graph* graph)
{
	/* Inicializa un estado con	la configuración de la solución */
	State solution = calloc(height, sizeof(uint8_t*));

	for(uint8_t row = 0; row < height; row++)
	{
		solution[row] = calloc(width, sizeof(uint8_t));
		for(uint8_t col = 0; col < width; col++)
		{
			solution[row][col] = row * width + col;
		}
	}

	/* Obtenemos el hash de la solución */
	uint64_t solution_hash = state_hash(graph, solution);

	/* Liberamos la memoria de este estado temporal */
	state_destroy(solution);

	return solution_hash;
}


/** Busca la ruta al nodo solución usando BFS. Retorna NULL si no existe */
Node* solve(State start, Graph* graph)
{
	/* Computamos el hash de la solución */
	uint64_t solution_hash = get_solution_hash(graph);

	/* Inicializamos una cola vacía */
	Queue* queue = queue_init();

	/* Creamos un nodo para el estado inicial. No tiene padre */ 
	Node* start_node = state_graph_new_node(graph, start, NULL, 0);

	/* Creamos un contenedor de estado en el que iremos guardando los hijos */
	State son_container = state_init_empty();

	/* Metemos este nodo a la cola */
	queue_enqueue(queue, start_node);

	/* Si aún hay nodos sin revisar en la cola */
	while(queue -> head)
	{
		/* Obtiene el siguiente nodo a revisar */
		Node* current = queue_dequeue(queue);

		/* Si es solución, estamos listos. Comparamos el hash primero ya que es O(1) */
		if(current -> hash_value == solution_hash && state_is_solution(current -> state))
		{
			queue_destroy(queue);
			state_destroy(son_container);
			return current;
		}

		/* Si no, se engendran sus hijos */
		for(int i = 0; i < op_count; i++)
		{
			/* Obtenemos el estado hijo */
			state_next(current -> state, operations[i], son_container);

			/* Creamos un nodo en el grafo para este estado */
			Node* son_node = state_graph_new_node(graph, son_container, current, operations[i]);

			/* Si efectivamente se creó el nodo, significa que el estado era nuevo */
			if(son_node)
			{
				/* Se ingresa a la cola */
				queue_enqueue(queue, son_node);

				/* Ahora que este contenedor está guardado en la tabla, creamos uno nuevo */
				son_container = state_init_empty();
			}
		}
	}
	queue_destroy(queue);
	state_destroy(son_container);
	return NULL;
}
