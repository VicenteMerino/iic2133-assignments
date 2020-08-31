#include "structs.h"
#include <stdio.h>
#include <stdlib.h>

Fila* fila_init()
{
  Fila* fila = malloc(sizeof(Fila));
  fila -> head = NULL;
  fila -> tail = NULL;
  fila -> largo = 0;

  return fila;
}

void fila_append(Fila* fila, int tipo, int id){
    Persona* persona = malloc(sizeof(Persona));
    persona -> tipo = tipo;
    persona -> id_persona = id;
    persona -> next = NULL;

    if(fila -> head == NULL){
        fila -> head = persona;
    }
    else{
        fila -> tail -> next = persona;
    }
    fila -> tail = persona;
    fila->largo+=1;

}

int largo_fila(Fila* fila){
  int count = 0;
  Persona* current = fila -> head;
  while (current != NULL)
  {
    current = current -> next;
    count += 1;
  }
  return count;
}

void fila_pop(Fila* fila){
  Persona* anterior = fila -> head;
  if (anterior -> next != NULL)
  {
    fila -> head = anterior -> next;

    fila->largo-=1;

  }
  else
  {
    fila ->head = NULL;
    fila ->tail = NULL;
    fila->largo=0;
  }
  free(anterior);
  
}



void fila_destroy(Fila* fila)
{
  // Liberamos los nodos
  if(fila -> head)
  {
    Persona* curr = fila -> head -> next;
    Persona* prev = fila -> head;

    while(curr)
    {
      free(prev);
      prev = curr;
      curr = curr -> next;
    }
    free(prev);
    free(curr);
  }
  // Liberamos la lista en si

  free(fila);
}

Puerta* puerta_init(int id)
{
  Puerta * puerta = malloc(sizeof(Puerta));
  puerta -> id_puerta = id;
  puerta -> fila_ninos = NULL;
  puerta -> fila_adultos = NULL;
  puerta -> fila_robots = NULL;
  puerta -> pod = 0;
  puerta -> abierto = 1;
  Fila* fila1;
  fila1 = fila_init();
  Fila* fila2;
  fila2 = fila_init();
  Fila* fila3;
  fila3 = fila_init();
  puerta -> fila_ninos = fila1;
  puerta -> fila_adultos = fila2;
  puerta -> fila_robots = fila3;
  return puerta;
}




Terminal* terminal_init(int id, int n){
  Terminal* terminal = malloc(sizeof(Terminal));
  terminal -> id_terminal = id;
  terminal -> cantidad_puertas = n;
  terminal -> arreglo_puertas = malloc(n*sizeof(Puerta));
  terminal -> abierto = 1;

  for (int i = 0; i < n; i++)
  {
    Puerta* puerta = puerta_init(i);
    terminal ->arreglo_puertas[i] = puerta;
  }
  
  return terminal;

}


// entrega la mejor opcion de puerta en un terminal "terminal" con prioridad
//"prioridad" excluyendo la puerta "excluid" si no se excluye nada se pone -1

int mejor_opcion(Terminal* terminal, int prioridad, int excluido){
  int choice = 0;
  Puerta* puerta_actual;
  int largo_actual;
  int largo;
  int cantidad_puertas = terminal -> cantidad_puertas;

  if (excluido == 0 || terminal->arreglo_puertas[0]->abierto==0)
  {  

    while (terminal->arreglo_puertas[choice]->abierto==0||excluido==choice)
    {
      
      choice += 1;
    }
    
  }

  
  if (prioridad == 0){
  largo = terminal -> arreglo_puertas[choice]->fila_ninos->largo;

  
    for(int npuerta = choice; npuerta < cantidad_puertas; npuerta++)
    {

      puerta_actual = terminal -> arreglo_puertas[npuerta];
      largo_actual = puerta_actual -> fila_ninos->largo;

      if (largo_actual < largo && excluido!=npuerta && puerta_actual ->abierto == 1)
      {
        largo = largo_actual;
        choice = npuerta;
      }
    }
  }

  else if (prioridad == 1)
  {

    largo = terminal -> arreglo_puertas[choice] -> fila_ninos->largo+ terminal -> arreglo_puertas[choice] -> fila_adultos->largo;

    for(int npuerta = choice; npuerta < cantidad_puertas; npuerta++)
    {

      puerta_actual = terminal -> arreglo_puertas[npuerta];
      largo_actual = puerta_actual -> fila_ninos->largo + puerta_actual -> fila_adultos->largo;

      if (largo_actual < largo && excluido != npuerta && puerta_actual -> abierto == 1)

      {

        largo = largo_actual;
        choice = npuerta;
      }
    }
  }

  else if (prioridad == 2)
  {
    largo = terminal -> arreglo_puertas[choice] -> fila_ninos->largo+terminal -> arreglo_puertas[choice] -> fila_adultos->largo+terminal -> arreglo_puertas[choice] -> fila_robots->largo;
    for(int npuerta = choice; npuerta < cantidad_puertas; npuerta++)
    {
    puerta_actual = terminal -> arreglo_puertas[npuerta];
      largo_actual = puerta_actual -> fila_ninos->largo + puerta_actual -> fila_adultos->largo + puerta_actual -> fila_robots->largo;
      if (largo_actual < largo && excluido != npuerta && puerta_actual -> abierto == 1)
      {
        largo = largo_actual;
        choice = npuerta;
      }
    }
  }
  return choice;
}
void ingreso(int passenger_id, Terminal* terminal, int priority, int excluido){

  int opcion = mejor_opcion(terminal, priority, excluido);
  Puerta* puerta_escogida = terminal -> arreglo_puertas[opcion];
  if (priority == 0)
  {
    fila_append(puerta_escogida -> fila_ninos, priority, passenger_id);
  }
  else if(priority == 1)
  {
    fila_append(puerta_escogida -> fila_adultos, priority, passenger_id);

  }
  else if(priority == 2){
    fila_append(puerta_escogida -> fila_robots, priority, passenger_id);
  }
}

int min(int a, int b){
  if (a < b)
  {
    return a;
    }
  else
  {
    return b;
  }
  
  
}
void abordaje(Puerta* puerta, Terminal* terminal){
  int id_terminal = terminal->id_terminal;
  int pod = puerta->pod;
  int id_puerta = puerta->id_puerta;
  printf("POD %i %i %i LOG\n", id_terminal, id_puerta, pod);
  int largo_ninos = puerta -> fila_ninos->largo;
  int contador = 0;
  int id_persona;
  Fila* fila_ninos = puerta->fila_ninos;
  Fila* fila_adultos = puerta->fila_adultos;
  Fila* fila_robots = puerta->fila_robots;
  while (largo_ninos > 0 && contador < 8)
  {
    id_persona = fila_ninos->head->id_persona;
    printf("%i\n", id_persona);
    fila_pop(fila_ninos);
    contador += 1;
    largo_ninos -= 1;
  }
  if (largo_ninos == 0 && contador < 8)
  {
    int largo_adultos = fila_adultos->largo;
    while (largo_adultos > 0 && contador < 8)
    {
      id_persona = fila_adultos ->head->id_persona;
      printf("%i\n", id_persona);
      fila_pop(fila_adultos);
      contador += 1;
      largo_adultos -= 1;
    if (largo_adultos == 0 && contador < 8)
    {
      int largo_robots = fila_robots->largo;
      while (largo_robots > 0 && contador < 8)
      {
        id_persona = fila_robots->head->id_persona;
        printf("%i\n", id_persona);
        fila_pop(fila_robots);
        contador += 1;
        largo_robots -= 1; 
      }
    }
    }   
  }
  puerta ->pod = puerta->pod+1;
}
void cierre(Terminal* terminal, Puerta* puerta){
  int id = puerta -> id_puerta;
  Fila* fila_ninos = puerta -> fila_ninos;
  Fila* fila_adultos = puerta -> fila_adultos;
  Fila* fila_robots = puerta -> fila_robots;
  Persona* persona_actual;

  while (fila_ninos->largo!=0)
  {
    persona_actual = fila_ninos ->head;
    ingreso(persona_actual ->id_persona, terminal, 0, id);
    fila_pop(fila_ninos);
  }
  while (fila_adultos->largo!=0)
  {
    persona_actual = fila_adultos ->head;
    ingreso(persona_actual ->id_persona, terminal, 1, id);
    fila_pop(fila_adultos);
  }  
  while (fila_robots->largo!=0)
  {
    persona_actual = fila_robots ->head;
    ingreso(persona_actual ->id_persona, terminal, 2, id);
    fila_pop(fila_robots);
  }
  puerta -> abierto=0;

}
int cantidad_terminal(Terminal* terminal, int tipo){
  int cantidad_puertas = terminal -> cantidad_puertas;
  int n = 0;
  for (int i = 0; i < cantidad_puertas; i++)
  {
    if (tipo == 0)
    {
    n += terminal->arreglo_puertas[i]->fila_ninos->largo;
    }
    else if (tipo==1){
    n += terminal-> arreglo_puertas[i]->fila_adultos->largo;

    }
    else if (tipo == 2)
    {
    n += terminal->arreglo_puertas[i]->fila_robots->largo;
    }
    
    
  }
  return n; 
}
// retorna el largo de la fila más larga del tipo priority en un terminal
int max_conjuto(Terminal* terminal, int priority){
  int cant_puertas = terminal ->cantidad_puertas;
  Puerta** array_puertas = terminal ->arreglo_puertas;
  int comparacion = 0;
  int largo_total = 0;
  for (int i = 0; i < cant_puertas; i++)
  {
    if (array_puertas[i]->abierto==1)
    {
      if (priority == 0)
      {
      largo_total = array_puertas[i]->fila_ninos->largo;
      }

      else if (priority == 1)
      {
      largo_total = array_puertas[i]->fila_adultos->largo;
      }
      
      else
      {
      largo_total = array_puertas[i]->fila_robots->largo;
      }
      
      

    }
    if (largo_total>=comparacion){
      comparacion = largo_total;
    }
  }
  return comparacion;  
}




void clausura(Terminal* terminal1, Terminal* terminal2){
  int npuertas1 = terminal1 -> cantidad_puertas;
  Puerta** arreglo1 = terminal1 -> arreglo_puertas;
  Fila* nueva_fila_ninos = fila_init();
  Fila* nueva_fila_adultos = fila_init();
  Fila* nueva_fila_robots = fila_init();
  int cantidad_ninos = max_conjuto(terminal1, 0);
  int cantidad_adultos = max_conjuto(terminal1, 1);
  int cantidad_robots = max_conjuto(terminal1, 2);



    for (int j = 0; j < cantidad_ninos; j++)
    {
        for (int i = 0; i < npuertas1; i++)
    {
    if (arreglo1[i] -> fila_ninos->largo>0)
      {
        fila_append(nueva_fila_ninos, arreglo1[i]->fila_ninos->head->tipo, arreglo1[i]->fila_ninos->head->id_persona);
        fila_pop(arreglo1[i]->fila_ninos);
      }
    else if (arreglo1[i] -> fila_adultos ->largo>0)
    {
      fila_append(nueva_fila_adultos, arreglo1[i]->fila_adultos->head->tipo, arreglo1[i]->fila_adultos->head->id_persona);
      fila_pop(arreglo1[i]->fila_adultos);
    }
    else if (arreglo1[i] ->fila_robots ->largo>0)
    {
      fila_append(nueva_fila_robots, arreglo1[i]->fila_robots->head->tipo, arreglo1[i]->fila_robots->head->id_persona);
      fila_pop(arreglo1[i]->fila_robots);
    }

    }
    }

    for (int j = 0; j < cantidad_adultos; j++){
            for (int i = 0; i < npuertas1; i++)
    {
      if (arreglo1[i] -> fila_adultos ->largo>0)
      {
        fila_append(nueva_fila_adultos, arreglo1[i]->fila_adultos->head->tipo, arreglo1[i]->fila_adultos->head->id_persona);
        fila_pop(arreglo1[i]->fila_adultos);
      }
      else if (arreglo1[i] ->fila_robots ->largo>0)
      {
        fila_append(nueva_fila_robots, arreglo1[i]->fila_robots->head->tipo, arreglo1[i]->fila_robots->head->id_persona);
        fila_pop(arreglo1[i]->fila_robots);
      }
    }
    }

    for (int j = 0; j < cantidad_robots; j++)
    {
            for (int i = 0; i < npuertas1; i++)
    {
      if (arreglo1[i] ->fila_robots ->largo>0)
      {
        fila_append(nueva_fila_robots, arreglo1[i]->fila_robots->head->tipo, arreglo1[i]->fila_robots->head->id_persona);
        fila_pop(arreglo1[i]->fila_robots);
      }
    }
    }
    
  terminal1 -> abierto = 0;
  int n = nueva_fila_ninos->largo;

  for (int i = 0; i < n; i++)
  {
    ingreso(nueva_fila_ninos->head->id_persona, terminal2, 0, -1);
    fila_pop(nueva_fila_ninos);
  }

  n = nueva_fila_adultos -> largo;

  for (int i = 0; i < n; i++)
  {

    ingreso(nueva_fila_adultos->head->id_persona, terminal2, 1, -1);
    fila_pop(nueva_fila_adultos);
  }

  n = nueva_fila_robots->largo;
  for (int i = 0; i < n; i++)
  {

    ingreso(nueva_fila_robots->head->id_persona, terminal2, 2, -1);
    fila_pop(nueva_fila_robots);
  }
  fila_destroy(nueva_fila_ninos);
  fila_destroy(nueva_fila_adultos);
  fila_destroy(nueva_fila_robots);
  
}
  
void laser(Puerta* puerta, int indice){
  int largo_ninos = puerta ->fila_ninos->largo;
  int largo_adultos = puerta -> fila_adultos->largo;
  int largo_robots = puerta -> fila_robots->largo;
  if (indice < largo_ninos)
  {
    if (indice == 0)
    { 
      
      Persona* pers_actual = puerta->fila_ninos->head;
      puerta ->fila_ninos->head=puerta->fila_ninos->head->next;
      free(pers_actual);
    }
    else if (indice == largo_ninos -1)
    {
      Persona* pers_actual = puerta ->fila_ninos->head;
      for (int i = 0; i < largo_ninos - 2; i++)
      {
        pers_actual = pers_actual ->next;
      }
      pers_actual -> next = NULL;
      free(puerta -> fila_ninos ->tail);
      puerta -> fila_ninos ->tail= pers_actual;


    }
    
    else
    {
      Persona* pers_eliminada = puerta -> fila_ninos ->head;
      Persona* pers_anterior = puerta -> fila_ninos -> head;
      Persona* pers_siguiente = puerta -> fila_ninos -> head;

      for (int i = 0; i < indice-1; i++)
      {
        pers_anterior = pers_anterior -> next;
      }
      for (int i = 0; i <= indice-1; i++)
      {
        pers_eliminada = pers_eliminada ->next;
      }
      for (int i = 0; i <= indice; i++)
      {
        pers_siguiente = pers_siguiente ->next;
      }

      pers_anterior ->next = pers_siguiente;
      free(pers_eliminada); 
    }
    puerta->fila_ninos->largo-=1;

  }

  else if (indice >= largo_ninos && indice < largo_adultos + largo_ninos)
  {
    
    if (indice == largo_ninos)
    { 
      Persona* pers_actual = puerta->fila_adultos->head;
      puerta ->fila_adultos->head=puerta->fila_adultos->head->next;
      free(pers_actual);
    }
    else if (indice == largo_ninos + largo_adultos -1)
    {
      Persona* pers_actual = puerta ->fila_adultos->head;
      for (int i = 0; i < largo_adultos - 2; i++)
      {
        pers_actual = pers_actual ->next;
      }
      pers_actual -> next = NULL;
      free(puerta -> fila_adultos ->tail);
      puerta -> fila_adultos ->tail= pers_actual;


    }
    
    else
    {

      
      Persona* pers_eliminada = puerta -> fila_adultos ->head;
      Persona* pers_anterior = puerta -> fila_adultos -> head;
      Persona* pers_siguiente = puerta -> fila_adultos -> head;

      for (int i = 0; i < indice - largo_ninos-1; i++)
      {
        pers_anterior = pers_anterior -> next;

        

      }

      for (int i = 0; i <= indice - largo_ninos-1; i++)
      {
        pers_eliminada = pers_eliminada ->next;
      }
      for (int i = 0; i <= indice - largo_ninos; i++)
      {
        pers_siguiente = pers_siguiente ->next;
      }



      pers_anterior ->next = pers_siguiente;
      free(pers_eliminada); 
    }
    puerta->fila_adultos->largo-=1;

  }
  
    else if (indice >= largo_ninos + largo_adultos)
  {    
    

    if (indice == largo_ninos + largo_adultos)
    { 
      Persona* pers_actual = puerta->fila_robots->head;
      puerta ->fila_robots->head=puerta->fila_robots->head->next;
      free(pers_actual);
    }
    else if (indice == largo_ninos + largo_adultos +largo_robots -1)
    {    

      Persona* pers_actual = puerta ->fila_robots->head;
      for (int i = 0; i < largo_robots - 2; i++)
      {
        pers_actual = pers_actual ->next;
      }
      pers_actual -> next = NULL;
      free(puerta -> fila_robots ->tail);
      puerta -> fila_robots ->tail= pers_actual;


    }
    
    else
    {


      Persona* pers_eliminada = puerta -> fila_robots ->head;
      Persona* pers_anterior = puerta -> fila_robots -> head;
      Persona* pers_siguiente = puerta -> fila_robots -> head;
      
      int cont = 0;
      for (int i = 0; i < indice - largo_ninos - largo_adultos-1; i++)
      {
        cont +=1;



        pers_anterior = pers_anterior -> next;

      }
      for (int i = 0; i <= indice - largo_ninos -largo_adultos-1; i++)
      {
        pers_eliminada = pers_eliminada ->next;
      }
      for (int i = 0; i <= indice - largo_ninos - largo_adultos; i++)
      {
        pers_siguiente = pers_siguiente ->next;
      }
      

      pers_anterior ->next = pers_siguiente;
      free(pers_eliminada);
    }
    puerta->fila_robots->largo-=1;
  }


  

}