#include <stdio.h>
#include <stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/bitarray.h>
#include<commons/config.h>

typedef int BlockSize[4096];
typedef int ptrGBloque;
t_log* logger;

typedef struct {
	char identificador[3];
	int version;
	ptrGBloque bloqueInicioBitmap;
	int tamanio;
	char relleno[4081]; // Ver
}Header;

typedef struct {
	char estado;  // No se como representar un byte si no es con un char
	char nombreArchivo[71];
	ptrGBloque bloquePadre;
	int tamanio;
	double fechaCreacion; //cambiar
	double fechaModificacion; //cambiar
	ptrGBloque arrayPunteros[1000];
}TablaNodos;

typedef struct{
	Header encabezado;
	t_bitarray bitmap; //ver como cuerno funciona (es de la commons)
	TablaNodos tablaNodos;
	BlockSize bloqueDatos;
}HD;


void crearHD(char nombre[],int tamanio){
	int tamanioMaximoDeHD = 16384; //en Gigabytes
	if(tamanio > tamanioMaximoDeHD){
		printf("El tamaño del disco supera el máximo soportado\n");
		log_error(logger, "No se pudo crear disco rígido debido a que superaba el tamaño máximo soportado\n");
	}else{
		printf("Disco creado con éxtito\n");
		log_info(logger,"Disco %s de %d Gigabytes creado exitosamente.\n",nombre,tamanio);
	}
}

void seleccionarHD(char path[]){
	log_info(logger, "Se seleccionó el disco %s.\n",path);
}

int main(){

	logger = log_create("SAC-Servicor.log","SAC-servidor.c",1,LOG_LEVEL_INFO);


	return 0;
}
