#include "Server.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}

void iniciarLog(void){
	Logger_CreateLog("SAC.log","SAC-Server",true);
}



void apagarServer(void){
	SocketServer_TerminateAllConnections();
}

void alRecibirPaquete(int socketID, int messageType, void* actualData){

	switch(messageType){
		case LEERDIR:
		{
			char** ls = malloc(sizeof(rutaArchivo));
			ls = leerDirectorio(actualData);
			printf("Se leyó el directorio %s :", ls);
			free(ls);
			break;
		}
		case CREARDIR:
		{
			printf("Entro la siguiente data : %s  ", actualData);
			/*
			char* nombre = malloc(sizeof(GFILENAMELENGTH));
			nroTabla padre;
			int caracter = '/';
			int tamanio = strlen(actualData);
			char* directorioPadre = malloc(sizeof(tamanio));
			padre = localizarTablaArchivo(directorioPadre);
			nombre = strrchr(actualData,caracter);
			crearDirectorio( (nombre+1) ,padre);
			free(nombre);
			free(directorioPadre);
			*/
			break;
		}
		case BORRARDIR:
		{
			break;
		}
		case CREARFILE:
		{
			break;
		}
		case BORRARFILE:
		{
			break;
		}

	}
}

void iniciarServer(void){

	t_config* fuseConfig;

	int puerto;

	fuseConfig = leer_config();

	puerto = config_get_int_value(fuseConfig, "LISTEN_PORT");

	SocketServer_Start("SAC",puerto);

	SocketServer_ActionsListeners* evento = malloc(sizeof(SocketServer_ActionsListeners));

	evento->OnPacketArrived = alRecibirPaquete;

	SocketServer_ListenForConnection(*evento);

	config_destroy(fuseConfig);

}

