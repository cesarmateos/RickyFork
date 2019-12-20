#include "Bloques.h"

void mapearBitmap(void){
	char* bitArrayTemp;
	//mapBitmap = calloc(largoBitmap,sizeof(char));
	bitArrayTemp = (char*) mmap(NULL,largoBitmap,PROT_READ|PROT_WRITE,MAP_SHARED,disco, ((*inicioBitmap) * BLOCKSIZE) );
	mapBitmap = bitarray_create(bitArrayTemp, largoBitmap );
}

void leerBloque(uint32_t bloque, void* datos){
	void *extrae;
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

ptrGBloque buscarBloqueDisponible(void){
	ptrGBloque  indice = GFILEBYTABLE;
	ptrGBloque  largoBitmap = bitarray_get_max_bit(mapBitmap);
	while(bitarray_test_bit(mapBitmap,indice) & ( indice < largoBitmap ) ){
		indice++;
	}
	if(indice == largoBitmap){
		Logger_Log(LOG_ERROR, "Disco lleno, no hay bloques vacios.");
		return -1;
	}else{
		return indice;
	}
}

uint32_t contadorBloquesLibres(void){
	uint32_t contador = 0;
	for(uint32_t i = 0 ; i < largoBitmap ; i++){
		contador += bitarray_test_bit(mapBitmap,i);
	}
	return largoBitmap - contador;
}

uint32_t bloquesNecesarios(uint32_t tamanio){
	return ceil( (tamanio + 0.0 ) /BLOCKSIZE);
}

uint32_t bloquesOcupados(uint32_t tamanio){
	uint32_t bloquesNecesariosDatos = bloquesNecesarios(tamanio);
	uint32_t bloquesNecesariosIndirectos = ceil( bloquesNecesariosDatos / 1024.0 );
	uint32_t bloquesNecesariosTotales = bloquesNecesariosDatos + bloquesNecesariosIndirectos;
	return bloquesNecesariosTotales;
}

void sincronizarBitArray(void){
	msync(mapBitmap,largoBitmap,MS_SYNC);
}

ptrGBloque buscarBloqueOffset(GFile tabla, int* posicionPunterosTabla, int*posicionPunterosIndirectos, off_t offset, int* offsetBloque){
	ptrGBloque bloqueDatos = 0;
	ptrGBloque bloqueIndirecto = 0 ;
	ptrGBloque* arrayPunterosABloques = calloc(GFILEBYTABLE,sizeof(ptrGBloque));
	int bytesDatosXBloqueIndirecto = 0;
	uint32_t bytesPreviosOffset = 0;
	uint32_t bytesBloquesPunterosIndirectos = 0;

	bytesDatosXBloqueIndirecto = GFILEBYTABLE * BLOCKSIZE;
	bytesPreviosOffset = sizeof(off_t) *offset;

	*posicionPunterosTabla = ceil (bytesPreviosOffset /  (bytesDatosXBloqueIndirecto + 0.0) );
	bloqueIndirecto =  tabla.blk_indirect[*posicionPunterosTabla];

	bytesBloquesPunterosIndirectos = bytesPreviosOffset %  bytesDatosXBloqueIndirecto;
	*posicionPunterosIndirectos = ceil(bytesBloquesPunterosIndirectos / BLOCKSIZE );


	leerBloque(bloqueIndirecto,arrayPunterosABloques);
	bloqueDatos = *(arrayPunterosABloques + *posicionPunterosIndirectos);
	free(arrayPunterosABloques);

	*offsetBloque = bytesBloquesPunterosIndirectos % BLOCKSIZE ;

	return bloqueDatos;
}

void liberarBloqueIndirecto(ptrGBloque bloqueIndirecto){
	liberarPunterosBloquesDatos(bloqueIndirecto,0);
	borrarBloque(bloqueIndirecto);
}

void liberarPunterosBloquesDatos(ptrGBloque bloqueIndirecto, int primerPuntero){
	ptrGBloque* arrayPunterosABloques = calloc(GFILEBYTABLE,sizeof(ptrGBloque));
	leerBloque(bloqueIndirecto,arrayPunterosABloques);
	uint32_t bloqueDatos = 0;
	int j = primerPuntero;
	while( *(arrayPunterosABloques + j) > 0 && j < GFILEBYTABLE){
		bloqueDatos = *(arrayPunterosABloques + j);
		borrarBloque(bloqueDatos);
		j++;
	}
	free(arrayPunterosABloques);
}





