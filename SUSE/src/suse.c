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
	int numeroDePrograma;
	t_list* ready;
	T_hilo* exec;
};
typedef struct t_programa T_programa;

struct t_semaforo{
	char identificador;
	int valor;
	int valorMaximo;
};
typedef struct t_semaforo T_semaforo;
static sem_t sem;
//estructuras a ser utilizadas en suse

static int i =0;
t_list* listaProgramas;
t_list* blockeados;
t_list* new;
t_list* listaSemaforos;

void iniciarLog(void){
	Logger_CreateLog("SUSE.log","SUSE",true);
}

//carga de los semaforos de suse
t_list* cargarsemaforos(int size){
	t_config* suseconfig = config_create("suse.config");
	T_semaforo sem;
	t_list* listaDeSemaforos = list_create();
	char** identificadores = config_get_array_value(suseconfig,"SEM_IDS");
	char** valoresiniciales = config_get_array_value(suseconfig,"SEM_INIT");
	char** valoresmaximos = config_get_array_value(suseconfig,"SEM_MAX");
	for(int i=0;i<size;i++){
		sem.identificador = *identificadores[i];
		Logger_Log(LOG_INFO,"el identificador es %c\n",sem.identificador);
		sem.valor = (*valoresiniciales[i]-'0');
		Logger_Log(LOG_INFO,"el valor es %d\n",sem.valor);
		sem.valorMaximo = (*valoresmaximos[i]-'0');
		list_add(listaDeSemaforos,&sem);
	}
	free(suseconfig);
	free(identificadores);
	free(valoresiniciales);
	return listaDeSemaforos;
}

//inicializacion de programa
void crearPrograma(int identificador,int numerodeprograma){
	T_programa programa;
	programa.identificador = identificador;
	programa.numeroDePrograma = numerodeprograma;
	programa.ready = list_create();
	T_programa* p = malloc(sizeof(T_programa));
	*p = programa;
	list_add(listaProgramas,p);
	Logger_Log(LOG_INFO,"se cargo el programa %d",programa.identificador);

}

void ejecutarHilo(T_programa* programa,T_hilo* hiloAejecutar){
	programa->exec = hiloAejecutar;
}
//inicializacion de hilo
void crearHilo(int identificador,int pid,double tiempoEstimado){
	T_hilo hilo;
	time_t tiempoactual;
	time(&tiempoactual);
	hilo.identificador = identificador;
	hilo.pid=pid;
	hilo.tiempoDeEjecucion = tiempoactual;
	hilo.tiempoEstimado = tiempoEstimado;
	hilo.tiempoReal = 0;
	T_hilo* h = malloc(sizeof(T_hilo));
	*h = hilo;
	list_add(new,h);
	Logger_Log(LOG_INFO,"se cargo el hilo en new %d",hilo.identificador);
}
//se carga al estado new los hilos este estado compartidos por todos lo hilos de los programas desde este estado pasa al ready de cada programa
void cargarhiloAnew(T_hilo* hilo,t_list* new){
	list_add(new,hilo);
	Logger_Log(LOG_INFO,"se cargo el hilo en new %d",hilo->identificador);
}
//devuelve el proximo hilo a ejecutar
T_hilo* proximoHiloAejecutar(T_programa* programa){
	T_hilo* hilo =list_get(programa->ready,0);
	Logger_Log(LOG_INFO,"se devuelve el hilo %d\n",hilo->identificador);
	return hilo;

}

bool sjf(T_hilo* hiloA,T_hilo* hiloB){
    t_config* suseconfig = config_create("suse.config");
	double alpha = config_get_double_value(suseconfig,"ALPHA_SJF");
	double a = hiloA->tiempoEstimado * alpha + hiloA->tiempoReal * (1- alpha);
	double b = hiloB->tiempoEstimado * alpha + hiloB->tiempoReal * (1- alpha);
	free(suseconfig);
	return a < b;
}
//se carga un programa en la lisat de programas
void CargarPrograma(T_programa* programa,t_list* lista){
		list_add(lista,programa);
		Logger_Log(LOG_INFO,"se cargo el programa %d",programa->identificador);
}
// pasaje de un hilo a estado de bloqueado se saca del estado exec y se pasa a bloqueado se calcula el tiempo que estuvo ejecutando para elc alculo del sjf
void bloquearHilo(T_programa* programa,t_list* blockeado){
	T_hilo* hiloBloqueado = programa->exec;
	time_t tiemponuevo;
	time(&tiemponuevo);
	hiloBloqueado->tiempoReal = difftime(tiemponuevo,hiloBloqueado->tiempoDeEjecucion);
	programa->exec = NULL;
	list_add(blockeado,hiloBloqueado);
}
//se elimina el hilo
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
T_hilo* encontrarHilo(int tid,t_list* lista){
	int buscarhilo(T_hilo* hilo){
		if(hilo->identificador == tid)
			return 1;
	}
	return list_find(lista,(void*)buscarhilo);
}
//pasaje de los estados de new a ready de cada programa se utiliza un semaforo para que no haya espera activa
void cargarhilos(t_list* new,t_list* listaProgramas){
	while(list_size(new) != 0){
		sem_wait(&sem);
		T_hilo* hiloAagregar = malloc(sizeof(T_hilo));
		hiloAagregar = list_remove(new,0);
		T_programa* programa = malloc(sizeof(T_programa));
		programa = encontrarPrograma(hiloAagregar->pid,listaProgramas);
		Logger_Log(LOG_INFO,"%d\n",programa->identificador);
		Logger_Log(LOG_INFO,"se cargo el hilo %d en el programa %d\n",hiloAagregar->identificador,programa->numeroDePrograma);
		list_add(programa->ready,hiloAagregar);
		if(list_size(programa->ready) > 2)
			list_sort(programa->ready,sjf);
	}
}

void removerhilo(int identificador,t_list* lista){
	int buscarhilo(T_hilo* hilo){
		if(hilo->identificador == identificador)
			return 1;
	}
	list_remove_by_condition(lista,(void*)buscarhilo);
}
//operacion signal de los semaforos de suse
void incrementarSemaforo(T_semaforo* sem,T_hilo* hilo,t_list* bloqueado,t_list* listaProgramas){
	sem->valor += 1;
	if(sem->valor > 0){
		removerhilo(hilo->identificador,bloqueado);
		time_t tiemponuevo;
		time(&tiemponuevo);
		hilo->tiempoDeEjecucion = tiemponuevo;
		T_programa* programa= encontrarPrograma(hilo->pid,listaProgramas);
		list_add(programa->ready,hilo);
		list_sort(programa->ready,sjf);
	}
}
//operacion wait de los semaforos de suse
void decrementarSemaforo(T_semaforo* sem,T_hilo* hilo,t_list* bloqueado,t_list* listaProgramas){
	sem->valor -= 1;
	if(sem->valor <= 0){
		T_programa* programa= encontrarPrograma(hilo->pid,listaProgramas);
		programa->exec = NULL;
		list_add(bloqueado,hilo);
	}
}

void alrecibirPaquete(int socketID, int messageType, void* actualData){
	/*t_list* blockeados = list_create();
	t_list* new = list_create();
	t_list* lista = cargarsemaforos(2);*/
	switch (messageType){
	case INICIAR:
	{


		printf("hola");
		i++;
		break;
	}
	case CARGARHILO:
	{
		crearHilo((int*)actualData,socketID,0);
		cargarhilos(new,listaProgramas);

		break;
	}
	case PROXIMOHILOAEJECUTAR:
	{
		T_programa* programa = encontrarPrograma(socketID,listaProgramas);
		T_hilo* hilo =proximoHiloAejecutar(programa);
		break;
	}
	case INCREMENTARSEMAFORO:
	{
		break;
	}
	case DECREMENTARSEMAFORO:
	{
		break;
	}
	case BLOQUEAR:
	{
		T_programa* programa = encontrarPrograma(socketID,listaProgramas);
		T_hilo* hilo = encontrarHilo(actualData,programa->ready);
		list_add(blockeados,hilo);
	}
	case ELIMINAR:
	{
		T_programa* programa = encontrarPrograma(socketID,listaProgramas);
		programa->exec = NULL;
		break;
	}
	}
}
void alRecibirConexion(int socketID){
	//t_list* listaProgramas = list_create();
	crearPrograma(socketID,i);
}
void iniciarServidor(){
	SocketServer_Start("SUSE",3801);
	SocketServer_ActionsListeners* evento = malloc(sizeof(SocketServer_ActionsListeners));
	evento->OnClientConnected = alRecibirConexion;
	evento->OnPacketArrived = alrecibirPaquete;
	SocketServer_ListenForConnection(*evento);
}
void iniciarListas(){
	listaProgramas = list_create();
	blockeados = list_create();
	new = list_create();
	//listaSemaforos = cargarsemaforos(2);
}

int main() {
	iniciarLog();
	iniciarListas();
	t_config* suseconfig = config_create("suse.config");
	int nivelDeMultiprogramacion = config_get_int_value(suseconfig,"MAX_MULTIPROG");
	sem_init(&sem,1,nivelDeMultiprogramacion);
	iniciarServidor();


    /*T_programa programa = crearPrograma(1);
	T_programa programa2 = crearPrograma(2);
	CargarPrograma(&programa,listaPrograma);
	CargarPrograma(&programa2,listaPrograma);
	T_hilo hilo = crearHilo(0,1,3);
	T_hilo hilo2 = crearHilo(0,2,3);
	cargarhiloAnew(&hilo,new);
	cargarhiloAnew(&hilo2,new);
	cargarhilos(new,listaPrograma);
	//cargarhilos(new,listaPrograma);
	T_hilo hiloA = crearHilo(1,3,3,new);
	T_hilo hiloB = crearHilo(2,2,3,new);
	T_hilo* ptr2;
	list_add(listaPrograma,&hiloA);
	list_add(listaPrograma,&hiloB);
	list_sort(listaPrograma,sjf);
	ptr2 = list_get(listaPrograma,0);
	printf("el hilo mas chico es %d\n",ptr2->identificador);*/

	return EXIT_SUCCESS;
}
