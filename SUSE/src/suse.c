#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <semaphore.h>
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
	int pid;//este es el identificador del programa vendria a ser el socket
	time_t tiempoDeEjecucion;
	double tiempoReal;
	double tiempoEstimado;
};
typedef struct t_hilo T_hilo;

struct t_programa{
	int identificador;//numero de socket;
	t_list* ready;
	T_hilo* exec;
};
typedef struct t_programa T_programa;

struct t_semaforo{
	char identificador;
	int valor;
};
typedef struct t_semaforo T_semaforo;
static sem_t sem;

void iniciarServidor(){
	SocketServer_Start("SUSE",3801);
	SocketServer_ActionsListeners evento;
	SocketServer_ListenForConnection(evento);
}
void iniciarLog(void){
	Logger_CreateLog("SUSE.log","SUSE",true);
}

t_list* cargarsemaforos(int size){
	t_config* suseconfig = config_create("suse.config");
	T_semaforo sem;
	t_list* listaDeSemaforos = list_create();
	char** identificadores = config_get_array_value(suseconfig,"SEM_IDS");
	char** valoresiniciales = config_get_array_value(suseconfig,"SEM_INIT");
	for(int i=0;i<size;i++){
		sem.identificador = identificadores[i];
		sem.valor = (*valoresiniciales[i]-'0');
		list_add(listaDeSemaforos,&sem);
	}
	free(suseconfig);
	free(identificadores);
	free(valoresiniciales);
	return listaDeSemaforos;
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
		list_add(lista,programa);
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
	sem_post(&sem);
}
T_programa* encontrarPrograma(int pid,t_list* lista){
	int buscarprograma(T_programa* programa){
		if(programa->identificador == pid)
			return 1;
	}
	return list_find(lista,(void*)buscarprograma);
}

void cargarhilos(t_list* new,t_list* listaProgramas){
	while(list_size(new) != 0){
		sem_wait(&sem);
		T_hilo* hiloAagregar = list_remove(new,0);
		T_programa* programa = encontrarPrograma(hiloAagregar->pid,listaProgramas);
		list_add(programa->ready,hiloAagregar);
	}
}

void removerhilo(int identificador,t_list* lista){
	int buscarhilo(T_hilo* hilo){
		if(hilo->identificador == identificador)
			return 1;
	}
	list_remove_by_condition(lista,(void*)buscarhilo);
}
void incrementarSemaforo(T_semaforo* sem,T_hilo* hilo,t_list* bloqueado,t_list* listaProgramas){
	sem->valor += 1;
	if(sem->valor > 0){
		removerhilo(hilo->identificador,bloqueado);
		T_programa* programa= encontrarPrograma(hilo->pid,listaProgramas);
		list_add(programa->ready,hilo);
	}
}

void decrementarSemaforo(T_semaforo* sem,T_hilo* hilo,t_list* bloqueado,t_list* listaProgramas){
	sem->valor -= 1;
	if(sem->valor <= 0){
		T_programa* programa= encontrarPrograma(hilo->pid,listaProgramas);
		programa->exec = NULL;
		list_add(bloqueado,hilo);
	}
}



int main() {
	iniciarLog();
	t_config* suseconfig = config_create("suse.config");
	int nivelDeMultiprogramacion = config_get_int_value(suseconfig,"MAX_MULTIPROG");
	sem_init(&sem,1,nivelDeMultiprogramacion);
	t_list* listaPrograma = list_create();
	t_list* blockeados = list_create();
	t_list* new = list_create();
	/*char** identificadores = config_get_array_value(suseconfig,"SEM_IDS");
	char* a = identificadores[0];
	T_semaforo* ptr = list_get(lista,0);
	Logger_Log(LOG_INFO,"se muestra el identificador %s\n",ptr->identificador);
	Logger_Log(LOG_INFO,"se muestra el valor %d\n",ptr->valor);
	T_hilo hiloA = crearHilo(1,3);
	T_hilo hiloB = crearHilo(2,2);
	T_hilo* ptr;
	list_add(listaPrograma,&hiloA);
	list_add(listaPrograma,&hiloB);
	list_sort(listaPrograma,sjf);
	ptr = list_get(listaPrograma,0);
	printf("el hilo mas chico es %d\n",ptr->identificador);*/

	return EXIT_SUCCESS;
}
