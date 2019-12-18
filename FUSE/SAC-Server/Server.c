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

void alRecibirConexion(int socketID){
	printf("Conexion entrante en %d \n",socketID);
}

void alRecibirPaquete(int socketID, int messageType, void* actualData){

	switch(messageType){
		case LEERDIR:
		{
			char** ls = malloc(sizeof(rutaArchivo));
			ls = leerDirectorio(actualData);
			printf("Se leyó el directorio %s :", ls);
			break;
		}
		case CREARDIR:
		{
			crearDirectorio( ((parametrosLeerDirectorio*)actualData)->rutaDirectorio);
			break;
		}
		case BORRARDIR:
		{
			borrarDirectorio( ((parametrosLeerDirectorio*)actualData)->rutaDirectorio);
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

	evento->OnClientConnected = alRecibirConexion;

	evento->OnPacketArrived = alRecibirPaquete;

	SocketServer_ListenForConnection(*evento);

	config_destroy(fuseConfig);

}

