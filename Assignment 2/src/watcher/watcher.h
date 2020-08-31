#pragma once

#include <stdbool.h>

/** Variable global: Indica si usar el watcher o no */
bool use_watcher;

/****************************************************************************/
/*                                Watcher                                   */
/*                                                                          */
/* Módulo a cargo de la visualización del problema.                         */
/****************************************************************************/

/** Abre una ventana para mostrar una imagen sobre la matriz dada */
void watcher_open     (char* img_file, int matrix_height, int matrix_width);
/** Visualiza en la ventana la inversión de una fila */
void watcher_flip_row (int row);
/** Visualiza en la ventana la inversión de una columna */
void watcher_flip_col (int col);
/** Genera una imagen PNG con el contenido actual de la ventana */
void watcher_snapshot (char* filename);
/** Cierra y libera los recursos de la ventana */
void watcher_close    ();
