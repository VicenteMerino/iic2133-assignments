#include "watcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#define WATCHER "./beholder"

static FILE* watcher = NULL;

/** Abre una ventana para mostrar una imagen sobre la matriz dada */
void watcher_open(char* img_file, int matrix_height, int matrix_width)
{
	setlocale(LC_ALL, "C");

	char command[256];

	sprintf(command, "%s %s %d %d", WATCHER, img_file, matrix_height, matrix_width);

	if(watcher) watcher_close();

	watcher = popen(command, "w");
}

/** Visualiza en la ventana la inversión de una fila */
void watcher_flip_row (int row)
{
	if(watcher)
	{
		if(fprintf(watcher, "R %d\n", row) < 0)
		{
			watcher_close();
		}
		else
		{
			fflush(watcher);
		}
	}
}

/** Visualiza en la ventana la inversión de una columna */
void watcher_flip_col (int col)
{
	if(watcher)
	{
		if(fprintf(watcher, "C %d\n", col) < 0)
		{
			watcher_close();
		}
		else
		{
			fflush(watcher);
		}
	}
}

/** Genera una imagen PNG con el contenido actual de la ventana */
void watcher_snapshot (char* filename)
{
	if(watcher)
	{
		if(fprintf(watcher, "S %s\n", filename) < 0)
		{
			watcher_close();
		}
		else
		{
			fflush(watcher);
		}
	}
}

/** Cierra y libera los recursos de la ventana */
void watcher_close()
{
	if(watcher)
  {
    if(fprintf(watcher, "%s\n", "X") >= 0)
    {
      fflush(watcher);
      pclose(watcher);
    }
    watcher = NULL;
  }
}
