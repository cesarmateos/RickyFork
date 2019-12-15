#include "Server.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}

void iniciarLog(void){
	Logger_CreateLog("SAC.log","SAC-Server",true);
}

void iniciarServer(void){

	t_config* fuseConfig;

	int puerto;

	fuseConfig = leer_config();

	puerto = config_get_int_value(fuseConfig, "LISTEN_PORT");

	SocketServer_Start("SAC",puerto);

	SocketServer_ActionsListeners evento;

	SocketServer_ListenForConnection(evento);

	config_destroy(fuseConfig);

}

void apagarServer(void){
	SocketServer_TerminateAllConnections();
}

