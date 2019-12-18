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
			char* nombre = malloc(sizeof(GFILENAMELENGTH));
			char* sobrante;
			nroTabla padre = 0;
			int caracter = '/';
			int largoNombre = 0;
			int largoSobrante = 0;

			nombre = strrchr(((parametrosLeerDirectorio*)actualData)->rutaDirectorio,caracter);
			largoNombre = strlen(nombre);
			largoSobrante = ((parametrosLeerDirectorio*)actualData)->largoRuta - largoNombre;
			strncpy(sobrante,((parametrosLeerDirectorio*)actualData)->rutaDirectorio, largoSobrante);
			padre = localizarTablaArchivo(sobrante);
			crearDirectorio( (nombre+1) ,padre);
			free(nombre);

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

	evento->OnClientConnected = alRecibirConexion;

	evento->OnPacketArrived = alRecibirPaquete;

	SocketServer_ListenForConnection(*evento);

	config_destroy(fuseConfig);

}

