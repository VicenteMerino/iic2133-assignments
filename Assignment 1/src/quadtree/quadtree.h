#include "../imagelib/imagelib.h"

#pragma once


struct nodo;

typedef struct nodo Nodo;

struct nodo{
    int x;
    int y;
    int width;
    int height;
    Nodo* hijos[4];
    
    double L;
    double a;
    double b;

    double sd_L;
    double sd_a;
    double sd_b;

    double mean_L;
    double mean_a;
    double mean_b;

    double suma2_a;
    double suma2_b;
    double suma2_L;

    double gamma;

    int hoja;

};

struct quadtree;

typedef struct quadtree Quadtree;

struct quadtree
{
    Nodo* root;
};


struct nodobinario;

typedef struct nodobinario NodoBinario;

struct nodobinario{
    double alpha;
    NodoBinario* izquierdo;
    NodoBinario* derecho;

};



Quadtree* quadtree_init(Image* img);
Nodo* nodo_init(int x, int y, int width, int height);
void colorear_alpha(Nodo* nodo, Image* img, double alpha);
int min_alpha(Quadtree* qt, int start, int end, int alpha, int h_max);
void free_qt(Nodo* nodo);