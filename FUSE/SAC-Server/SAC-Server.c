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

void mapearTablas(void){
	mapTablas = calloc(GFILEBYTABLE, sizeof(GFile));
	mapTablas = (void*) mmap(NULL,(GFILEBYTABLE * sizeof(GFile)) ,PROT_READ|PROT_WRITE,MAP_SHARED,disco, (inicioTablas *BLOCKSIZE) );
}
void mapearBitmap(void){
	char* bitArrayTemp = calloc(largoBitmap,sizeof(char));
	mapBitmap = calloc(largoBitmap,sizeof(char));
	bitArrayTemp = (char*) mmap(NULL,largoBitmap,PROT_READ|PROT_WRITE,MAP_SHARED,disco, ((*inicioBitmap) * BLOCKSIZE) );
	mapBitmap = bitarray_create(bitArrayTemp, largoBitmap );
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

		leerHead();
	}

}
void descargarAlmacenamiento(){
	conteos();
	free(bloquesBitmap);
	free(inicioBitmap);
	munmap(mapBitmap, largoBitmap);
	munmap(mapTablas,sizeof(GFile) * GFILEBYTABLE);
	bitarray_destroy(mapBitmap);
	close(disco);
	Logger_Log(LOG_INFO, "Se ha descargado el Almacenamiento");
}


void leerHead(void){

	if(disco == NULL){
		Logger_Log(LOG_INFO, "No se pudo leer el head, no hay almacenamiento cargado.");
	}else{

	void* temporario = malloc(BLOCKSIZE);
	char* nombreFS = malloc(3*sizeof(char));
	uint32_t* versionFS = malloc(sizeof(uint32_t));
	inicioBitmap = malloc(sizeof(uint32_t));
	bloquesBitmap = malloc(sizeof(uint32_t));

	leerBloque(0,temporario);

	memcpy(nombreFS,temporario,sizeof(nombreFS));
	Logger_Log(LOG_INFO, "El volumen contiene un FileSystem: %s .", nombreFS);
	free(nombreFS);

	memcpy(versionFS,temporario+4,sizeof(uint32_t));
	Logger_Log(LOG_INFO, "Version: %lu .", *versionFS);
	free(versionFS);

	memcpy(inicioBitmap,temporario+8,sizeof(uint32_t));
	Logger_Log(LOG_INFO, "Bitmap inicia en el bloque: %lu .", *inicioBitmap);

	largoBitmap = largoAlmacenamiento/ BLOCKSIZE;
	Logger_Log(LOG_INFO, "Largo del Bitmap en bits: %lu .", largoBitmap);

	memcpy(bloquesBitmap,temporario+12,sizeof(uint32_t));
	Logger_Log(LOG_INFO, "Cantidad de bloques que ocupa el bitmap: %lu .", *bloquesBitmap);

	inicioTablas = 1 + (*bloquesBitmap);
	Logger_Log(LOG_INFO, "Las tablas de Nodos inician en el bloque: %lu .", inicioTablas);

	bloquesDatos = largoBitmap - 1025 - (*bloquesBitmap);
	Logger_Log(LOG_INFO, "Bloques para datos totales: %lu.", bloquesDatos);

	mapearTablas();
	mapearBitmap();

	crearRaiz();

	conteos();

	free(temporario);
	}
}

void leerBloque(uint32_t bloque, void* datos){
	void *extrae = malloc(BLOCKSIZE);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,bloque*BLOCKSIZE);
	memcpy(datos,extrae,BLOCKSIZE);
	munmap(extrae,BLOCKSIZE);
}


void escribirBloque(uint32_t bloque, void* datos){
	void *extrae = malloc(BLOCKSIZE);
	extrae = (void*) mmap(NULL,BLOCKSIZE,PROT_READ|PROT_WRITE,MAP_SHARED,disco,bloque*BLOCKSIZE);
	memcpy(extrae, datos, BLOCKSIZE);
	msync(extrae,BLOCKSIZE,MS_SYNC);
	munmap(extrae,BLOCKSIZE);
}


void borrarBloque(uint32_t bloque){
	bitarray_clean_bit(mapBitmap,bloque);
}


uint32_t buscarBloqueDisponible(void){
	uint32_t indice = GFILEBYTABLE;
	uint32_t largoBitmap = bitarray_get_max_bit(mapBitmap);
	while(bitarray_test_bit(mapBitmap,indice) & ( indice < largoBitmap ) ){
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
	while( ( ( (int)( (mapTablas + i)->state )) != 0   )  &  ( i < GFILEBYTABLE)   ){
		i++;
	}
	if(i == GFILEBYTABLE){
		return -1;
	}else{
		return i;
	}
}

uint32_t contadorBloquesLibres(void){
	uint32_t contador = 0;
	for(uint32_t i = 0 ; i < largoBitmap ; i++){
		contador += bitarray_test_bit(mapBitmap,i);
	}
	return largoBitmap - contador;
}

int contadorTablasLibres(void){
	int contador = 0;
	for(int i = 0 ; i < GFILEBYTABLE ; i++){
		if((int)(mapTablas + i)->state == 0){
			contador++;
		}
	}
	return contador;
}

void crearPunterosIndirectos(GFile *tabla, int cantidad){ //TERMINAR
	ptrGBloque* arrayPunterosIndirectos = malloc(sizeof(ptrGBloque) *BLKINDIRECT);
	uint32_t bloqueVacio = 0;
	for(int i = 0; i < cantidad ; i++){
		bloqueVacio = buscarBloqueDisponible();  //Busco bloque Disponible.
		*(arrayPunterosIndirectos + i) = bloqueVacio; //Copio el numero de bloque disponible en el array.
		bitarray_set_bit(mapBitmap,bloqueVacio); //Reservo el bloque.
	}
	memcpy(tabla->blk_indirect,arrayPunterosIndirectos,cantidad);
}

GFile* devolverTabla(int numeroTabla){
	return (mapTablas + numeroTabla);
}
void archivoNuevo(char* nombre, void*datos, uint32_t tamanio, uint32_t padre){
	int numeroTablaAUsar = 0;
	GFile* tabla = malloc(sizeof(GFile));
	ptrGBloque* arrayPunterosIndirectos;

	numeroTablaAUsar = buscarTablaDisponible();

	if(numeroTablaAUsar == -1){
		Logger_Log(LOG_ERROR, "No se pueden crear mas archivos.");
	}else{

		int bloquesNecesariosDatos = ceil( (tamanio + 0.0 ) /BLOCKSIZE);
		int bloquesNecesariosIndirectos = ceil( bloquesNecesariosDatos / 1024.0 );
		int bloquesNecesariosTotales = bloquesNecesariosDatos + bloquesNecesariosIndirectos;
		int bloquesDisponibles = contadorBloquesLibres();

		time_t fechaActual;
		fechaActual = time(NULL);

			if(bloquesNecesariosTotales <= bloquesDisponibles){
				tabla = devolverTabla(numeroTablaAUsar);
				crearPunterosIndirectos(tabla,bloquesNecesariosIndirectos);
				tabla->state = 1;

				int largoNombre = strlen(nombre);
				memcpy(tabla->fname,nombre,largoNombre);
				tabla->parent_dir_block = padre;
				tabla->file_size = tamanio;
				tabla->c_date = fechaActual;
				tabla->m_date = fechaActual;

				ptrGBloque bloqueIndirecto = 0;
				uint32_t bloqueParaDatos = 0;
				uint32_t bloquesAcumulados = 0;
				void* segmentoDatos = malloc(BLOCKSIZE);

				arrayPunterosIndirectos = tabla->blk_indirect;

				int tope = 0;

				for(int i = 0; i < bloquesNecesariosIndirectos; i++){

					bloqueIndirecto = *(arrayPunterosIndirectos + i);  // Busco bloque con punteros a bloques

					if(i == bloquesNecesariosIndirectos - 1){
						tope = bloquesNecesariosDatos - ((bloquesNecesariosIndirectos - 1)*1024);
					}else{
						tope = 1024;
					}
					ptrGBloque* arrayPunterosABloques = malloc(sizeof(ptrGBloque)*tope); // Creo array con punteros a bloques

					for(int j =  0; j < tope ;j++){
						bloqueParaDatos = buscarBloqueDisponible();  // Busco bloque disponible
						*(arrayPunterosABloques + j) = bloqueParaDatos;  // Guardo en en array de punteros a bloques el numero de bloque a apuntar
						bitarray_set_bit(mapBitmap,bloqueParaDatos);  // Reservo el bloque en el bitmap
						bloquesAcumulados = (i * 1024) + j ;  // bloques ya guardados
						memcpy(segmentoDatos,(datos + (bloquesAcumulados* BLOCKSIZE)),BLOCKSIZE);
						escribirBloque(bloqueParaDatos,segmentoDatos); //Guardo Segmento en el bloque
					}
					free(arrayPunterosABloques);
				}
				free(segmentoDatos);
				Logger_Log(LOG_INFO, "Se creo el archivo: %s.", nombre);

			}else{
				Logger_Log(LOG_ERROR, "No hay espacio suficiente en el almacenamiento para guardar el archivo.");
			}

	}
	sincronizarTabla();
	sincronizarBitArray();
	//free(tabla);
}

void crearDirectorio(char* nombre, uint32_t padre){
	int numeroTablaAUsar = 0;
	GFile* tabla = malloc(sizeof(GFile));

	numeroTablaAUsar = buscarTablaDisponible();

	if(numeroTablaAUsar == -1){
		Logger_Log(LOG_ERROR, "No se pueden crear mas directorios.");
	}else{
		time_t fechaActual;
		fechaActual = time(NULL);
		tabla = devolverTabla(numeroTablaAUsar);

		tabla->state = 2;
		int largoNombre = strlen(nombre);
		memcpy(tabla->fname,nombre,largoNombre);
		tabla->parent_dir_block = padre;
		tabla->c_date = fechaActual;
		tabla->m_date = fechaActual;
	}
	sincronizarTabla();
	Logger_Log(LOG_INFO, "Se creo el directorio: %s.", nombre);
}

void crearRaiz(void){
	GFile* tabla = malloc(sizeof(GFile));
	tabla = devolverTabla(0);
	tabla->state = 2;
	memcpy(tabla->fname,"/",1);
	tabla->parent_dir_block = -1;
	sincronizarTabla();
}

void* leerArchivo(char* path){
	void* datos;
	int tabla = 0;
	tabla = localizarTablaArchivo(path);
	if(tabla == -1){
		Logger_Log(LOG_INFO, "No existe el archivo con la ruta: %s.", path);
		return -1;
	}else{
		return datos;
	}
}

int* encontrarPadres(char* nombre){
	int* padresProbables;
	int aciertos = 0;
	for(int i = 0; i < GFILEBYTABLE ; i++){
		if(*(mapTablas + i)->fname == *nombre){
			*(padresProbables + aciertos) = i;
			aciertos++;
		}
	}
	if(aciertos == 0){
		*padresProbables = -1;
	}
	return padresProbables;
}

int localizarTablaArchivo(char* path){
	int tablaArchivo = 0;
	char* extracto = malloc(GFILENAMELENGTH);
	int caracter = '/';

	while(){

	}
	return tablaArchivo;
}

void conteos(){

	uint32_t bloquesUso = 0;
	uint32_t bloquesDisponibles = 0;
	uint32_t primerBloqueDisponible = 0;
	int tablasUso = 0;
	int primerTablaVacia = 0;
	int tablasDisponibles = 0;

	bloquesDisponibles = contadorBloquesLibres();
	Logger_Log(LOG_INFO, "Bloques para datos disponibles: %lu .", bloquesDisponibles);

	bloquesUso = (int)bloquesDatos - bloquesDisponibles;
	Logger_Log(LOG_INFO, "Bloques para datos en uso: %lu .", bloquesUso);

	primerBloqueDisponible = buscarBloqueDisponible();
	Logger_Log(LOG_INFO, "Primer bloque disponible : %lu .", primerBloqueDisponible);

	tablasDisponibles = contadorTablasLibres();
	Logger_Log(LOG_INFO, "Tablas disponibles: %d .", tablasDisponibles);

	tablasUso = GFILEBYTABLE - tablasDisponibles;
	Logger_Log(LOG_INFO, "Tablas en uso: %d .", tablasUso);

	primerTablaVacia = buscarTablaDisponible();
	Logger_Log(LOG_INFO, "Primer tabla disponible : %d", primerTablaVacia);

}

void sincronizarBitArray(void){
	msync(mapBitmap,largoBitmap,MS_SYNC);
}

void sincronizarTabla(void){
	msync(mapTablas,(GFILEBYTABLE * sizeof(GFile)),MS_SYNC);
}

int main(){

	iniciarLog();

	cargarAlmacenamiento();

	char prueba[] = "ESTA ES UNA PRUEBA PARA VER SI ANDA EL NUEVO ARCHIVO";
	char prueba2[] = "ARCHIVO DE PRUEBA 2 JAJAJJAJAJA RAJAJAJA POPO CUCUCU PIPIPI";

	char nombre1 [] =  "Prueba.txt";
	char nombre2 [] =  "Test.pdf";
	char nombre3 [] =  "PrimerDirectorio";
	char ruta [] = "/dir1/dire/test/Prueba.txt";
	char* extracto = malloc(GFILENAMELENGTH);

	int caracter = '/';

	extracto = strrchr(ruta,caracter);


	int largoPrueba = strlen(prueba);
	int largoPrueba2 = strlen(prueba2);

	printf("Ulitmo nombre: %s .\n", extracto);

	int*  listaPadres;
	listaPadres = encontrarPadres((extracto+1));
	printf("Padre de %s es: %d .\n", extracto, *listaPadres);

	/*
	crearDirectorio(nombre3,0);

	archivoNuevo(nombre1,prueba,largoPrueba,1);
	archivoNuevo(nombre2,prueba2,largoPrueba2,1);

*/
	//iniciarServer();
	//apagarServer();

	descargarAlmacenamiento();

	return 0;
}

