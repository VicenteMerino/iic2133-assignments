#pragma once

#include <cairo.h>
#include <stdbool.h>
#include "color.h"
#include <pthread.h>

pthread_mutex_t drawing_mutex;

struct sub_image
{
	/** Imagen de la sub-imagen */
	cairo_surface_t* image;
	/** Transformación inicial */
	cairo_matrix_t transform;
};

typedef struct sub_image SubImage;

/** Contiene la información de lo que ha de ser dibujado en el canvas */
struct content
{
	/** Ancho de la ventana */
	int image_width;
	/** Alto de la ventana */
	int image_height;
	/** La imagen completa */
	cairo_surface_t* image;
	/** Ancho de la matriz */
	int matrix_width;
	/** Alto de la matriz */
	int matrix_height;
	/** Ancho de las sub-imagenes */
	int sub_width;
	/** Alto de las sub-imagenes */
	int sub_height;
	/** Matriz de sub-imagenes */
	SubImage** sub_matrix;


	/** Interpolador de transformaciones */
	double t;
	/** Indica si una fila está siendo animada */
	bool animating_row;
	/** Indica si una columna está siendo animada */
	bool animating_col;
	/** Indica el índice de la fila / columna siendo animada */
	int index;
	/** Imagen de la animación */
	cairo_surface_t* animation_image;
	/* Matrices de transformación para la imagen de animaciones */
	cairo_matrix_t flip_row_target;
	cairo_matrix_t flip_row_source;
	cairo_matrix_t flip_col_target;
	cairo_matrix_t flip_col_source;



	/** Matriz de sub-imagenes */
	// cairo_surface_t*** image_matrix;
	// /** Transformaciones de la matriz de sub-imagenes */
	// cairo_matrix_t** transform_matrix_matrix;
};
/** Contiene la información de lo que ha de ser dibujado en el canvas */
typedef struct content Content;

/** Inicializa las herramientas de dibujo*/
Content* drawing_init(char* filename, int height, int width);
/** Dibuja sobre el canvas dado */
bool     drawing_draw         (cairo_t* cr, Content* cont);
/** Invierte una fila de la matriz */
void     drawing_flip_row     (Content* cont, int row);
/** Invierte una columna de la matriz */
void     drawing_flip_col     (Content* cont, int col);

/** Genera una imagen en pdf para un estado en particular */
void     drawing_snapshot     (Content* cont, char* filename);
/** Libera los recursos asociados a las herramientas de dibujo */
void     drawing_free         (Content* cont);
