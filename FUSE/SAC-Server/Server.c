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

void alDesconcetarCliente(int socketID){
	close(socketID);
}

void alRecibirPaquete(int socketID, int messageType, void* actualData){

	switch(messageType){
		case LEERDIR:
		{
			char** ls = malloc(sizeof(rutaArchivo));
			ls = leerDirectorio( ((SoloRuta*)actualData)->rutaDirectorio);
			printf("Se leyó el directorio %s :", ((SoloRuta*)actualData)->rutaDirectorio);
			SocketCommons_SendData(socketID,LISTADOARCHIVOS,ls,10000); //SACAR EL HARDCODEO
			break;
		}
		case CREARDIR:
		{
			crearDirectorio( ((SoloRuta*)actualData)->rutaDirectorio);
			free(actualData); //??
			break;
		}
		case BORRARDIR:
		{
			borrarDirectorioVacio( ((SoloRuta*)actualData)->rutaDirectorio);
			//close(socketID);
			break;
		}
		case ABRIRDIR:
		{
			bool respuesta = existe( ((SoloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case ATRIBUTOS:
		{
			GFile* tabla = malloc(sizeof(GFile));
			nroTabla nroTabla = localizarTablaArchivo( ((SoloRuta*)actualData)->rutaDirectorio);
			if(nroTabla == -1){
				tabla->tablaPadre = -2;
			}else{
			*tabla = *(mapTablas + nroTabla);
			}
			SocketCommons_SendData(socketID,DEVUELVETABLA,tabla,sizeof(GFile));
			free(tabla);
			break;
		}
		case ABRIRFILE:
		{
			bool respuesta = existe( ((SoloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case LEERFILE:
		{
			break;
		}
		case CREARFILE:
		{
			int estado = ArchivoNuevoVacio(((SoloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case BORRARFILE:
		{
			borrarArchivo( ((SoloRuta*)actualData)->rutaDirectorio);
			break;
		}
		case ESCRIBIRFILE:
		{
			Escribir* inicio = malloc(sizeof(Escribir));
			memcpy(inicio,actualData,sizeof(Escribir));
			void* datos = malloc(inicio->size);
			memcpy(datos, (actualData+sizeof(Escribir)), inicio->size );
			escribirArchivo(inicio->rutaDirectorio,inicio->size,inicio->offset,datos);
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

	evento->OnClientDisconnect = alDesconcetarCliente;

	SocketServer_ListenForConnection(*evento);

	config_destroy(fuseConfig);


	//iniciarServidor();
}

