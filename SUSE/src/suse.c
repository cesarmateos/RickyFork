#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>
#include"kemmens/logger.h"
#include<commons/collections/list.h>
#include<commons/temporal.h>
#include<readline/readline.h>
#include"kemmens/SocketServer.h"

struct t_programa{
	int identificador;
	t_list* hilos;
};
struct t_hilo{
	int identificador;
	char* tiempoDeEjecucion;
};
typedef struct t_programa T_programa;
typedef struct t_hilo T_hilo;

void iniciarServidor(){
	SocketServer_Start("SUSE",3801);
	SocketServer_ActionsListeners evento;
	SocketServer_ListenForConnection(evento);
}
void iniciarLog(void){
	Logger_CreateLog("SUSE.log","SUSE",true);
}
T_programa* crearPrograma(int identificador){
	T_programa* programa;
	programa->identificador = identificador;
	programa->hilos = list_create();
	return programa;
}
T_hilo crearHilo(int identificador){
	T_hilo hilo;
	hilo.identificador = identificador;
	hilo.tiempoDeEjecucion = temporal_get_string_time();
}
int main() {
	t_list* listaPrograma = list_create();
	T_programa* programa1 = crearPrograma(1);
	list_add(listaPrograma,programa1);
	t_list* blockeados = list_create();
	return EXIT_SUCCESS;
}
