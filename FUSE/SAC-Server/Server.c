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
			//SocketCommons_SendData(sock,);
			break;
		}
		case CREARDIR:
		{
			crearDirectorio( ((soloRuta*)actualData)->rutaDirectorio);
			free(actualData); //??
			break;
		}
		case BORRARDIR:
		{
			borrarDirectorioVacio( ((soloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case ABRIRDIR:
		{
			bool respuesta = existe( ((soloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case ATRIBUTOS:
		{
			GFile tabla;
			nroTabla nroTabla = localizarTablaArchivo( ((soloRuta*)actualData)->rutaDirectorio);
			tabla = *(mapTablas + nroTabla);
			//SocketCommons_SendData(sock,DEVUELVETABLA,tabla,sizeof(GFile));
			break;
		}
		case ABRIRFILE:
		{
			bool respuesta = existe( ((soloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case LEERFILE:
		{
			break;
		}
		case CREARFILE:
		{
			break;
		}
		case BORRARFILE:
		{
			borrarArchivo( ((soloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case ESCRIBIRFILE:
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

