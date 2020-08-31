#include "state.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../watcher/watcher.h"

/****************************************************************************/
/*                                Operaciones                               */
/****************************************************************************/

/** Máscara para obtener el primer bit del byte de operaciones */
#define ROW_MASK 0b10000000
/** Máscara para obtener los últimos 7 bits del byte de operaciones */
#define IDX_MASK 0b01111111

/** El indice de una operacion son sus ultimos 7 bytes */
#define INDEX(op) ((op) & IDX_MASK)
/** Si tiene un 1 en el primer bit, entonces la operacion es de fila */
#define ISROW(op) (((op) & ROW_MASK) == ROW_MASK)

/** La operacion de hacer flip de la fila i */
#define ROWOP(i) (ROW_MASK | (i))
/** La operacion de hacer flip de la columna i */
#define COLOP(i) (0 | (i))

/** Da true si la operación es flip de fila, false si es flip de columna */
bool operation_is_row  (Operation op)
{
	return ISROW(op);
}
/** Obtiene el indice de la fila o columna sobre la que opera la operación */
uint8_t  operation_index   (Operation op)
{
	return INDEX(op);
}

/****************************************************************************/
/*                                Funciones                                 */
/****************************************************************************/

/** Lee el estado inicial e inicializa las variables globales y el watcher */
State state_init(char* filename)
{
	/* Abre el archivo en modo lectura */
	FILE* f = fopen(filename, "r");

	if(!f)
	{
		perror("Error abriendo archivo\n");
		abort();
	}

	/* Lee el nombre de la imagen */
	char buffer[32];
	fscanf(f, "%s", buffer) ? : abort();

	/* Lee las dimensiones de la matriz */
	fscanf(f, "%hhu %hhu", &height, &width) ? : abort();

	/* Abre la interfaz gráfica */
	if(use_watcher) watcher_open(buffer, height, width);

	/* Lee la matriz misma */
	State state = calloc(height, sizeof(uint8_t*));
	for(uint8_t row = 0; row < height; row++)
	{
		state[row] = calloc(width, sizeof(uint8_t));
		for(uint8_t col = 0; col < width; col++)
		{
			fscanf(f, "%hhu", &state[row][col]) ? : abort();
		}
	}

	fclose(f);

	/* Inicializa la lista de operaciones */
	operations = calloc(height + width, sizeof(Operation));

	op_count = 0;
	for(uint8_t row = 0; row < height; row++)
	{
		operations[op_count++] = ROWOP(row);
	}
	for(uint8_t col = 0; col < width; col++)
	{
		operations[op_count++] = COLOP(col);
	}

	return state;
}

/*************************** Espacio de estados *****************************/

/** Guarda el nuevo estado producto de hacer flip row */
void state_flip_row(State parent, State son, uint8_t flip_row)
{
	for(uint8_t row = 0; row < height; row++)
	{
		/* La fila afectada */
		if(row == flip_row)
		{
			/* Se invierte */
			for(uint8_t col = 0; col < width; col++)
			{
				son[row][col] = parent[row][width - 1 - col];
			}
		}
		/* Las demás */
		else
		{
			/* Se copian tal cual */
			memcpy(son[row], parent[row], width);
		}
	}
}

/** Guarda el nuevo estado producto de hacer flip col */
void state_flip_col(State parent, State son, uint8_t flip_col)
{
	for(uint8_t row = 0; row < height; row++)
	{
		for(uint8_t col = 0; col < width; col++)
		{
			/* La columna afectada se invierte */
			if(col == flip_col)
			{
				son[row][col] = parent[height - 1 - row][col];
			}
			/* Las demás */
			else
			{
				/* Se copian tal cual */
				son[row][col] = parent[row][col];
			}
		}
	}
}

/** Obtiene el estado producto de aplicar una operación */
void state_next (State parent, Operation op, State container)
{
	/* Obtiene el índice de la operación */
	uint8_t index = INDEX(op);

	/* Si el primer bit no es 0, entonces es flip row */
	if(ISROW(op))
	{
		state_flip_row(parent, container, index);
	}
	/* Sino, es flip col */
	else
	{
		state_flip_col(parent, container, index);
	}
}

/********************************* Útiles ***********************************/

/** Entrega true si y solo si el estado es solución */
bool state_is_solution(State state)
{
	for(uint8_t row = 0; row < height; row++)
	{
		for(uint8_t col = 0; col < width; col++)
		{
			if(state[row][col] != row * width + col) return false;
		}
	}
	return true;
}

/** Entrega true si y solo si ambos estados son iguales */
bool  state_equals(State a, State b)
{
	for(uint8_t row = 0; row < height; row++)
	{
		for(uint8_t col = 0; col < width; col++)
		{
			if(a[row][col] != b[row][col]) return false;
		}
	}
	return true;
}

/******************************** Watcher ***********************************/

/** Visualiza la operacion en el watcher */
void  operation_watch(Operation op)
{
	/* Obtiene el índice de la operación */
	uint8_t index = INDEX(op);

	/* Si el primer bit no es 0, entonces es flip row */
	if(ISROW(op))
	{
		watcher_flip_row(index);
	}
	/* Sino, es flip col */
	else
	{
		watcher_flip_col(index);
	}
}

/****************************** Destructores ********************************/

/** Libera los recursos asociados a este estado */
void  state_destroy(State state)
{
	for(uint8_t row = 0; row < height; row++)
	{
		free(state[row]);
	}
	free(state);
}

/** Libera todos los recursos de la lista de operaciones */
void operations_destroy()
{
	free(operations);
}
