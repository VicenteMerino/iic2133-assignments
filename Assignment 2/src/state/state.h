#pragma once

/** Módulo estándar de números enteros */
#include <stdint.h>
/** Módulo estándar de valores booleanos */
#include <stdbool.h>

/****************************************************************************/
/*                                  Estado                                  */
/*                                                                          */
/* Representa una posible configuración del problema. Dado que vas a tener  */
/* guardar miles de estados simultáneamente al hacer tu búsqueda, este está */
/* diseñado para ser lo más eficiente posible en memoria.                   */
/*                                                                          */
/* En primer lugar, las dimensiones del tablero no cambian entre un estado  */
/* y otro, por lo que se trabajan como variables globales.                  */
/*                                                                          */
/* El tablero mismo al final no es más que una matriz, por lo tanto se      */
/* escribe como tal. Puedes acceder a ella indexando con [row][col]         */
/*                                                                          */
/* Considerando además los límites de los problemas que trabajaremos, se    */
/* usan enteros positivos de 1 byte, uint8_t, que es lo más pequeño posible */
/****************************************************************************/

/** Variable global: Alto de la matriz */
uint8_t height;
/** Variable global: Ancho de la matriz */
uint8_t width;

/** Matriz que representa un estado del puzzle */
typedef uint8_t** State;

/****************************************************************************/
/*                                Operaciones                               */
/*                                                                          */
/* Representan las operaciones que llevan de un estado a otro. Para poder   */
/* guardarlas o pasarlas como parámetro, se codifican en un byte.           */
/*                                                                          */
/* El primer bit indica si la operación es flip de fila o columna,          */
/* y los demás indican la fila/columna sobre la cual actúa la operación     */
/*                                                                          */
/* Para que la búsqueda sea más ordenada, se provee una lista global de     */
/* todas las operaciones que tiene el problema: así se puede generar los    */
/* hijos de un estado iterando por esta lista y aplicando cada operación.   */
/****************************************************************************/

/** Representa una operación para pasar de un estado a otro */
typedef uint8_t Operation;

/** Variable global: Arreglo con todas las operaciones */
Operation* operations;
/** Variable global: Cantidad de operaciones que existen */
uint8_t op_count;

/****************************************************************************/
/*                                Funciones                                 */
/****************************************************************************/

/** Lee el estado inicial e inicializa las variables globales y el watcher */
State state_init        (char* filename);
/** Obtiene el estado producto de aplicar una operación */
void  state_next        (State state, Operation op, State container);
/** Libera los recursos asociados a este estado */
void  state_destroy     (State state);
/** Entrega true si y solo si el estado es solución */
bool  state_is_solution (State state);
/** Entrega true si y solo si ambos estados son iguales */
bool  state_equals      (State a, State b);


/** Da true si la operación es flip de fila, false si es flip de columna */
bool     operation_is_row  (Operation op);
/** Obtiene el indice de la fila o columna sobre la que opera la operación */
uint8_t  operation_index   (Operation op);
/** Visualiza la operacion en el watcher */
void     operation_watch   (Operation op);
/** Libera todos los recursos de la lista de operaciones */
void     operations_destroy();
