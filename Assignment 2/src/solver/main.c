#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../watcher/watcher.h"
#include "bfs.h"
#include <time.h>
#include <string.h>

/* Revisa la integridad de los parámetros */
void check_parameters(int argc, char** argv)
{
	histograms_file = NULL;
	hashes_file = NULL;
	general_stats_file = NULL;
	use_watcher = true;

	bool bad_input = false;
	if(argc == 5)
	{
		histograms_file = fopen(argv[2], "w");
		hashes_file = fopen(argv[3], "w");
		general_stats_file = fopen(argv[4], "a");
		use_watcher = false;
	}
	else if(argc < 2)	bad_input = true;

	if(bad_input)
	{
		printf("Modo de uso: %s puzzle [histograms hashes stats]\n", argv[0]);
		printf("Donde\n");
		printf("\tpuzzle es el archivo donde se describe el puzzle\n");
		printf("\thistograms es el archivo donde se guardarán los histogramas\n");
		printf("\thashes es el archivo donde se guardarán los valores de hash\n");
		printf("\tstats es el archivo donde se guardarán las demás estadísticas\n");
		printf("\nSi quieres generar estadísticas, utiliza los scripts de python");
		exit(1);
	}
}

/** Visualiza la solucion en el watcher */
void visualize_solution(Node* node)
{
	if(node -> parent)
	{
		visualize_solution(node -> parent);
		operation_watch(node -> op);
	}
}

int main(int argc, char** argv)
{
	/* Revisa la integridad de los parametros del programa */
	check_parameters(argc, argv);

	/* Lee el estado inicial e inicializa las variables globales y el watcher */
	State state = state_init(argv[1]);

	/* Inicializa el grafo vacío y estructuras auxiliares */
	Graph* graph = state_graph_init();

	/* Marcamos el comienzo de la resolución */
	clock_t start = clock();

	/* Intenta resolver el problema */
	Node* node = solve(state, graph);

	double elapsed = ((double)(clock() - start))/ CLOCKS_PER_SEC;

	/* Si se encontró solución */
	if(node)
	{
		printf("Resuelto en %lf segundos\n", elapsed);

		visualize_solution(node);
	}
	else
	{
		printf("No existe solución\n");
	}

	state_graph_destroy(graph);

	if(general_stats_file) fprintf(general_stats_file, "%lf\n", elapsed);

	operations_destroy();

	watcher_close();

	if(histograms_file) fclose(histograms_file);
	if(hashes_file) fclose(hashes_file);
	if(general_stats_file) fclose(general_stats_file);

	return 0;
}
