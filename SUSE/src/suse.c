#include "suse.h"

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
};
typedef struct t_semaforo T_semaforo;
static sem_t sem;
//estructuras a ser utilizadas en suse

static int i =0;
void iniciarServidor(){
	SocketServer_Start("SUSE",3801);
	SocketServer_ActionsListeners evento;
	SocketServer_ListenForConnection(evento);
}
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
	for(int i=0;i<size;i++){
		sem.identificador = *identificadores[i];
		Logger_Log(LOG_INFO,"el identificador es %c\n",sem.identificador);
		sem.valor = (*valoresiniciales[i]-'0');
		Logger_Log(LOG_INFO,"el valor es %d\n",sem.valor);
		list_add(listaDeSemaforos,&sem);
	}
	free(suseconfig);
	free(identificadores);
	free(valoresiniciales);
	return listaDeSemaforos;
}
//inicializacion de programa
T_programa crearPrograma(int identificador,int numerodeprograma){
	T_programa programa;
	programa.identificador = identificador;
	programa.numeroDePrograma = numerodeprograma;
	programa.ready = list_create();
	return programa;
}

void ejecutarHilo(T_programa* programa,T_hilo* hiloAejecutar){
	programa->exec = hiloAejecutar;
}
//inicializacion de hilo
T_hilo crearHilo(int identificador,int pid,double tiempoEstimado){
	T_hilo hilo;
	time_t tiempoactual;
	time(&tiempoactual);
	hilo.identificador = identificador;
	hilo.pid=pid;
	hilo.tiempoDeEjecucion = tiempoactual;
	hilo.tiempoEstimado = tiempoEstimado;
	hilo.tiempoReal = 0;
	return hilo;
}
//se carga al estado new los hilos este estado compartidos por todos lo hilos de los programas desde este estado pasa al ready de cada programa
void cargarhiloAnew(T_hilo* hilo,t_list* new){
	list_add(new,hilo);
	Logger_Log(LOG_INFO,"se cargo el hilo en new %d",hilo->identificador);
}
//devuelve el proximo hilo a ejecutar
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
//pasaje de los estados de new a ready de cada programa se utiliza un semaforo para que no haya espera activa
void cargarhilos(t_list* new,t_list* listaProgramas){
	while(list_size(new) != 0){
		sem_wait(&sem);
		T_hilo* hiloAagregar = list_remove(new,0);
		T_programa* programa = encontrarPrograma(hiloAagregar->pid,listaProgramas);
		Logger_Log(LOG_INFO,"se cargo el hilo %d en el programa %d\n",hiloAagregar->identificador,programa->identificador);
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



int main() {
	iniciarLog();
	int messageType;
	t_config* suseconfig = config_create("suse.config");
	int nivelDeMultiprogramacion = config_get_int_value(suseconfig,"MAX_MULTIPROG");
	sem_init(&sem,1,nivelDeMultiprogramacion);
	t_list* listaPrograma = list_create();
	t_list* blockeados = list_create();
	t_list* new = list_create();
	t_list* lista = cargarsemaforos(2);
	iniciarServidor();
	/*switch(messageType){
		case 1:
		{
			//T_programa programa = crearPrograma(socket,i);
			//CargarPrograma(&programa,listaPrograma);
			break;
		}
		case 2:
		{
			//T_hilo hilo = crearHilo(0,socket,0);
			//cargarhiloAnew(&hilo,new);
			//cargarhilos(new,listaPrograma);
			break;
		}
		case 3:
		{
			//proximohiloAejecutar()
			break;
		}
		case 4:
		{
			//incrementarsemaforo
			break;
		}
		case 5:
		{
			//decrementarsemaforo
			break;
		}
		case 6:
		{
			//bloquearhilo
			break;
		}
		case 7:
		{
			//eliminarhilo
			break;
		}
		}
    T_programa programa = crearPrograma(1);
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
