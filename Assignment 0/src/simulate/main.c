#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "structs.h"

/** Retorna true si ambos strings son iguales */
bool string_equals(char* string1, char* string2)
{
  return !strcmp(string1, string2);
}
void simulate(char* filename)
{
  // Abrimos el archivo
  FILE* file = fopen(filename, "r");

  // Leemos la cantidad de terminales
  int terminal_count;
  fscanf(file, "%i", &terminal_count);


  // TODO: Inicializar estructura principal
  
  Terminal** titanic = malloc(terminal_count*sizeof(Terminal*));
  Terminal* nuevo_terminal;
  Puerta* puerta;


  for (int term = 0; term < terminal_count; term++)
  { 
    int gate_count;
    fscanf(file, "%i", &gate_count);

    nuevo_terminal = terminal_init(term, gate_count);


    
    titanic[term] = nuevo_terminal;
  }
  nuevo_terminal = NULL;
  puerta = NULL;
  free(puerta);
  free(nuevo_terminal);

  char command[32];

  while(true)
  {
    fscanf(file, "%s", command);

    if(string_equals(command, "END"))
    {
      break;
    }
    else if(string_equals(command, "INGRESO"))
    {
      int terminal, passenger_id, priority;

      fscanf(file, "%i %i %i", &terminal, &passenger_id, &priority);

      ingreso(passenger_id, titanic[terminal], priority, -1);


      // TODO: Procesar ingreso
    }
    else if(string_equals(command, "ABORDAJE"))
    {
      int terminal, gate;
      fscanf(file, "%i %i", &terminal, &gate);



      abordaje(titanic[terminal]->arreglo_puertas[gate], titanic[terminal]);




    }
    
    else if(string_equals(command, "CIERRE")){

      int terminal, gate;
      fscanf(file, "%i %i", &terminal, &gate);

      cierre(titanic[terminal],titanic[terminal]->arreglo_puertas[gate]);



    }
    else if(string_equals(command, "CLAUSURA"))
    {
      int term_out, term_in;

      fscanf(file, "%i %i", &term_out, &term_in);  

      clausura(titanic[term_out],titanic[term_in]);

      // TODO: Procesar clausura de terminal
      


      
    }

    else if(string_equals(command, "LASER")){
   
      int terminal, gate, index;
      fscanf(file, "%i %i %i\n", &terminal, &gate, &index);  
      laser(titanic[terminal]->arreglo_puertas[gate],index);
    }
  }

  fclose(file);
  printf("TITANIC LOG\n");
  Terminal* term_actual;
  Fila* fila_adultos;
  Fila* fila_ninos;
  Fila* fila_robots;
  Persona* p_actual;
  int cantidad_puertas;
  int largo_ninos;
  int largo_adultos;
  int largo_robots;
  for (int i = 0; i < terminal_count; i++)
  {

    if (titanic[i]->abierto==1)
    {
      printf("TERMINAL %i\n", titanic[i]->id_terminal);
      term_actual = titanic[i];

      cantidad_puertas = term_actual->cantidad_puertas;

      for (int j = 0; j < cantidad_puertas; j++)
      {
        fila_adultos = term_actual ->arreglo_puertas[j]->fila_adultos;
        fila_ninos = term_actual -> arreglo_puertas[j]->fila_ninos;
        fila_robots = term_actual -> arreglo_puertas[j]->fila_robots;
        largo_ninos = fila_ninos-> largo;
        largo_adultos= fila_adultos -> largo;
        largo_robots = fila_robots -> largo;

        if (term_actual->arreglo_puertas[j]->abierto==1)
        {
          printf("GATE %i: %i\n", term_actual->arreglo_puertas[j]->id_puerta, fila_adultos->largo+fila_robots->largo+fila_ninos->largo);
          p_actual = fila_ninos -> head;
          for (int k = 0; k < largo_ninos; k++)
          {
            printf("%i\n",p_actual->id_persona);
            p_actual = p_actual -> next;
      
          }
          p_actual = fila_adultos -> head;
          for (int k = 0; k < largo_adultos; k++)
          {
            printf("%i\n",p_actual->id_persona);
            p_actual = p_actual ->next;
          }
          p_actual = fila_robots -> head;
          for (int k = 0; k < largo_robots; k++)
          {

            printf("%i\n",p_actual->id_persona);
            p_actual = p_actual->next;

            
          }
        }
      }
    }
  // TODO: Liberar recursos e imprimir los pasajeros que siguen a bordo
  }
    printf("END LOG\n");
for (int t = 0; t < terminal_count; t++)
{
  for (int p = 0; p < titanic[t]->cantidad_puertas; p++)
  {
    //Niños
    fila_destroy(titanic[t]->arreglo_puertas[p]->fila_ninos);
    //Adultos
    fila_destroy(titanic[t]->arreglo_puertas[p]->fila_adultos);
    //Robots
    fila_destroy(titanic[t]->arreglo_puertas[p]->fila_robots);
    free(titanic[t]->arreglo_puertas[p]);
  }
  free(titanic[t]->arreglo_puertas);
  free(titanic[t]);
  }
free(titanic);
    
}
    
 


int main(int argc, char *argv[])
{
  // Este programa recibe dos parámetros:
  //  argv[0] = el programa en sí
  //  argv[1] = la ruta al archivo de input
  if (argc != 2)
  {
    printf("Cantidad de parámetros incorrecta\n");
    printf("Uso correcto: %s PATH_TO_INPUT\n", argv[0]);
    return 1;
  }

  simulate(argv[1]);

  return 0;
}
