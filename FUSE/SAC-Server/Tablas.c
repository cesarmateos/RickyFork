#include "Tablas.h"

void mapearTablas(void){
	//mapTablas = calloc(GFILEBYTABLE, sizeof(GFile));
	mapTablas = (void*) mmap(NULL,(GFILEBYTABLE * sizeof(GFile)) ,PROT_READ|PROT_WRITE,MAP_SHARED,disco, (inicioTablas *BLOCKSIZE) );
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

int contadorTablasLibres(void){
	int contador = 0;
	for(int i = 0 ; i < GFILEBYTABLE ; i++){
		if((int)(mapTablas + i)->state == 0){
			contador++;
		}
	}
	return contador;
}

GFile devolverTabla(nroTabla numeroTabla){
	return (*(mapTablas + numeroTabla));
}

void crearRaiz(void){
	GFile tabla;
	tabla = devolverTabla(0);
	tabla.state = 2;
	tabla.fname[0] = '/';
	tabla.fname[1] = '\0';
	tabla.tablaPadre = -1;
	*mapTablas = tabla;
	sincronizarTabla();
}

void crearPunterosIndirectos(ptrGBloque* punteros, int cantidad){
	ptrGBloque bloqueVacio = 0;
	for(int i = 0; i < cantidad ; i++){
		bloqueVacio = buscarBloqueDisponible();				//Busco bloque Disponible.
		punteros[i] = bloqueVacio;							//Copio el numero de bloque disponible en el array.
		bitarray_set_bit(mapBitmap,bloqueVacio);			//Reservo el bloque.
	}
}

void crearPunterosIndirectosOLd(GFile *tabla, int cantidad){
	ptrGBloque* arrayPunterosIndirectos = calloc(BLKINDIRECT, sizeof(ptrGBloque));
	ptrGBloque bloqueVacio = 0;
	for(int i = 0; i < cantidad ; i++){
		bloqueVacio = buscarBloqueDisponible();				//Busco bloque Disponible.
		*(arrayPunterosIndirectos + i) = bloqueVacio;		//Copio el numero de bloque disponible en el array.
		bitarray_set_bit(mapBitmap,bloqueVacio);			//Reservo el bloque.
	}
	memcpy(tabla->blk_indirect,arrayPunterosIndirectos,(cantidad * sizeof(ptrGBloque)));
	free(arrayPunterosIndirectos);
}


int localizarTablaArchivo(char* path){
	nroTabla numeroTablaPadre = 0;
	size_t tamanio = strlen(path);
	int tamanioSobrante = 0;
	int tamanioExtracto = 0;
	char* extracto;
	char* sobrante = malloc(tamanio);
	char* segmentoActual = malloc(GFILENAMELENGTH);
	int caracter = '/';
	char seg[GFILENAMELENGTH];
	GFile tabla;

	extracto = strrchr(path,caracter);								//Extraigo el nombre del archivo
	tamanioExtracto =  strlen(extracto);							//Calculo el largo del nombre del archivo
	tamanioSobrante = tamanio - tamanioExtracto;					//Calculo el largo del resto de la ruta
	memcpy(segmentoActual,extracto,tamanioExtracto);
	memcpy(sobrante, path, tamanioSobrante);						//Copio en sobrante el resto de la ruta.


	for(nroTabla i = 0; i < GFILEBYTABLE ; i++){
		numeroTablaPadre = i;
		tabla = devolverTabla(numeroTablaPadre);
		for(int z = 0 ; z < (tamanioExtracto -1) ; z++){
			seg[z] = *(segmentoActual + z + 1 );
		}
			seg[tamanioExtracto -1] = '\0';

		while(strcmp(tabla.fname , seg)  == 0 ){
			*(sobrante + tamanioSobrante) = '\0';						//Pongo en sobrante el caracter nulo
			*(segmentoActual + tamanioExtracto) = '\0';
			numeroTablaPadre = tabla.tablaPadre;						//Asgino la tabla Padre al numero de tabla
			if(tamanioSobrante == 0 && numeroTablaPadre == 0){
				free(segmentoActual);
				free(sobrante);
				return i;
			}else{
				tabla = devolverTabla(numeroTablaPadre);
				extracto = strrchr(sobrante,caracter);					//Extraigo el siguiente directorio de la ruta
				tamanioExtracto =  strlen(extracto);					//Calculo el largo el nuevo extracto.
				memcpy(segmentoActual,extracto,tamanioExtracto);
				tamanioSobrante -= tamanioExtracto;						//Calculo el largo del resto de la ruta.
				for(int z = 0 ; z < tamanioExtracto-1 ; z++){
					seg[z] = *(segmentoActual + z + 1 );
				}
					seg[tamanioExtracto-1] = '\0';
			}
		}
		extracto = strrchr(path,caracter);								//Extraigo el nombre del archivo
		tamanioExtracto =  strlen(extracto);							//Calculo el largo del nombre del archivo
		tamanioSobrante = tamanio - tamanioExtracto;					//Calculo el largo del resto de la ruta
		memcpy(segmentoActual,extracto,tamanioExtracto);
		memcpy(sobrante, path, tamanioSobrante);						//Copio en sobrante el resto de la ruta.
	}
	free(segmentoActual);
	free(sobrante);
	return -1;
}

void sincronizarTabla(void){
	msync(mapTablas,(GFILEBYTABLE * sizeof(GFile)),MS_SYNC);
}

