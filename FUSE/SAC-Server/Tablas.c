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


int localizarTablaArchivo(char* ruta){
	if(strlen(ruta) == 0){
		return 0;
	}
	nroTabla numeroTablaPadre = 0;
	size_t tamanio = strlen(ruta);
	int tamanioPrimerSobrante = 0;
	int tamanioPrimerExtracto = 0;
	int tamanioRestoSobrante = 0;
	char* primerExtracto = malloc(GFILENAMELENGTH);
	char* primerSobrante = malloc(tamanio);
	char* restoExtracto = malloc(GFILENAMELENGTH);
	char* restoSobrante = malloc(tamanio);
	int caracter = '/';
	GFile tabla;

	separar(ruta,primerSobrante,primerExtracto);
	tamanioPrimerSobrante = strlen(primerSobrante);

	memcpy(restoSobrante, primerSobrante, tamanioPrimerSobrante);
	*(restoSobrante + tamanioPrimerSobrante) = '\0';

	for(nroTabla i = 0; i < GFILEBYTABLE ; i++){
		tabla = devolverTabla(i);
		if(strcmp(tabla.fname , primerExtracto )  == 0 && tabla.state != 0){
			tamanioRestoSobrante = tamanioPrimerSobrante;
			numeroTablaPadre = tabla.tablaPadre;
			do{
				if(tamanioRestoSobrante == 0 && numeroTablaPadre == 0){
					free(primerExtracto);
					free(primerSobrante);
					free(restoExtracto);
					free(restoSobrante);
					return i;
				}
				separar(restoSobrante,restoSobrante,restoExtracto);
				tabla = devolverTabla(numeroTablaPadre);
				numeroTablaPadre = tabla.tablaPadre;
				tamanioRestoSobrante = strlen(restoSobrante);
			}while(strcmp(tabla.fname, restoExtracto) == 0);
		}
		memcpy(restoSobrante, primerSobrante, tamanioPrimerSobrante);
		*(restoSobrante + tamanioPrimerSobrante) = '\0';
	}
	free(primerExtracto);
	free(primerSobrante);
	free(restoExtracto);
	free(restoSobrante);
	return -1;
}


void sincronizarTabla(void){
	msync(mapTablas,(GFILEBYTABLE * sizeof(GFile)),MS_SYNC);
}

char* sobrante(char* ruta){
	int caracter = '/';
	int largoRuta = strlen(ruta);
	char* extracto;
	extracto = strrchr(ruta,caracter);
	int largoExtracto  = strlen(extracto);
	int largoSobrante = largoRuta - largoExtracto;
	char* sobrante = malloc(largoSobrante);
	memcpy(sobrante,ruta,largoRuta);
	*(sobrante + largoSobrante) = '\0';
	return *sobrante;
}

char* extracto(char* ruta){
	int caracter = '/';
	char* extracto;
	extracto = strrchr(ruta,caracter);
	//*(extracto + strlen(extracto)) = '\0';
	return *(extracto +1);
}

void separar(char*ruta, char* sobrante, char* extracto){
	int caracter = '/';
	int largoRuta = strlen(ruta);
	char* extractoFun;
	extractoFun = strrchr(ruta,caracter);
	int largoExtracto  = strlen(extractoFun);
	strncpy(extracto, (extractoFun + 1) , largoExtracto);
	int largoSobrante = largoRuta - largoExtracto;
	memcpy(sobrante,ruta,largoRuta);
	*(sobrante + largoSobrante) = '\0';
}



