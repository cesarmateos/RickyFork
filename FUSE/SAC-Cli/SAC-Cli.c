#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include"kemmens/SocketClient.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}


int main(){

	t_config* fuseConfig;

	char* puerto;
	//int socketConectado;

	fuseConfig = leer_config();

	puerto = config_get_string_value(fuseConfig, "LISTEN_PORT");

	SocketClient_ConnectToServer(puerto);

	return 0;
}
