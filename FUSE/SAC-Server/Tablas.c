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

void sincronizarTabla(void){
	msync(mapTablas,(GFILEBYTABLE * sizeof(GFile)),MS_SYNC);
}

