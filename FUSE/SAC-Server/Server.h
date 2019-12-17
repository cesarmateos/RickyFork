#include "kemmens/SocketServer.h"
#include "SAC.h"


typedef char nombreArchivo[GFILENAMELENGTH];
typedef char rutaArchivo[GFILENAMELENGTH*100];
/*
typedef struct{
	rutaArchivo rutaDirectorio;
}parametrosLeerDirectorio;
*/

t_config* leer_config(void);

void alRecibirPaquete(int socketID, int messageType, void* actualData);

void iniciarLog(void);

void iniciarServer(void);

void apagarServer(void);
