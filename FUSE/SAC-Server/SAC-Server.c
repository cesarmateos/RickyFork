#include"SAC-Server.h"



t_config* leer_config(void){
		return config_create("fuse.config");
}

void iniciarLog(void){
	Logger_CreateLog("SAC.log","SAC-Server",true);
}


void leerHead(void){
	unsigned char* nombreFS;
	uint32_t* versionFS;
	uint32_t* inicioBitmapFS;
	uint32_t* bloquesBitmapFS;

	leer(0,0);
	nombreFS = (void *) extrae;
	Logger_Log(LOG_INFO, "El volumen contiene un FileSystem %s .", nombreFS);

	leer(0,3);
	versionFS = (void *) extrae;
	Logger_Log(LOG_INFO, "Version %lu .", versionFS);

	leer(0,7);
	versionFS = (void *) extrae;
	Logger_Log(LOG_INFO, "Bitmap inicia en %lu .", inicioBitmapFS);

	leer(0,11);
	versionFS = (void *) extrae;
	Logger_Log(LOG_INFO, "Largo en bloques del bitmap %lu .", bloquesBitmapFS);

}

void leer(int bloque, int offset){
	int principio = BLOCKSIZE * bloque;
	int largo = BLOCKSIZE - offset;

	lseek(disco,principio+offset,SEEK_SET);

	extrae =(void*)mmap(NULL,largo,PROT_READ|PROT_WRITE,MAP_SHARED,disco,offset);

	return;
}
void cargarAlmacenamiento(void){

	t_config* fuseConfig;
	char* nombreHD;

	fuseConfig = leer_config();

	nombreHD = config_get_string_value(fuseConfig, "DISCO");

	disco = open(nombreHD, O_RDWR);


	if (!disco) {
		Logger_Log(LOG_ERROR, "Error al abrir el disco %s", nombreHD);
	} else {
		int bloquesTotales;
		off_t inicioArchivo;
		off_t finArchivo;
		off_t largoArchivo;

		inicioArchivo = lseek(disco, 0, SEEK_SET);
		finArchivo = lseek(disco, 0, SEEK_END);
		largoArchivo = finArchivo - inicioArchivo;
		lseek(disco, 0, SEEK_SET);

		bloquesTotales = largoArchivo/ BLOCKSIZE;
		bloquesBitmap = ((bloquesTotales/8)/BLOCKSIZE) + 1;
		bloquesDatos = bloquesTotales -1025 -bloquesBitmap;

		Logger_Log(LOG_INFO, "Disco  %s cargado exitosamente. Contiene %d bloques de datos", nombreHD, bloquesDatos);

		config_destroy(fuseConfig);
	}

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
	close(disco);
	int algo = munmap(extrae,BLOCKSIZE);
}
int main(){

	iniciarLog();

	cargarAlmacenamiento();

	leerHead();

	iniciarServer();

	apagarServer();

	return 0;
}

