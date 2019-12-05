#include"SAC-Server.h"

t_config* leer_config(void){
		return config_create("fuse.config");
}

void iniciarLog(void){
	Logger_CreateLog("SAC.log","SAC-Server",true);
}

void leerHead(void){

	if(disco == NULL){
		Logger_Log(LOG_INFO, "No se pudo leer el head, no hay almacenamiento cargado.");
	}else{

	char* nombreFS = malloc(3*sizeof(char));
	uint32_t* versionFS = malloc(sizeof(uint32_t));
	uint32_t* inicioBitmapFS = malloc(sizeof(uint32_t));
	uint32_t* bloquesBitmapFS = malloc(sizeof(uint32_t));;

	int bloquesTotales;

	leerBloqueConOffset(0,0,nombreFS);
	Logger_Log(LOG_INFO, "El volumen contiene un FileSystem: %s .", nombreFS);
	free(nombreFS);

	leerBloqueConOffset(0,3,versionFS);
	Logger_Log(LOG_INFO, "Version: %lu .", versionFS);
	free(versionFS);

	leerBloqueConOffset(0,7,inicioBitmapFS);
	Logger_Log(LOG_INFO, "Bitmap inicia en: %lu .", inicioBitmapFS);
	free(inicioBitmapFS);

	leerBloqueConOffset(0,11,bloquesBitmapFS);
	Logger_Log(LOG_INFO, "Cantidad de bloques que ocupa el bitmap: %lu .", bloquesBitmapFS);
	free(bloquesBitmapFS);

	bloquesTotales = largoAlmacenamiento/ BLOCKSIZE;
	//bloquesBitmap = ((bloquesTotales/8)/BLOCKSIZE) + 1;

	bloquesDatosFS = bloquesTotales -1025 - 1 ;//-(int)bloquesBitmapFS;  El 1 ese se puede calcular con bloquesBitmap
	largoBitmapFS = bloquesTotales - 2;

	Logger_Log(LOG_INFO, "Largo del Bitmap: %d .", largoBitmapFS);
	Logger_Log(LOG_INFO, "Bloques para datos: %d .", bloquesDatosFS);

	bitArrayFS = malloc(largoBitmapFS); //Cambiar largo de malloc con bloquesDatosFS cuando ande
	leerBloqueConOffset(1,0,bitArrayFS);

	}
}

void escribirBloqueConOffset(int bloque, int offset, void* datos){
	char *extrae = malloc(BLOCKSIZE);
	int largo = sizeof(datos);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,0);
	memcpySegmento(extrae, datos,0,offset,largo);
	msync(extrae,BLOCKSIZE,MS_SYNC);
	munmap(extrae,BLOCKSIZE);
}

void leerBloqueConOffset(int bloque, int offset, void* datos){
	char *extrae = malloc(BLOCKSIZE);
	int largo = sizeof(datos);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,0);
	memcpySegmento(datos, extrae,offset,0,largo);
	munmap(extrae,BLOCKSIZE);
}

int buscarBloqueVacio(){
	int indice = 1024;
	while(bitarray_test_bit(bitArrayFS,indice) & indice < largoBitmapFS + 1){
		indice++;
	}
	if(indice == largoBitmapFS+1){
		Logger_Log(LOG_INFO, "Disco lleno, no hay bloques vacios.");
		return NULL; //Ver salida de este error;
	}else{
		return indice;
	}
}

int buscarTablaVacia(){
	int indice = 0;
	while(bitarray_test_bit(bitArrayFS,indice)& indice < 1024){
		indice++;
	}
	if(indice == 1024){
		Logger_Log(LOG_INFO, "No se pueden crear mas archivos.");
		return NULL; //Ver salida de este error;
	}else{
		return indice;

}
void escribirBloque(void*datos){
	int bloqueObjetivo;
	bloqueObjetivo = buscarBloqueVacio;
	escribirBloqueConOffset(bloqueObjetivo,0,datos);
	bitarray_set_bit(bitArrayFS,bloqueObjetivo);
}

void borrarBloque(int bloque){
	bitarray_clean_bit(bitArrayFS,bloque);
}
void archivoNuevo(char* nombre,void*datos, void*padre){ //VER
}
void borrarArchivo(){ //VER

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
		Logger_Log(LOG_ERROR, "Error al abrir el disco %s.", nombreHD);
	} else {
		off_t inicioArchivo;
		off_t finArchivo;

		inicioArchivo = lseek(disco, 0, SEEK_SET);
		finArchivo = lseek(disco, 0, SEEK_END);
		largoAlmacenamiento = finArchivo - inicioArchivo;
		lseek(disco, 0, SEEK_SET);

		Logger_Log(LOG_INFO, "Disco %s cargado exitosamente. Contiene %d bytes.", nombreHD, largoAlmacenamiento);

		config_destroy(fuseConfig);
	}

}
void descargarAlmacenamiento(){
	free(bitArrayFS);
	close(disco);
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

int main(){

	iniciarLog();

	cargarAlmacenamiento();

	leerHead();

	int bloqueVacio = 0;
	bloqueVacio = buscarBloqueVacio;
	printf("Bloque Vacio : %d \n", bloqueVacio);

	//iniciarServer();

	//apagarServer();

	return 0;
}

