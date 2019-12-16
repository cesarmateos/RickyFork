#include"SAC.h"


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

void archivoNuevo(char* nombre, void*datos, uint32_t tamanio, nroTabla padre){

	nroTabla numeroTablaAUsar = 0;
	GFile tabla;
	ptrGBloque* arrayPunterosIndirectos = calloc(BLKINDIRECT,sizeof(ptrGBloque));

	numeroTablaAUsar = buscarTablaDisponible();

	if(numeroTablaAUsar == -1){
		Logger_Log(LOG_ERROR, "No se pueden crear mas archivos.");
	}else{

		int bloquesNecesariosDatos = bloquesNecesarios(tamanio);
		int bloquesNecesariosIndirectos = ceil( bloquesNecesariosDatos / 1024.0 );
		int bloquesNecesariosTotales = bloquesNecesariosDatos + bloquesNecesariosIndirectos;
		int bloquesDisponibles = contadorBloquesLibres();

		time_t fechaActual;
		fechaActual = time(NULL);

			if(bloquesNecesariosTotales <= bloquesDisponibles){
				tabla = devolverTabla(numeroTablaAUsar);

				crearPunterosIndirectos(arrayPunterosIndirectos, bloquesNecesariosIndirectos);		//Se crea el puntero a bloques de punteros a bloques de datos
				printf("%u", arrayPunterosIndirectos[0]);

				tabla.state = 1;
				strcpy(tabla.fname,nombre);
				tabla.tablaPadre = padre;
				tabla.file_size = tamanio;
				tabla.c_date = fechaActual;
				tabla.m_date = fechaActual;
				for(int i = 0 ; i < BLKINDIRECT; i++){
					tabla.blk_indirect[i] = *(arrayPunterosIndirectos +i);
				}
				*(mapTablas + numeroTablaAUsar) = tabla;							//se copian los datos de la tabla nueva en el mapeo de tablas.

				free(arrayPunterosIndirectos);

				ptrGBloque bloqueIndirecto = 0;
				uint32_t bloqueParaDatos = 0;
				uint32_t bloquesAcumulados = 0;
				void* segmentoDatos = malloc(BLOCKSIZE);

				int tope = 0;

				for(int i = 0; i < bloquesNecesariosIndirectos; i++){

					bloqueIndirecto = tabla.blk_indirect[i];								// Busco bloque con punteros a bloques de datos

					if(i == bloquesNecesariosIndirectos - 1){
						tope = bloquesNecesariosDatos - ((bloquesNecesariosIndirectos - 1)*1024);	// Evalua cuantos punteros a guardar dependiendo si es o no e ultimo bloque
					}else{
						tope = GFILEBYTABLE;
					}
					ptrGBloque* arrayPunterosABloques = calloc(GFILEBYTABLE,sizeof(ptrGBloque));			// Creo array con punteros a bloques de datos

					for(int j =  0; j < tope ;j++){
						bloqueParaDatos = buscarBloqueDisponible();									// Busco bloque disponible
						*(arrayPunterosABloques + j) =  bloqueParaDatos;							// Guardo en en array de punteros a bloques el numero de bloque a apuntar
						bitarray_set_bit(mapBitmap,bloqueParaDatos);								// Reservo el bloque en el bitmap
						bloquesAcumulados = (i * 1024) + j ;										// Calculo cuantos bloques ya se han guardado.
						memset(segmentoDatos,0,BLOCKSIZE);											// Limpio la memoria para que no me queden pegados residuos.
						memcpy(segmentoDatos,(datos + (bloquesAcumulados* BLOCKSIZE)),BLOCKSIZE);	// Copio eltamaño de un bloque, desde la zona del archivo correspondiente al bloque actual
						escribirBloque(bloqueParaDatos,segmentoDatos);								// Guardo Segmento en el bloque
					}
					escribirBloque(bloqueIndirecto,arrayPunterosABloques);
					free(arrayPunterosABloques);
				}
				free(segmentoDatos);
				Logger_Log(LOG_INFO, "Se creo el archivo: %s, cuya tabla es la numero : %d", nombre, numeroTablaAUsar);

			}else{
				Logger_Log(LOG_ERROR, "No hay espacio suficiente en el almacenamiento para guardar el archivo.");
			}

	}
	//sincronizarTabla();
	//sincronizarBitArray();
}

void borrarArchivo(char* path){
	nroTabla numeroTablaAUsar = 0;
	GFile tabla;
	numeroTablaAUsar = buscarTablaDisponible();
	if(numeroTablaAUsar == -1){
		Logger_Log(LOG_ERROR, "No se pueden crear mas directorios.");
	}else{
		tabla = devolverTabla(numeroTablaAUsar);
		tabla.state = 0;
	}
}

void* leerArchivo(char* path){
	nroTabla numeroTabla = 0;
	numeroTabla = localizarTablaArchivo(path);
	if(numeroTabla == -1){
		Logger_Log(LOG_INFO, "No existe el archivo con la ruta: %s.", path);
		return -1;
	}else{
		uint32_t tamanio;
		GFile tabla;
		tabla = devolverTabla(numeroTabla);

		tamanio = tabla.file_size ;
		int bloquesALeer = bloquesNecesarios(tamanio);
		void* datos = calloc(bloquesALeer,BLOCKSIZE);
		void* datosTemporarios = malloc(BLOCKSIZE);


		ptrGBloque* datosBloqueIndirecto = malloc(sizeof(BLOCKSIZE));
		uint32_t bloque = 0;

		int i = 0;
		int k = 0;

		while(tabla.blk_indirect[i] > 0){
			bloque = tabla.blk_indirect[i];						//Busco el numero del bloque con punteros a bloques de datos
			leerBloque(bloque,datosBloqueIndirecto); 			//Leo los punteros en el bloque con punteros a bloques que datos
			int j = 0;
			while( *(datosBloqueIndirecto + j) > 0 ){			//Evaluó si el puntero tiene la direccion de algun bloque.
				bloque = *(datosBloqueIndirecto + j);			//Extraigo el numero del bloque apuntado.
				leerBloque(bloque,datosTemporarios);			//Extraigo los datos del bloque.
				memcpy((datos + k),datosTemporarios,BLOCKSIZE);	//Copio los datos extraidos a los datos consolidados.
				j++;
				k++;
			}
			i++;
		}
		//free(datosBloqueIndirecto);
		//free(datosTemporarios);
		return datos;
	}
}

void crearDirectorio(char* nombre, nroTabla padre){
	nroTabla numeroTablaAUsar = 0;
	GFile tabla;
	numeroTablaAUsar = buscarTablaDisponible();

	if(numeroTablaAUsar == -1){
		Logger_Log(LOG_ERROR, "No se pueden crear mas directorios.");
	}else{
		time_t fechaActual;
		fechaActual = time(NULL);
		tabla = devolverTabla(numeroTablaAUsar);

		tabla.state = 2;
		strcpy(tabla.fname,nombre);
		tabla.tablaPadre = padre;
		tabla.c_date = fechaActual;
		tabla.m_date = fechaActual;
	}
	*(mapTablas +numeroTablaAUsar) = tabla;
	Logger_Log(LOG_INFO, "Se creo el directorio: %s. en el bloque: %d, padre: %d", nombre,numeroTablaAUsar,padre);
}

char** leerDirectorio(char* path){
	nroTabla numeroTabla = 0;
	numeroTabla = localizarTablaArchivo(path);
	if(numeroTabla == -1){
		Logger_Log(LOG_INFO, "No existe el directorio con la ruta: %s.", path);
		return -1;
	}else{
		char** listadoDeArchivos = calloc(GFILEBYTABLE,sizeof(char*));
		int j = 0;
		for(int i = 0 ; i < GFILEBYTABLE ; i++){
			if((mapTablas + i)->state !=0 && (mapTablas + i)->tablaPadre == numeroTabla){
				//listadoDeArchivos[j] = calloc(GFILENAMELENGTH,sizeof(char));
				*(listadoDeArchivos + j) = (mapTablas + i)->fname;
				j++;
			}
		}
		return listadoDeArchivos;

	}
}

void renombrar(char* path, char* nuevoNombre){
	nroTabla numeroTabla = 0;
	numeroTabla = localizarTablaArchivo(path);
	GFile tabla;
	if(numeroTabla == -1){
		Logger_Log(LOG_INFO, "No existe: %s.", path);
	}else{
		tabla = devolverTabla(numeroTabla);
		strcpy(tabla.fname,nuevoNombre);
		*(mapTablas + numeroTabla) = tabla;
		//memcpy((mapTablas + numeroTabla)->fname , nuevoNombre, GFILENAMELENGTH);
		Logger_Log(LOG_INFO, "Se renombro: %s. a %s", path,nuevoNombre);
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

	char prueba[] = "ESTA ES UNA PRUEBA PARA VER SI ANDA EL NUEVO ARCHIVO";
	char prueba2[] = "ARCHIVO DE PRUEBA 2 JAJAJJAJAJA RAJAJAJA POPO CUCUCU PIPIPI";

	char nombre1 [] =  "Prueba.txt";
	char nombre2 [] =  "Test.pdf";
	char nombre3 [] =  "PrimerDirectorio";
	char nombre4 [] =  "SegundoDirectorio";
	char nombre5 [] =  "TercerDirectorio";

	int largoPrueba = strlen(prueba);
	int largoPrueba2 = strlen(prueba2);

	crearDirectorio(nombre3,0);
	crearDirectorio(nombre4,1);
	crearDirectorio(nombre5,2);

	archivoNuevo(nombre1,prueba,largoPrueba,3);
	archivoNuevo(nombre2,prueba2,largoPrueba2,3);
	archivoNuevo(nombre1,prueba,largoPrueba,2);

	int tablaArch = 0;
	tablaArch = localizarTablaArchivo(ruta);
	printf("La tabla del archivo %s es la: %d .\n", ruta, tablaArch);

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
	//CommandInterpreter_Init();

	iniciarServer();
	//apagarServer();
	//descargarAlmacenamiento();

	return 0;
}

