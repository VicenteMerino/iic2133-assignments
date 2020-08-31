#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include "animation.h"
#include "../drawing/drawing.h"
#include <math.h>
#include <locale.h>

#define TOTALTIME 320.0
#define TIMESTEP 8.0

/** El thread encargado de actualizar el contenido de la ventana */
static pthread_t* update_thread;

static void animate_flip_row(Content* cont, int row, GtkWidget* canvas)
{
	cont -> animating_row = true;
	cont -> index = row;

	double start_x = 0;
	double start_y = cont -> sub_height * row;

	/* Parámetros de la animación */
	double frames = TOTALTIME / TIMESTEP;
	double delta = 1.0 / frames;

	/* La animación en sí */
	for(int i = 0; i < frames; i++)
	{
		pthread_mutex_lock(&drawing_mutex);
			cont -> t += delta;
		pthread_mutex_unlock(&drawing_mutex);

		gtk_widget_queue_draw_area(canvas, start_x, start_y, cont -> image_width, cont -> sub_height);
		// gtk_widget_queue_draw(canvas);

		usleep(TIMESTEP * 1000);
	}

	/* Hacer permantente el cambio de posición */
	pthread_mutex_lock(&drawing_mutex);
		cont -> t = 0;
		cont -> animating_row = false;
	pthread_mutex_unlock(&drawing_mutex);

	// gtk_widget_queue_draw_area(canvas, start_x, start_y, cont -> image_width, cont -> sub_height);
	gtk_widget_queue_draw(canvas);

	usleep(TOTALTIME * 1000);

}

static void animate_flip_col(Content* cont, int col, GtkWidget* canvas)
{
	cont -> animating_col = true;
	cont -> index = col;

	double start_x = cont -> sub_width * col;
	double start_y = 0;

	/* Parámetros de la animación */
	double frames = TOTALTIME / TIMESTEP;
	double delta = 1.0 / frames;

	/* La animación en sí */
	for(int i = 0; i < frames; i++)
	{
		pthread_mutex_lock(&drawing_mutex);
			cont -> t += delta;
		pthread_mutex_unlock(&drawing_mutex);

		gtk_widget_queue_draw_area(canvas, start_x, start_y, cont -> sub_width, cont -> image_height);
		// gtk_widget_queue_draw(canvas);

		usleep(TIMESTEP * 1000);
	}

	/* Hacer permantente el cambio de posición */
	pthread_mutex_lock(&drawing_mutex);
		cont -> t = 0;
		cont -> animating_col = false;
	pthread_mutex_unlock(&drawing_mutex);

	// gtk_widget_queue_draw_area(canvas, start_x, start_y, cont -> image_width, cont -> sub_height);
	gtk_widget_queue_draw(canvas);

	usleep(TOTALTIME * 1000);
}


/** Lleva a cabo la actualización del tablero */
static void* update(void* ptr)
{
	setlocale(LC_ALL, "C");

	/* Desencapsula los parámetros */
	void** param = ptr;
	GtkWidget* canvas = param[0];
	Content* cont = param[1];
	free(param);

	char command[64];

  while(update_thread)
  {
    /* Si alguno de los numeros falla, dejamos de llamar */
    if(fscanf(stdin, "%s", command))
    {
			/* Cerrar ventana */
      if(!strcmp(command, "X"))
      {
				gtk_main_quit();
				break;
      }
			/* Flip Row */
			else if(!strcmp(command, "R"))
			{
				int row;
				fscanf(stdin, "%d", &row) ? : abort();
				drawing_flip_row(cont, row);
				// gtk_widget_queue_draw(canvas);
				animate_flip_row(cont, row, canvas);
			}
			/* Flip Column */
			else if(!strcmp(command, "C"))
			{
				int col;
				fscanf(stdin, "%d", &col) ? : abort();
				drawing_flip_col(cont, col);
				// gtk_widget_queue_draw(canvas);
				animate_flip_col(cont, col, canvas);
			}
			/* Guardar imagen */
			else if(!strcmp(command, "S"))
			{
				char filename[64];
				fscanf(stdin, "%s", filename) ? : abort();
				drawing_snapshot(cont, filename);
			}
			/* Comando desconocido */
			else
			{
				break;
			}
    }
    else
    {
      break;
    }

  }
  pthread_exit(NULL);
}

/** Inicializa el thread que animará el programa */
void animation_init(GtkWidget* canvas, gpointer user_data)
{
	/* Encapsula ambos parámetros dentro de un mismo puntero */
	void** param = calloc(2, sizeof(void*));
	param[0] = canvas;
	param[1] = user_data;

  /* Inicializamos el thread */
  update_thread = malloc(sizeof(pthread_t));
  /* Y lo lanzamos */
  pthread_create(update_thread, NULL, update, param);
}

/** Aborta la rutina que actualiza el tablero */
void animation_abort()
{
	pthread_cancel(*update_thread);
	free(update_thread);
	update_thread = NULL;
}
