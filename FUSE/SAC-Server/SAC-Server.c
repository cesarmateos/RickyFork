#include"SAC-Server.h"




t_config* leer_config(void){
		return config_create("fuse.config");
}

void iniciarLog(void){
	Logger_CreateLog("SAC.log","SAC-Server",true);
}


void leerHead(void){
	char* nombreFS = malloc(3*sizeof(char));
	uint32_t* versionFS = malloc(sizeof(uint32_t));
	uint32_t* inicioBitmapFS = malloc(sizeof(uint32_t));
	uint32_t* bloquesBitmapFS = malloc(sizeof(uint32_t));;

	int bloquesTotales;


	leer(0,0,nombreFS);
	Logger_Log(LOG_INFO, "El volumen contiene un FileSystem %s .", nombreFS);
	free(nombreFS);

	leer(0,3,versionFS);
	Logger_Log(LOG_INFO, "Version %lu .", versionFS);
	free(versionFS);

	leer(0,7,inicioBitmapFS);
	Logger_Log(LOG_INFO, "Bitmap inicia en %lu .", inicioBitmapFS);
	free(inicioBitmapFS);

	leer(0,11,bloquesBitmapFS);
	Logger_Log(LOG_INFO, "Largo en bloques del bitmap %lu .", bloquesBitmapFS);
	free(bloquesBitmapFS);

	bloquesTotales = largoAlmacenamiento/ BLOCKSIZE;
	//bloquesBitmap = ((bloquesTotales/8)/BLOCKSIZE) + 1;
	bloquesDatosFS = bloquesTotales -1025 - 1 ;//-(int)bloquesBitmapFS;

	Logger_Log(LOG_INFO, "Bloques para datos %d .", bloquesDatosFS);

}

void escribir(int bloque, int offset, void* dato){
	char *extrae = malloc(BLOCKSIZE);
	int largo = sizeof(dato);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,0);
	memcpySegmento(extrae, dato,0,offset,largo);
	msync(extrae,BLOCKSIZE,MS_SYNC);
	munmap(extrae,BLOCKSIZE);
}

void leer(int bloque, int offset, void* dato){
	char *extrae = malloc(BLOCKSIZE);
	int largo = sizeof(dato);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,0);
	memcpySegmento(dato, extrae,offset,0,largo);
	munmap(extrae,BLOCKSIZE);
}


void memcpySegmento(void *destino, void *origen, size_t offsetOrigen, size_t offsetDestino, size_t largo)
{
   char *corigen = (char *)origen;
   char *cdestino = (char *)destino;

   for(int i = 0; i< largo;i++){
		cdestino[i+offsetDestino] = corigen[i+offsetOrigen];
	}
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
		off_t inicioArchivo;
		off_t finArchivo;


		inicioArchivo = lseek(disco, 0, SEEK_SET);
		finArchivo = lseek(disco, 0, SEEK_END);
		largoAlmacenamiento = finArchivo - inicioArchivo;
		lseek(disco, 0, SEEK_SET);


		Logger_Log(LOG_INFO, "Disco  %s cargado exitosamente. Contiene %d bytes", nombreHD, largoAlmacenamiento);

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

}
int main(){

	iniciarLog();

	cargarAlmacenamiento();

	leerHead();

	iniciarServer();

	apagarServer();

	return 0;
}

