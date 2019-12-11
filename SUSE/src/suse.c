#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>
#include"kemmens/logger.h"
#include<commons/collections/list.h>
#include<commons/temporal.h>
#include<readline/readline.h>
#include"kemmens/SocketServer.h"

static int gradoDeMutiprogramacion;
struct t_hilo{
	int identificador;
	char* tiempoDeEjecucion;
	int tiempoReal;
	int tiempoEstimado;
};
typedef struct t_hilo T_hilo;

struct t_programa{
	int identificador;
	t_list* ready;
	T_hilo* exec;
};
typedef struct t_programa T_programa;

struct t_semaforo{
	char identificador;
	int valor;
};
typedef struct t_semaforo T_semaforo;

void iniciarServidor(){
	SocketServer_Start("SUSE",3801);
	SocketServer_ActionsListeners evento;
	SocketServer_ListenForConnection(evento);
}
void iniciarLog(void){
	Logger_CreateLog("SUSE.log","SUSE",true);
}

T_programa crearPrograma(int identificador){
	T_programa programa;
	programa.identificador = identificador;
	programa.ready = list_create();
	return programa;
}

void ejecutarHilo(T_programa* programa,T_hilo* hiloAejecutar){
	programa->exec = hiloAejecutar;
}

T_hilo crearHilo(int identificador,int tiempoEstimado){
	T_hilo hilo;
	hilo.identificador = identificador;
	hilo.tiempoDeEjecucion = temporal_get_string_time();
	hilo.tiempoEstimado = tiempoEstimado;
	hilo.tiempoReal = 0;
	return hilo;
}

T_hilo* proximoHiloAejecutar(T_programa programa){
	return list_get(programa.ready,0);
}

bool sjf(T_hilo* hiloA,T_hilo* hiloB){
	int a = hiloA->tiempoEstimado * 0.5 + hiloA->tiempoReal *0.5;
	int b = hiloB->tiempoEstimado * 0.5 + hiloB->tiempoReal *0.5;
	return a > b;
}

void CargarPrograma(T_programa* programa,t_list* lista){
	if(list_size(lista) < gradoDeMutiprogramacion)
		list_add(lista,programa);
}
void bloquearHilo(T_programa* programa,t_list* blockeado){
	T_hilo* hiloBloqueado = programa->exec;
	programa->exec = NULL;
	list_add(blockeado,hiloBloqueado);
}
int main() {
	t_list* listaPrograma = list_create();
	T_hilo hiloA = crearHilo(1,3);
	T_hilo hiloB = crearHilo(2,2);
	T_hilo* ptr;
	list_add(listaPrograma,&hiloA);
	list_add(listaPrograma,&hiloB);
	list_sort(listaPrograma,sjf);
	ptr = list_get(listaPrograma,0);
	printf("el hilo mas chico es %d\n",ptr->identificador);
	t_list* blockeados = list_create();
	return EXIT_SUCCESS;
}
