#include "Operaciones.h"

void archivoNuevo(char* nombre, void*datos, uint32_t tamanio, nroTabla padre){

	nroTabla numeroTablaAUsar = 0;
	GFile tabla;
	ptrGBloque* arrayPunterosIndirectos = calloc(BLKINDIRECT,sizeof(ptrGBloque));

	numeroTablaAUsar = buscarTablaDisponible();

	if(numeroTablaAUsar == -1){
		Logger_Log(LOG_ERROR, "No se pueden crear mas archivos.");
	}else{

		uint32_t bloquesNecesariosDatos = bloquesNecesarios(tamanio);
		uint32_t bloquesNecesariosIndirectos = ceil( bloquesNecesariosDatos / 1024.0 );
		uint32_t bloquesNecesariosTotales = bloquesNecesariosDatos + bloquesNecesariosIndirectos;
		uint32_t bloquesDisponibles = contadorBloquesLibres();

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

void borrarArchivo(char* ruta){
	if(existe(ruta)){
		nroTabla numeroTabla = localizarTablaArchivo(ruta);
		GFile tabla = devolverTabla(numeroTabla);
		tabla.state = 0;
		liberarPunterosABloques(tabla,0);
	}else{
		Logger_Log(LOG_ERROR, "No existe %s.", ruta);
	}
}


void* leerArchivo(char* ruta){

	if(existe(ruta)){
		nroTabla numeroTabla = 0;
		uint32_t tamanio;
		GFile tabla;

		numeroTabla = localizarTablaArchivo(ruta);
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
	}else{
		Logger_Log(LOG_INFO, "No existe el archivo con la ruta: %s.", ruta);
		return -1;
	}
}

void escribirArchivo(char* ruta, size_t tamanio,  off_t offset, void*datos){
	if(existe(ruta)){
		nroTabla numeroTabla = localizarTablaArchivo(ruta);
		GFile tabla = devolverTabla(numeroTabla);

		uint32_t tamanioAnterior = tabla.file_size;
		uint32_t bloquesUsados = bloquesOcupados(tamanioAnterior);

		uint32_t bloquesNecesariosDatos = bloquesNecesarios( tamanio + offset);
		uint32_t bloquesNecesariosIndirectos = ceil( bloquesNecesariosDatos / 1024.0 );
		uint32_t bloquesNecesariosTotales = bloquesNecesariosDatos + bloquesNecesariosIndirectos;
		uint32_t bloquesDisponibles = contadorBloquesLibres();

		if( bloquesNecesariosTotales > (bloquesDisponibles + bloquesUsados) ){
			Logger_Log(LOG_ERROR, "No hay espacio suficiente para guardar los cambios en el archivo %s.", ruta);
		}else{
			ptrGBloque primerBloque = 0;
			int* posicionPunterosTabla = malloc(sizeof(int));
			int* posicionPunterosIndirectos = malloc(sizeof(int));
			void* segmentoDatos = malloc(BLOCKSIZE);
			void* datosPrimerBloque = malloc(BLOCKSIZE);
			int* offsetBloque = malloc(BLOCKSIZE);
			int restoBloque = 0;
			uint32_t bloquesDatosAcumulados = 0; //(i * 1024) + j ;
			ptrGBloque* arrayPunterosABloques = calloc(GFILEBYTABLE,sizeof(ptrGBloque));
			uint32_t bloqueIndirecto = 0;


			primerBloque = buscarBloqueOffset(tabla,posicionPunterosTabla,posicionPunterosIndirectos,offset,offsetBloque);	// Busco hasta que bloque no se modifica nada
			restoBloque = BLOCKSIZE - *offsetBloque;  																		// Calculo cuanto me falta para llenar el primer bloque

			//Llenamos el bloque correspondiente al offset//
			leerBloque(primerBloque,datosPrimerBloque);
			memcpy(datosPrimerBloque + *offsetBloque, datos,restoBloque);
			escribirBloque(primerBloque,datosPrimerBloque);
			free(datosPrimerBloque);

			//Llenamos los bloques apuntados por el pirmer bloque indirecto//
			bloqueIndirecto =  tabla.blk_indirect[*posicionPunterosTabla];
			leerBloque(bloqueIndirecto,arrayPunterosABloques);
			int i = (*posicionPunterosIndirectos) + 1;
			int j = (*posicionPunterosTabla) + 1;

			bloquesDatosAcumulados = ((*posicionPunterosTabla) * GFILEBYTABLE) + j ;		//Calculo cuantos bloques de los necesarios ya fueron llenados.

			int bloquesAcumuladosNuevos = 0;

			while( i < 1024 && (bloquesDatosAcumulados < bloquesNecesariosDatos)){
				memset(segmentoDatos,0,BLOCKSIZE);
				memcpy(segmentoDatos,(datos + (bloquesAcumuladosNuevos* BLOCKSIZE) + restoBloque),BLOCKSIZE);
				escribirBloque(*(arrayPunterosABloques + i),segmentoDatos);
				bloquesDatosAcumulados++;
				bloquesAcumuladosNuevos++;
				i++;
			}

			//Borro bloques a ser pisados
			librearPunterosABloques(tabla, j);

			llenarBloques(void* datos, tabla, uint32_t tamanio, j);

		}
	}else{
		Logger_Log(LOG_ERROR, "El archivo %s no existe.", ruta);
	}
}

void crearDirectorio(char* ruta){
	if(existe(ruta)){
		Logger_Log(LOG_ERROR, "El directorio que se quiere crear ya existe");

	}else{
		char* nombre;// = malloc(sizeof(nombreArchivo));
			nroTabla numeroTablaAUsar = 0;
			nroTabla padre = 0;
			GFile tabla;
			char* sobrante = malloc(sizeof(rutaArchivo));
			int largoNombre = 0;
			int largoSobrante = 0;
			int largoRuta = strlen(ruta);
			int caracter = '/';

			nombre = strrchr(ruta, caracter);
			largoNombre = strlen(nombre);
			largoSobrante = largoRuta  - largoNombre;
			strncpy(sobrante,ruta,largoSobrante);
			*(sobrante+largoSobrante) = '\0';

			padre = localizarTablaArchivo(sobrante);
			numeroTablaAUsar = buscarTablaDisponible();

			if(numeroTablaAUsar == -1){
				Logger_Log(LOG_ERROR, "No se pueden crear mas directorios.");
			}else{
				time_t fechaActual;
				fechaActual = time(NULL);
				tabla = devolverTabla(numeroTablaAUsar);

				tabla.state = 2;
				strcpy(tabla.fname,(nombre+1));
				tabla.tablaPadre = padre;
				tabla.c_date = fechaActual;
				tabla.m_date = fechaActual;
			}
			*(mapTablas +numeroTablaAUsar) = tabla;
			Logger_Log(LOG_INFO, "Se creo el directorio: %s. en el bloque: %d, padre: %d", nombre,numeroTablaAUsar,padre);
			//free(nombre);
			free(sobrante);
	}

}

char** leerDirectorio(char* ruta){

	if(existe(ruta)){
		nroTabla numeroTabla = 0;
		char** listadoDeArchivos = calloc(GFILEBYTABLE,sizeof(char*));
		int j = 0;
		for(int i = 0 ; i < GFILEBYTABLE ; i++){
			if((mapTablas + i)->state !=0 && (mapTablas + i)->tablaPadre == numeroTabla){
				*(listadoDeArchivos + j) = (mapTablas + i)->fname;
				j++;
			}
		}
		return listadoDeArchivos;
	}else{

		Logger_Log(LOG_INFO, "No existe el directorio con la ruta: %s.", ruta);
		return -1;
	}
}

void borrarDirectorio(char* ruta){

	if(existe(ruta)){
		tablaOFF(ruta);
		nroTabla numeroTabla = localizarTablaArchivo(ruta);
		GFile tabla = devolverTabla(numeroTabla);
		for(int i = 0 ; i < GFILEBYTABLE ; i++){
			if((mapTablas + i)->tablaPadre == numeroTabla){
				tabla = devolverTabla(i);
				tabla.state = 0;
			}
		}
		Logger_Log(LOG_INFO, "Se elimino el directorio %s y todas sus subCarpetas y archivos.", ruta);
	}else{
		Logger_Log(LOG_ERROR, "No existe %s.", ruta);
	}
}

void borrarDirectorioVacio(char* ruta){
	if(existe(ruta)){
		char** listaArchivos;
		listaArchivos = leerDirectorio(ruta);
		int largo = strlen(listaArchivos[0]);
		if(largo>0){
			tablaOFF(ruta);
			Logger_Log(LOG_INFO, "Se elimino el directorio %s y todas sus subCarpetas y archivos.", ruta);
		}else{
			Logger_Log(LOG_ERROR, "Se intento borrar el directorio %s. y no esta vacio", ruta);
		}
	}else{
		Logger_Log(LOG_ERROR, "No existe %s.", ruta);
	}

}

void renombrar(char* ruta, char* nuevoNombre){

	if(existe(ruta)){
		nroTabla numeroTabla = 0;
		numeroTabla = localizarTablaArchivo(ruta);
		GFile tabla;
		tabla = devolverTabla(numeroTabla);
		strcpy(tabla.fname,nuevoNombre);
		*(mapTablas + numeroTabla) = tabla;
		Logger_Log(LOG_INFO, "Se renombro: %s. a %s", ruta,nuevoNombre);
	}else{
		Logger_Log(LOG_INFO, "No existe %s.", ruta);
	}
}

bool existe(char* ruta){
	nroTabla numeroTabla;
	numeroTabla = localizarTablaArchivo(ruta);
	if(numeroTabla < 0 ){
		return false;
	}
	GFile tabla;
	tabla = devolverTabla(numeroTabla);
	if(tabla.state > 0 ){
		return false;
	}
	return true;
}

void llenarBloques(void* datos, GFile tabla, uint32_t tamanio, int pirmerPunteroTabla){
	uint32_t bloquesNecesariosDatos = bloquesNecesarios(tamanio);
	uint32_t bloquesAcumulados = 0;
	ptrGBloque bloqueIndirecto = 0;
	ptrGBloque bloqueDatos = 0;
	int i = pirmerPunteroTabla;
	int j = 0;
	ptrGBloque* arrayPunterosABloques = calloc(GFILEBYTABLE,sizeof(ptrGBloque));
	void* segmentoDatos = malloc(BLOCKSIZE);

	//Limpio el resto de los punteros de la tabla por las dudas.
	for(int h = pirmerPunteroTabla ; h < 1000; h++){
		tabla.blk_indirect[h] = 0;
	}

	//Guardo Datos
	while(bloquesAcumulados < bloquesNecesariosDatos && i < 1000){
		j = 0;
		bloqueIndirecto = tabla.blk_indirect[i];
		while(bloquesAcumulados < bloquesNecesariosDatos && j < 1024){

			bloqueDatos = buscarBloqueDisponible();										// Busco bloque disponible
			*(arrayPunterosABloques + j) =  bloqueDatos;								// Guardo en en array de punteros a bloques el numero de bloque a apuntar
			bitarray_set_bit(mapBitmap,bloqueDatos);									// Reservo el bloque en el bitmap
			memset(segmentoDatos,0,BLOCKSIZE);											// Limpio la memoria para que no me queden pegados residuos.
			memcpy(segmentoDatos,(datos + (bloquesAcumulados* BLOCKSIZE)),BLOCKSIZE);	// Copio eltamaño de un bloque, desde la zona del archivo correspondiente al bloque actual
			escribirBloque(bloqueDatos,segmentoDatos);									// Guardo Segmento en el bloque

			bloquesAcumulados++;
			j++;
		}
		escribirBloque(bloqueIndirecto,arrayPunterosABloques);
		memset(arrayPunterosABloques,0,sizeof(arrayPunterosABloques));					//Limpio array por las dudas
		i++;
	}
	free(arrayPunterosABloques);
	free(segmentoDatos);

}
