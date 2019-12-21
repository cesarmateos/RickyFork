#include "kemmens/SocketServer.h"
#include "Operaciones.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<commons/log.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>

t_config* leer_config(void);

void alRecibirConexion(int socketID);

void alRecibirPaquete(int socketID, int messageType, void* actualData);

void iniciarLog(void);

void iniciarServer(void);

void apagarServer(void);
