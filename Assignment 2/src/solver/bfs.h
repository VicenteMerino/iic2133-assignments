#pragma once

#include "../graph/state_graph.h"

/** Busca la ruta al nodo solución usando BFS. Retorna NULL si no existe */
Node* solve(State initial, Graph* graph);
