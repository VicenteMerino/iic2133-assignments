#pragma once

//nodo de la lista ligada: fila

struct persona;

typedef struct persona Persona;

struct persona{
    int tipo;
    int id_persona;
    Persona* next;
    
};

struct fila;

typedef struct fila Fila;

struct fila
{
    Persona* head;
    Persona* tail;
    int largo;

};
struct puerta;

typedef struct puerta Puerta;

struct puerta
{
    Fila* fila_ninos;
    Fila* fila_adultos;
    Fila* fila_robots;
    //1 si el terminal está abierto, 0 eoc
    int abierto;
    int id_puerta;
    int pod;
};

struct terminal;

typedef struct terminal Terminal;

struct terminal{
    int id_terminal;
    int cantidad_puertas;
    //1 si el terminal está abierto, 0 en otro caso
    int abierto; 
    Puerta** arreglo_puertas;

};

void fila_append(Fila* fila, int tipo, int id);
void fila_print(Fila* fila);
void fila_destroy(Fila* fila);
Puerta* puerta_init(int id);
void puerta_append_filas(Puerta* puerta);
Terminal* terminal_init(int id, int n);
void terminal_append_puerta(Terminal* terminal, Puerta* puerta, int posicion);
void ingreso(int passenger_id, Terminal* terminal, int priority, int excluido);
void abordaje(Puerta* puerta, Terminal* terminal);
void cierre(Terminal* terminal, Puerta* puerta);
void clausura(Terminal* terminal1, Terminal* terminal2);
void laser(Puerta* puerta, int indice);
int largo_fila(Fila* fila);

