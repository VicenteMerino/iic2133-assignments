#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "drawing.h"
#include "../jpg/cairo_jpg.h"

/** El mutex de dibujo */
// static pthread_mutex_t drawing_mutex;

/** Interpolación entre 2 matrices */
cairo_matrix_t matrix_lerp(cairo_matrix_t a, cairo_matrix_t b, double t)
{
  return (cairo_matrix_t)
  {
    .xx = (1 - t) * a.xx + t * b.xx,
    .yy = (1 - t) * a.yy + t * b.yy,
    .xy = (1 - t) * a.xy + t * b.xy,
    .yx = (1 - t) * a.yx + t * b.yx,
    .y0 = (1 - t) * a.y0 + t * b.y0,
    .x0 = (1 - t) * a.x0 + t * b.x0,
  };
}

static void subimage_swap(SubImage* a, SubImage* b)
{
  SubImage aux = *a;
  *a = *b;
  *b = aux;
}

cairo_matrix_t matrix_flip_horizontal(Content* cont, cairo_matrix_t mat)
{
  cairo_matrix_t res;

  int col = (mat.x0 + mat.xx) / cont -> sub_width;
  int ncol = cont -> matrix_width - 1 - col;
  int x = ncol * cont -> sub_width;

  res.xy = 0;
  res.yx = 0;

  res.xx = -mat.xx;
  res.yy = mat.yy;

  res.x0 = x;
  res.y0 = mat.y0;


  // if(res.xx < 0) res.x0 += cont -> sub_width;
  // if(res.yy < 0) res.y0 += cont -> sub_height;
  if(mat.xx > 0) res.x0 += cont -> sub_width;


  return res;
}

cairo_matrix_t matrix_flip_vertical(Content* cont, cairo_matrix_t mat)
{
  cairo_matrix_t res;

  int row = (mat.y0 + mat.yy) / cont -> sub_height;
  int nrow = cont -> matrix_height - 1 - row;
  int y = nrow * cont -> sub_height;

  res.xy = 0;
  res.yx = 0;

  res.xx = mat.xx;
  res.yy = -mat.yy;

  res.x0 = mat.x0;
  res.y0 = y;

  if(mat.yy > 0) res.y0 += cont -> sub_height;


  // matrix_transform(&res, cont -> sub_width, cont -> sub_height, mat.xx, -mat.yy, mat.x0, y);

  return res;
}

/** Invierte una fila de la matriz */
void drawing_flip_row(Content* cont, int row)
{
  cairo_t* cr = cairo_create(cont -> animation_image);
  cairo_identity_matrix(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
  cairo_set_source_rgba(cr, 0, 0, 0, 0);
  cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

  for(int col = 0; col < cont -> matrix_width; col++)
  {
    cairo_set_matrix(cr, &cont -> sub_matrix[row][col].transform);
    cairo_set_source_surface(cr, cont -> sub_matrix[row][col].image, 0, 0);
    cairo_paint(cr);

    cont -> sub_matrix[row][col].transform = matrix_flip_horizontal(cont, cont -> sub_matrix[row][col].transform);
  }
  cairo_destroy(cr);

  for(int col = 0; col < cont -> matrix_width / 2; col++)
  {
    subimage_swap(&cont -> sub_matrix[row][col], &cont -> sub_matrix[row][cont -> matrix_width - 1 - col]);
  }
}

/** Invierte una columna de la matriz */
void drawing_flip_col(Content* cont, int col)
{
  cairo_t* cr = cairo_create(cont -> animation_image);
  cairo_identity_matrix(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
  cairo_set_source_rgba(cr, 0, 0, 0, 0);
  cairo_paint(cr);
  cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

  for(int row = 0; row < cont -> matrix_height; row++)
  {
    cairo_set_matrix(cr, &cont -> sub_matrix[row][col].transform);
    cairo_set_source_surface(cr, cont -> sub_matrix[row][col].image, 0, 0);
    cairo_paint(cr);

    cont -> sub_matrix[row][col].transform = matrix_flip_vertical(cont, cont -> sub_matrix[row][col].transform);
  }

  for(int row = 0; row < cont -> matrix_height / 2; row++)
  {
    subimage_swap(&cont -> sub_matrix[row][col], &cont -> sub_matrix[cont -> matrix_height - 1 - row][col]);
  }
}

bool drawing_draw(cairo_t* restrict cr, Content* restrict cont)
{
  pthread_mutex_lock(&drawing_mutex);

  cairo_set_source_rgb(cr, 0, 0, 0);

  cairo_paint(cr);

  //
  // cairo_matrix_t mat;
  //
  // matrix_transform(&mat, cont -> width, cont -> height, -1, -1, 10, 10);
  //
  // cairo_set_matrix(cr, &mat);

  for(int row = 0; row < cont -> matrix_height; row++)
  {
    for(int col = 0; col < cont -> matrix_width; col++)
    {
      // if(cont -> sub_matrix[row][col].transform.xx > 0 || cont -> sub_matrix[row][col].transform.yy > 0) continue;
      if(cont -> animating_row && cont -> index == row)
      {
        continue;
      }
      else if(cont -> animating_col && cont -> index == col)
      {
        continue;
      }
      else
      {
        cairo_set_matrix(cr, &cont -> sub_matrix[row][col].transform);
        cairo_set_source_surface(cr, cont -> sub_matrix[row][col].image, 0, 0);
        cairo_paint(cr);
      }
    }
  }

  // cairo_matrix_t anim = sub_image_animate(cont, cont -> sub_matrix[row][col]);
  if(cont -> animating_row || cont -> animating_col)
  {
    cairo_matrix_t anim;

    if(cont -> animating_row)
    {
      anim = matrix_lerp(cont -> flip_row_source, cont -> flip_row_target, cont -> t);
    }
    else
    {
      anim = matrix_lerp(cont -> flip_col_source, cont -> flip_col_target, cont -> t);
    }

    cairo_set_matrix(cr, &anim);
    cairo_set_source_surface(cr, cont -> animation_image, 0, 0);

    cairo_paint(cr);
  }

  pthread_mutex_unlock(&drawing_mutex);

	return true;
}


/** Genera los contenedores para las dos imagenes superpuestas */
Content* drawing_init(char* filename, int height, int width)
{
  Content* cont = malloc(sizeof(Content));

	pthread_mutex_init(&drawing_mutex, NULL);

  cont -> image = cairo_image_surface_create_from_jpeg(filename);

  cont -> image_height = cairo_image_surface_get_height(cont -> image);
  cont -> image_width = cairo_image_surface_get_width(cont -> image);

  cont -> image_height -= cont -> image_height % height;
  cont -> image_width -= cont -> image_width % width;

  cont -> matrix_height = height;
  cont -> matrix_width = width;

  cont -> sub_width = cont -> image_width / width;
  cont -> sub_height = cont -> image_height / height;

  cont -> sub_matrix = calloc(height, sizeof(SubImage*));

  // cont -> image_matrix = calloc(height, sizeof(cairo_surface_t**));
  // cont -> transform_matrix_matrix = calloc(height, sizeof(cairo_matrix_t*));


  for(int row = 0; row < height; row++)
  {
    // cont -> image_matrix[row] = calloc(width, sizeof(cairo_surface_t*));
    // cont -> transform_matrix_matrix[row] = calloc(width, sizeof(cairo_matrix_t));

    cont -> sub_matrix[row] = calloc(width, sizeof(SubImage));

    for(int col = 0; col < width; col++)
    {
      cont -> sub_matrix[row][col].image = cairo_image_surface_create(CAIRO_FORMAT_RGB24, cont -> sub_width, cont -> sub_height);


      cairo_t* cr = cairo_create(cont -> sub_matrix[row][col].image);
      cairo_set_source_surface(cr, cont -> image, -col * cont -> sub_width, -row * cont -> sub_height);
      cairo_paint(cr);
      cairo_destroy(cr);

      cairo_matrix_init_translate(&cont -> sub_matrix[row][col].transform, col * cont -> sub_width, row * cont -> sub_height);

    }
  }
  cont -> animating_row = false;
  cont -> animating_col = false;
  cont -> index = -1;
  cont -> t = 0;
  cont -> animation_image = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, cont -> image_width, cont -> image_height);
  cairo_matrix_init_identity(&cont -> flip_col_source);
  cairo_matrix_init_identity(&cont -> flip_row_source);

  cont -> flip_row_target = (cairo_matrix_t)
  {
    .xx = -1,
    .yy = 1,
    .xy = 0,
    .yx = 0,
    .x0 = cont -> image_width,
    .y0 = 0
  };

  cont -> flip_col_target = (cairo_matrix_t)
  {
    .xx = 1,
    .yy = -1,
    .xy = 0,
    .yx = 0,
    .x0 = 0,
    .y0 = cont -> image_height
  };

  return cont;
}

/** Geenera una imagen en pdf para un estado en particular */
void drawing_snapshot(Content* cont, char* filename)
{
	double width = cont -> image_width;
	double height = cont -> image_height;

	/* Imprimimos las imagenes del tablero */
	cairo_surface_t* surface;
	cairo_t *cr;

  surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, width, height);

	cr = cairo_create(surface);

	/* Reseteamos los parámetros para generar correctamente la imagen */
	Content aux = *cont;

	/* Dibuja el estado actual */
	drawing_draw(cr, &aux);

  cairo_image_surface_write_to_jpeg(surface, filename, 100);

	cairo_surface_destroy(surface);
	cairo_destroy(cr);
}

/** Libera los recursos asociados a las herramientas de dibujo */
void drawing_free(Content* cont)
{
  for(int row = 0; row < cont -> matrix_height; row++)
  {
    for(int col = 0; col < cont -> matrix_width; col++)
    {
      cairo_surface_destroy(cont -> sub_matrix[row][col].image);
    }

    free(cont -> sub_matrix[row]);
  }

  free(cont -> sub_matrix);

  cairo_surface_destroy(cont -> image);

  free(cont);

  pthread_mutex_destroy(&drawing_mutex);
}
