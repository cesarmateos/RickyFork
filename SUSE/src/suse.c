#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>
#include"kemmens/logger.h"
#include<commons/collections/list.h>
#include<commons/temporal.h>
#include<readline/readline.h>
#include"kemmens/SocketServer.h"

struct t_hilo{
	int identificador;
	int pid;//este es el identificador del programa
	time_t tiempoDeEjecucion;
	double tiempoReal;
	double tiempoEstimado;
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

void crearHilo(int identificador,double tiempoEstimado,t_list* new){
	T_hilo hilo;
	time_t tiempoactual;
	time(&tiempoactual);
	hilo.identificador = identificador;
	hilo.tiempoDeEjecucion = tiempoactual;
	hilo.tiempoEstimado = tiempoEstimado;
	hilo.tiempoReal = 0;
	//semaforowait
	list_add(new,&hilo);
}

T_hilo* proximoHiloAejecutar(T_programa programa){
	return list_get(programa.ready,0);
}

bool sjf(T_hilo* hiloA,T_hilo* hiloB){
    t_config* suseconfig = config_create("suse.config");
	double alpha = config_get_double_value(suseconfig,"ALPHA_SJF");
	double a = hiloA->tiempoEstimado * alpha + hiloA->tiempoReal * (1- alpha);
	double b = hiloB->tiempoEstimado * alpha + hiloB->tiempoReal * (1- alpha);
	free(suseconfig);
	return a < b;
}

void CargarPrograma(T_programa* programa,t_list* lista){
	/*t_config* suseconfig = config_create("suse.config");
	int gradoDeMultiprogramacion = config_get_int_value(suseconfig,"MAX_MULTIPROG");
	if(list_size(lista) < gradoDeMultiprogramacion)
		list_add(lista,programa);
	free(suseconfig);*/
}
void bloquearHilo(T_programa* programa,t_list* blockeado){
	T_hilo* hiloBloqueado = programa->exec;
	time_t tiemponuevo;
	time(&tiemponuevo);
	hiloBloqueado->tiempoReal = difftime(tiemponuevo,hiloBloqueado->tiempoDeEjecucion);
	hiloBloqueado->tiempoDeEjecucion = tiemponuevo;
	programa->exec = NULL;
	list_add(blockeado,hiloBloqueado);
}
void eliminarHilo(T_programa* programa,int index){
	list_remove(programa->ready,index);
	//semaforo signal
}
int main() {
	t_list* listaPrograma = list_create();
	t_list* blockeados = list_create();
	t_list* new = list_create();
	T_hilo hiloA = crearHilo(1,3);
	T_hilo hiloB = crearHilo(2,2);
	T_hilo* ptr;
	list_add(listaPrograma,&hiloA);
	list_add(listaPrograma,&hiloB);
	list_sort(listaPrograma,sjf);
	/*t_config* suseconfig = config_create("suse.config");
	double alpha = config_get_double_value(suseconfig,"ALPHA_SJF");*/
	ptr = list_get(listaPrograma,0);
	printf("el hilo mas chico es %d\n",ptr->identificador);
	/*printf("el alpha es %f",alpha);*/
	return EXIT_SUCCESS;
}
