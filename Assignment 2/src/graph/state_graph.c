#include "state_graph.h"
#include <stdio.h>
#include <math.h>

#include "../random/random.h"

/****************************************************************************/
/*                              Estadísticas                                */
/****************************************************************************/

/* Cantidad total de colisiones */
uint64_t total_collisions;

/* Histograma de colisiones: donde la función de hash tiene una colision */
uint64_t* hash_histogram;

/* Histograma de colisiones: donde hay colisión debido al probing */
uint64_t* probing_histogram;

/* Cantidad de rehashings */
uint64_t resize_count;

/* Cantidad de veces que intentamos ver si un elemento existe */
uint64_t total_state_count;

/** Guarda los datos del histograma en un archivo */
void histograms_save_and_destroy(Graph* graph)
{
	if(histograms_file)
	{
		fprintf(histograms_file, "[");
		for(int i = 0; i < graph -> table_size; i++)
		{
			fprintf(
				histograms_file,
				"[%lu,%lu]",
				probing_histogram[i],
				hash_histogram[i]
			);
			if(i < graph ->table_size - 1)
			{
				fprintf(histograms_file, ",");
			}
		}
		fprintf(histograms_file, "],\n");
	}
	free(probing_histogram);
	free(hash_histogram);
}

/** Guardamos todas las estadísticas en sus respectivos archivos */
void statistics_save(Graph* graph)
{
	histograms_save_and_destroy(graph);

	if(general_stats_file)
	{
		fprintf(general_stats_file, "%lu,", total_state_count);
		fprintf(general_stats_file, "%d,", graph -> state_count);
		fprintf(general_stats_file, "%lu,", total_collisions);
		fprintf(general_stats_file, "%lu,", resize_count);
	}
}

/****************************************************************************/
/*                           Lógica de la tabla                             */
/****************************************************************************/

uint64_t fast_modulus(uint64_t n, int m){
	return n >= m ? n % m : n;
	
}

/** Obtiene el hash de un estado mediante zobrist */
uint64_t state_hash(Graph* graph, State state)
{
	uint64_t hash_value = 0;

	/* Hash en O(n) , con n la cantidad de casillas */
	for (uint8_t row = 0; row < height; row++)
	{
		for (uint8_t col = 0; col < width; col++)
		{
			hash_value^= graph->zobrist[row][col][state[row][col]];
			
		}
	}
	return hash_value;
}

uint64_t incremental_state_hash(Graph* graph, State state, State prev_state, Operation op, uint64_t prev_hash){
	uint64_t hash_value = prev_hash;
	uint8_t index = operation_index(op);
	if(operation_is_row(op)){
		for (uint8_t i = 0; i < width; i++)
		{

			hash_value^=graph->zobrist[index][i][prev_state[index][i]];
			hash_value^=graph->zobrist[index][i][state[index][i]];
		}
		
	}
	else
	{

		for (uint8_t j = 0; j < height; j++)
		{
			hash_value^=graph->zobrist[j][index][prev_state[j][index]];
			hash_value^=graph->zobrist[j][index][state[j][index]];
		}
	}
	return hash_value;
}



/** Ajusta el hash al tamaño de la tabla */
static uint64_t hash_table_adjust_hash(Graph* graph, uint64_t hash)
{
	/* Hacemos módulo, asi si el hash se sale de la tabla se da la vuelta */
	return fast_modulus(hash, graph -> table_size);
}

/** Obtiene la siguiente celda a revisar dado el indice de la celda */
static uint64_t hash_table_next_cell(Graph* graph, uint64_t index, int contador, int c1, int c2)
{
	/* Avanzamos el indice en 1, dandonos la vuelta si llegamos al final */
	return fast_modulus((index + contador*c1+c2*contador*contador), graph -> table_size);
}

 /** Indica si la tabla deberia hacer rehashing */ 
 static bool hash_table_should_rehash(Graph* graph) 
 { 
 	/* Hacemos rehashing si la tabla está llena */ 
 	double cond; 
 	cond = ((double) graph ->state_count)/((double) graph ->table_size); 
 	if (cond < 0.8) 
 	{ 
 		return false; 
 	} 
 	else 
 	{ 
 		return true; 
 	} 
 } 

/****************************************************************************/
/*                          Funciones de la tabla                           */
/****************************************************************************/

/** Entrega true si las celdas corresponden al mismo estado */
static bool state_node_equals(Node* a, Node* b)
{
	/* Si los hashes son distintos, entonces no hay forma de que sean iguales */
	if(a -> hash_value != b -> hash_value)
	{
		return false;
	}
	/* Si los hashes son iguales */
	else
	{
		/* Comparamos los estados manualmente, ya que igual pueden ser distintos */
		return state_equals(a -> state, b -> state);
	}
}

/** Inicializa un grafo vacío */
Graph* state_graph_init()
{
	Graph* graph = malloc(sizeof(Graph));

	/* Nuestra tabla tendra un tamaño inicial de 7, una potencia de 2, -1 */
	graph -> table_size = 7;

	/* Calloc = inicializa el arreglo en 0. Eso significa que todo parte como NULL */
	graph -> hash_table = calloc(graph -> table_size, sizeof(Node*));

	/* Inicialmente no tenemos ningun estado */
	graph -> state_count = 0;

	/* Inicializa matriz de hash zobrist*/
	Zobrist zobrist = calloc(height, sizeof(uint64_t**));
	for (uint8_t row = 0; row < height; row++)
	{
		zobrist[row] = calloc(width, sizeof(uint64_t*));
		for (uint8_t col = 0; col < width; col++)
		{
			zobrist[row][col] = calloc(height*width, sizeof(uint64_t));
			for (uint16_t n = 0; n < height*width; n++)
			{
				zobrist[row][col][n] =  get_random();
			}
		}
	}


	graph -> zobrist = zobrist;



	/* Inicializamos las variables para generar métricas */
	total_collisions = 0;
	resize_count = 0;
	total_state_count = 0;
	probing_histogram = calloc(graph -> table_size, sizeof(uint64_t));
	hash_histogram = calloc(graph -> table_size, sizeof(uint64_t));

	return graph;
}

/** Hace crecer la tabla y traspasa todos los elementos */
static void hash_table_rehash(Graph* graph)
{
	/* Guardamos el histograma en el archivo */
	histograms_save_and_destroy(graph);

	/* Contamos que se hizo un resize más */
	resize_count++;

	/* El tamaño de la tabla antes del rehash */
	int old_size = graph -> table_size;

	/* La tabla antes del rehash */
	Node** old_table = graph -> hash_table;

	/* Aumentamos el tamaño de la tabla a la siguiente potencia de 2, -1 */
	graph -> table_size = 2 * (graph -> table_size + 1) - 1;

	/* Inicializamos la nueva tabla y el contador de colisiones */
	graph -> hash_table = calloc(graph -> table_size, sizeof(Node*));
	probing_histogram = calloc(graph -> table_size, sizeof(uint64_t));
	hash_histogram = calloc(graph -> table_size, sizeof(uint64_t));

	/* Recorremos toda la tabla antigua */
	for(int i = 0; i < old_size; i++)
	{
		Node* node = old_table[i];

		/* Cantidad de nuevas colisiones que se han generado en este paso */
		uint64_t new_collisions = 0;

		/* Solo nos interesa si tiene algo guardado, es decir no es NULL */
		if(node)
		{
			/* Indice en el que cae el nodo en la tabla */
			uint64_t index = hash_table_adjust_hash(graph, node -> hash_value);
			uint64_t index_inicial = index;
			int contador = 1;
			/* Si la celda en la nueva tabla está ocupada */
			while(graph -> hash_table[index])
			{
				/* Avanza a la siguiente celda segun la regla establecida */
				index = hash_table_next_cell(graph, index_inicial, contador, 1, 1);
				contador++;
				/* Si es nuestra primera colision */
				if(new_collisions == 0)
				{
					/* Contamos que hubo una colisión en esa celda  debido al hash */
					hash_histogram[index]++;
				}
				else
				{
					/* Contamos que hubo una colisión en esa celda debido al probing */
					probing_histogram[index]++;
				}

				/* Contamos que tenemos una colisión más en este paso */
				new_collisions++;
			}

			/* Finalmente, ingresa el dato en la celda establecida */
			graph -> hash_table[index] = node;

			/* Agregamos las colisiones al contador total */
			total_collisions += new_collisions;
		}
	}

	free(old_table);
}

/** Crea un nuevo nodo para un estado, pero solo si ese estado no existía */
Node*  state_graph_new_node (Graph* graph, State state, Node* parent, Operation op)
{
	/* Contamos que hemos intentado insertar un nuevo elemento */
	total_state_count++;

	/* Obtiene el hash del estado */
	uint64_t hash;
	if (parent)
	{
		hash = incremental_state_hash(graph, state, parent->state, op, parent->hash_value);
	}
	else
	{
		hash = state_hash(graph, state);
	}
	
	/* Juntamos la información que contendrá el nuevo nodo */
	Node protonode = (Node)
	{
		.state = state,
		.hash_value = hash,
		.parent = parent,
		.op = op,
	};

	/* Indice en el que cae el nodo en la tabla */
	uint64_t index = hash_table_adjust_hash(graph, protonode.hash_value);
	uint64_t index_inicial = index;
	int contador = 1;

	/* Cantidad de nuevas colisiones que se han generado en este paso */
	uint64_t new_collisions = 0;

	/* Si la celda ya está ocupada */
	while(graph -> hash_table[index])
	{
		/* Y además está usada por este mismo estado */
		if(state_node_equals(&protonode, graph -> hash_table[index]))
		{
			/* Detenemos la creación de un nuevo nodo */
			return NULL;
		}
		/* Pasamos a la siguiente celda a revisar */
		index = hash_table_next_cell(graph, index_inicial, contador, 1, 1);
		contador++;


		/* Si es nuestra primera colision */
		if(new_collisions == 0)
		{
			/* Contamos que hubo una colisión en esa celda  debido al hash */
			hash_histogram[index]++;
		}
		else
		{
			/* Contamos que hubo una colisión en esa celda debido al probing */
			probing_histogram[index]++;
		}
		/* Contamos que tenemos una colisión más en este paso */
		new_collisions++;
	}

	/* Agregamos las colisiones al contador total */
	total_collisions += new_collisions;

	/* Ya sabemos que el estado no está asique creamos un nuevo nodo para él */
	Node* new_node = malloc(sizeof(Node));
	/* Guardamos en este nuevo nodo la información necesaria */
	*new_node = protonode;
	/* Guardamos este nuevo nodo en la tabla*/
	graph -> hash_table[index] = new_node;

	/* Indicamos que ahora hay un estado más */
	graph -> state_count++;

	/** Hacemos crecer la tabla de ser necesario */
	if(hash_table_should_rehash(graph))
	{
		hash_table_rehash(graph);
	}

	/* Guardamos el valor de este nuevo hash en el archivo de hashes */
	if(hashes_file) fprintf(hashes_file, "%lu\n", new_node -> hash_value);

	/* Retorna el puntero al nuevo nodo que creamos */
	return new_node;
}

/** Libera todos los recursos asociados a la tabla */
void state_graph_destroy(Graph* graph)
{
	/* Guardamos todas las estadisticas en sus archivos */
	statistics_save(graph);

	for (uint8_t row = 0; row < height; row++)
	{
		for (uint8_t col = 0; col < width; col++)
		{
			free(graph->zobrist[row][col]);
		}
		free(graph->zobrist[row]);
	}
	free(graph->zobrist);
	

	for(int i = 0; i < graph -> table_size; i++)
	{
		Node* node = graph -> hash_table[i];
		if(node)
		{
			state_destroy(node -> state);
			free(node);
		}
	}
	free(graph -> hash_table);
	free(graph);
}