#include "Inicio.h"

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

void descargarAlmacenamiento(void){

	conteos();
	sincronizarTabla();
	sincronizarBitArray();
	free(bloquesBitmap);
	munmap(mapBitmap, largoBitmap);
	munmap(mapTablas,sizeof(GFile) * GFILEBYTABLE);
	free(inicioBitmap);
	bitarray_destroy(mapBitmap);
	close(disco);
	Logger_Log(LOG_INFO, "Se ha descargado el Almacenamiento");
}

void leerHead(void){

	if(disco == NULL){
		Logger_Log(LOG_INFO, "No se pudo leer el head, no hay almacenamiento cargado.");
	}else{

	void* temporario = malloc(BLOCKSIZE);
	char* nombreFS = calloc(4,sizeof(char));
	uint32_t* versionFS = malloc(sizeof(uint32_t));
	inicioBitmap = malloc(sizeof(uint32_t));
	bloquesBitmap = malloc(sizeof(uint32_t));

	leerBloque(0,temporario);

	memcpy(nombreFS,temporario,sizeof(char)*4);
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

	bloquesDatos = largoBitmap - 1025 - (uint32_t)(*bloquesBitmap);
	Logger_Log(LOG_INFO, "Bloques para datos totales: %lu.", bloquesDatos);

	mapearTablas();
	mapearBitmap();

	crearRaiz();

	conteos();

	free(temporario);
	}
}

void conteos(void){

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

int main(){

	iniciarLog();

	cargarAlmacenamiento();

/*
	char* ruta = "/PrimerDirectorio/SegundoDirectorio/Prueba.txt";
	char* ruta2 = "/PrimerDirectorio/SegundoDirectorio/TercerDirectorio/Prueba.txt";
	char* ruta4 = "/PrimerDirectorio/SegundoDirectorio/OtroDir";
	char* ruta3 = "/PrimerDirectorio/SegundoDirectorio/TercerDirectorio/CuartoDirectorio";

	char prueba[] = "ESTA ES UNA PRUEBA PARA VER SI ANDA EL NUEVO ARCHIVO";
	char prueba2[] = "ARCHIVO DE PRUEBA 2 JAJAJJAJAJA RAJAJAJA POPO CUCUCU PIPIPI";

	char nombre1 [] =  "Prueba.txt";
	char nombre2 [] =  "Test.pdf";
	char nombre3 [] =  "/PrimerDirectorio";
	char nombre4 [] =  "/PrimerDirectorio/SegundoDirectorio";
	char nombre5 [] =  "/PrimerDirectorio/SegundoDirectorio/TercerDirectorio";

	int largoPrueba = strlen(prueba);
	int largoPrueba2 = strlen(prueba2);
/*
	int tablaArch = 0;
	tablaArch = localizarTablaArchivo2(ruta2);
	printf("La tabla del archivo 2 %s es la: %d .\n", ruta2, tablaArch);
*/
/*
	crearDirectorio(nombre3);
	crearDirectorio(nombre4);
	crearDirectorio(nombre5);
*/
/*
	archivoNuevo(nombre1,prueba,largoPrueba,3);
	archivoNuevo(nombre2,prueba2,largoPrueba2,3);
	archivoNuevo(nombre1,prueba,largoPrueba,2);
*/
/*
	int tablaArch = 0;
	tablaArch = localizarTablaArchivo(ruta);
	printf("La tabla del archivo %s es la: %d .\n", ruta, tablaArch);
*/
/*
	tablaArch = localizarTablaArchivo(ruta2);
	printf("La tabla del archivo %s es la: %d .\n", ruta2, tablaArch);

	char* datos;
	datos = leerArchivo(ruta);
	printf("El archivo %s dice: %s .\n", ruta, datos);

	renombrar(ruta2, "Pajarito.jpg");

	char** lectura;
	lectura = leerDirectorio("/PrimerDirectorio/SegundoDirectorio/TercerDirectorio");

	for(int i = 0 ; i < 2 ; i++){
		printf("%s .\n",lectura[i]);
	}
	free(lectura);
*/

	//iniciarServer();
	//apagarServer();
	//descargarAlmacenamiento();

	int largo = 0;

	largo = strlen((char*) mapTablas);
	printf("%s .\n",largo);

	return 0;
}

