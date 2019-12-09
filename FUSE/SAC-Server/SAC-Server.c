#include"SAC-Server.h"


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

void cargarBitArray(){

	char* bitArrayTemp = malloc(largoBitmapFS);
	leerBloque(1,bitArrayTemp);

	for(int i = 0 ; i < (*bloquesBitmapFS) ;i++){

	leerBloque((i+1),bitArrayTemp+i);

	}

	bitArrayFS = bitarray_create(bitArrayTemp, largoBitmapFS );
}

void cargarTablasNodos(void){
	tablas = malloc(1024 * sizeof(GFile));
	GFile* datos  = malloc(sizeof(GFile));
	for(int i = 0  ; i < 1024 ; i++){
		leerBloque(  (i + 1 + (*bloquesBitmapFS) ),datos); //cuando ande bloquesBitmapFS, usarlo
		memcpy(tablas + i,datos,sizeof(GFile));
	}
	free(datos);
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
	bitarray_destroy(bitArrayFS);
	free(tablas);
	free(bloquesBitmapFS);
	close(disco);
}


void leerHead(void){

	if(disco == NULL){
		Logger_Log(LOG_INFO, "No se pudo leer el head, no hay almacenamiento cargado.");
	}else{

	void* temporario = malloc(BLOCKSIZE);
	char* nombreFS = malloc(3*sizeof(char));
	uint32_t* versionFS = malloc(sizeof(uint32_t));
	uint32_t* inicioBitmapFS = malloc(sizeof(uint32_t));
	bloquesBitmapFS = malloc(sizeof(uint32_t));;

	int bloquesDisponibles = 0;
	int primerBloqueDisponible = 0;
	int primerTablaVacia = 0;
	int tablasDisponibles = 0;

	leerBloque(0,temporario);

	memcpy(nombreFS,temporario,sizeof(nombreFS));
	Logger_Log(LOG_INFO, "El volumen contiene un FileSystem: %s .", nombreFS);
	free(nombreFS);

	memcpy(versionFS,temporario+4,sizeof(versionFS));
	Logger_Log(LOG_INFO, "Version: %lu .", *versionFS);
	free(versionFS);

	memcpy(inicioBitmapFS,temporario+8,sizeof(inicioBitmapFS));
	Logger_Log(LOG_INFO, "Bitmap inicia en: %lu .", *inicioBitmapFS);
	free(inicioBitmapFS);

	memcpy(bloquesBitmapFS,temporario+12,sizeof(bloquesBitmapFS));
	Logger_Log(LOG_INFO, "Cantidad de bloques que ocupa el bitmap: %lu .", *bloquesBitmapFS);

	largoBitmapFS = largoAlmacenamiento/ BLOCKSIZE;
	Logger_Log(LOG_INFO, "Largo del Bitmap: %d .", largoBitmapFS);

	bloquesDatosFS = largoBitmapFS - 1025 - (*bloquesBitmapFS);
	Logger_Log(LOG_INFO, "Bloques para datos: %d .", bloquesDatosFS);

	cargarBitArray();
	cargarTablasNodos();

	bloquesDisponibles = bloquesLibres();
	Logger_Log(LOG_INFO, "Bloques disponibles: %d .", bloquesDisponibles);

	primerBloqueDisponible = buscarBloqueDisponible();
	Logger_Log(LOG_INFO, "Primer bloque disponible : %d .", primerBloqueDisponible);

	tablasDisponibles = tablasLibres();
	Logger_Log(LOG_INFO, "Tablas disponibles: %d .", tablasLibres);

	primerTablaVacia = buscarTablaDisponible();
	Logger_Log(LOG_INFO, "Primer tabla disponible : %d", primerTablaVacia);


	free(temporario);
	}
}

void leerBloque(int bloque, void* datos){
	void *extrae = malloc(BLOCKSIZE);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,bloque*BLOCKSIZE);
	memcpy(datos,extrae,BLOCKSIZE);
	munmap(extrae,BLOCKSIZE);
}
/*
void escribirBloqueConOffset(int bloque, int offset, void* datos){
	char *extrae = malloc(BLOCKSIZE);
	int largo = sizeof(uint32_t);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,0);
	memcpySegmento(extrae, datos,,0,offset,largo);
	msync(extrae,BLOCKSIZE,MS_SYNC);
	munmap(extrae,BLOCKSIZE);
}
*/

void borrarBloque(int bloque){
	bitarray_clean_bit(bitArrayFS,bloque);
}


int buscarBloqueDisponible(void){
	int indice = 0;
	int largoBitmap = bitarray_get_max_bit(bitArrayFS);
	while(bitarray_test_bit(bitArrayFS,indice) & ( indice < largoBitmap ) ){
		indice++;
	}
	if(indice == largoBitmap){
		Logger_Log(LOG_ERROR, "Disco lleno, no hay bloques vacios.");
		return -1; //Ver salida de este error;
	}else{
		return indice;
	}
}

int buscarTablaDisponible(void){
	int i = 0;
	while( (  (int)( (tablas + i)->state )   )  &  ( i < 1024) ){
		i++;
	}
	if(i == 1024){
		return -1; //Ver salida de este error;
	}else{
		return i;
	}
}

int bloquesLibres(void){
	int contador = 0;
	for(int i = 0 ; i < largoBitmapFS ; i++){
		contador += bitarray_test_bit(bitArrayFS,i);
	}
	return largoBitmapFS - contador;
}

int tablasLibres(void){
	int contador = 0;
	for(int i = 0 ; i < 1024 ; i++){
		if((int)(tablas + i)->state==  0){
			contador++;
		}
	}
	return contador;
}

void* crearArrayPunteros(GFile *tabla, int cantidad){ //TERMINAR
	void* arrayPunterosABloque = malloc(sizeof(ptrGBloque) *BLKINDIRECT);
	int bloqueVacio = 0;
	for(int i = 0; i < cantidad ; i++){
		bloqueVacio = buscarBloqueDisponible();
		//(arrayPunterosABloque + i) = bloqueVacio;
		bitarray_set_bit(bitArrayFS,bloqueVacio);
	}
	return arrayPunterosABloque;
}
void archivoNuevo(char* nombre,void*datos, void*padre){ //TERMINAR
	int tablaObjetivo = 0;
	int bloqueVacio = 0;

	tablaObjetivo = buscarTablaDisponible();

	if(tablaObjetivo == -1){
		Logger_Log(LOG_ERROR, "No se pueden crear mas archivos.");
	}else{

		int bloquesNecesariosDatos = ceil(sizeof(datos) + 0.0 /BLOCKSIZE);
		/*int bloquesNecesariosDirecciones = ceil((double) bloquesNecesariosDatos /(double)1024.0 );
		int bloquesNecesariosTotales = bloquesNecesariosDatos + bloquesNecesariosDirecciones;
		int bloquesDisponibles = bloquesLibres();

			if(bloquesNecesariosTotales <= bloquesDisponibles){
				crearArrayPunteros(tablas,bloquesNecesariosDirecciones);


			}else{
				Logger_Log(LOG_ERROR, "No hay espacio suficiente en el almacenamiento para guardar el archivo.");
			}
*/
	}
}
void borrarArchivo(void){ //SIN EMPEZAR

}

void memcpySegmento(void *destino, void *origen, size_t offsetOrigen, size_t offsetDestino, size_t largo)
{
   char *corigen = (char *)origen;
   char *cdestino = (char *)destino;

   for(int i = 0; i< largo;i++){
		cdestino[i+offsetDestino] = corigen[i+offsetOrigen];
	}
}



int main(){

	iniciarLog();

	cargarAlmacenamiento();

	leerHead();


	//iniciarServer();

	//apagarServer();

	return 0;
}

